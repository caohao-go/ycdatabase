local mysql = require "resty.mysql"
local cjson = require "cjson"

local assert = assert
local setmetatable = setmetatable
local tonumber = tonumber

-- 解析请求
local function parse_request(sock)
    --获取 sql 语句
    local sql_size, err = sock:receive()
    
    if not sql_size then
    	if err == "timeout" then
            sock:close()
        end
		return nil, err
    end
    
    local size = tonumber(sql_size)
    if size <= 0 then
		return nil, "SQL size is zero"
	end
	
    local sql_str, err = sock:receive(size)
    if not sql_str then
    	if err == "timeout" then
            sock:close()
        end
		return nil, err
	end
	
	--获取 map
	local map_size, err = sock:receive()
		
    if not map_size then
        if err == "timeout" then
            sock:close()
        end
        return nil, err
    end
    
    size = tonumber(map_size);
    if size <= 0 then
    	-- 没有 map
		return sql_str
	end
	
    local map_res, err = sock:receive(map_size)
    if not map_res then
    	if err == "timeout" then
            sock:close()
        end
		return nil, err
	end
	
	-- 解析 map ，创建 SQL 语句，防止SQL注入
	local maps = cjson.decode(map_res)
	
	for k, v in pairs(maps) do
		if v == true then
			v = 1
		elseif v == false then
			v = 0
		end
		
		sql_str = ngx.re.gsub(sql_str, k, ngx.quote_sql_str(v))
	end
	
    return sql_str
end

-- 返回请求
local function response_success(sock, result)
	local ret = {
		errno = 0,
		data = result
	}
	
	local send_str = cjson.encode(ret)
	
	local ret, err = sock:send(string.len(send_str) ..  "\n" .. send_str)
	
	if not ret then
		ngx.log(ngx.ERR, "response success failed : [", err, "], send_str=[", send_str, "]")
		return nil, err
	end
end

-- 返回请求
local function response_error(sock, errno, errmsg, sqlstate)
	local ret = {
		errno = errno,
		errorCode = sqlstate,
		errorInfo = {sqlstate, errno, errmsg}
	}
	
	local send_str = cjson.encode(ret)
	local ret, err = sock:send(string.len(send_str) ..  "\n" .. send_str)
	
	if not ret then
		ngx.log(ngx.ERR, "response error failed : [", err, "], send_str=[", send_str, "]")
		return nil, err
	end
end

-- 关闭数据库
local function close_db(db)
	if not db then
		return
	end
	db:close()
end

-- 异常退出
local function exit(err)
    ngx.log(ngx.ERR, "ERROR EXIT: [", err, "]")
    return ngx.exit(ngx.ERROR)
end

----------------------------------------
local _M = {}
_M._VERSION = "1.0"

function _M.new(self, config)
    local t = {
        _host = config.host,
        _port = config.port or 3306,
        _user = config.user,
        _password = config.password,
        _database = config.database,
        _timeout = config.timeout or 2000,  -- default 2 sec
        _pool_size = config.pool_size or 100,
        _max_idle_timeout = config.max_idle_timeout or 10000
    }
    
    return setmetatable(t, { __index = _M })
end

function _M.run(self)
	local downstream_sock = assert(ngx.req.socket(true))
	
	local query_sql, err = parse_request(downstream_sock)
	
	if not query_sql then
		return exit("parse_request failed : " .. err)
	end
        
	--数据库连接
	local db, err = mysql:new()
	    
	db:set_timeout(self._timeout)
	
	local ok, err, errno, sqlstate = db:connect{
		host = self._host,
		port = self._port,
		database = self._database,
		user = self._user,
		password = self._password,
		max_packet_size = 1024 * 1024}
	
	if not ok then
		response_error(downstream_sock, -1, err, "E0001")
		return exit("connect mysql error : " .. err)
	end
	
	local result, err, errno, sqlstate = db:query(query_sql)
	
	-- 返回结果
	if result then
		response_success(downstream_sock, result)
	else
		ngx.log(ngx.ERR, "query failed: [", errno, "][", sqlstate, "][",err , "]")
		response_error(downstream_sock, errno, err, sqlstate)
	end
	
	-- 设置 mysql 连接池
	local ok, err = db:set_keepalive(self._max_idle_timeout, self._pool_size)
	if not ok then
		ngx.log(ngx.ERR, "set_keepalive failed: [", err, "]")
	end
end

return _M
