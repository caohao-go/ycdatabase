local redis = require "resty.redis"

local assert = assert
local rawget = rawget
local setmetatable = setmetatable
local tonumber = tonumber
local byte = string.byte
local sub = string.sub

-- 解析请求
local function parse_request(sock)
    local line, err = sock:receive()
    
    if not line then
        if err == "timeout" then
            sock:close()
        end
        return nil, err
    end
    
    local result = line .. "\r\n"
    local prefix = byte(line)
    
    if prefix == 42 then -- char '*'
        local num = tonumber(sub(line, 2))
        if num <= 0 then
            return result
        end
        
        for i = 1, num do
            local res, err = parse_request(sock)
            if res == nil then
                return nil, err
            end
            result = result .. res
        end
        
    elseif prefix == 36 then -- char '$'
        local size = tonumber(sub(line, 2))
        if size <= 0 then
            return result
        end
        
        local res, err = sock:receive(size)
        if not res then
            return nil, err
        end
        
        local crlf, err = sock:receive(2)
        if not crlf then
            return nil, err
        end
        
        result = result .. res .. crlf
    end
    
    return result
end

-- 异常退出
local function exit(err)
    ngx.log(ngx.ERR, "Redis ERROR EXIT: [", err, "]")
    return ngx.exit(ngx.ERROR)
end

----------------------------------------
local _M = {}
_M._VERSION = "1.0"

function _M.new(self, config)
    local t = {
        _ip = config.ip or "127.0.0.1",
        _port = config.port or 6379,
        _auth = config.auth,
        _timeout = config.timeout or 1000,  -- default 1 sec
        _pool_size = config.pool_size or 100,
        _max_idle_timeout = config.max_idle_timeout or 10000
    }
    return setmetatable(t, { __index = _M })
end

function _M.run(self)
	local downstream_sock = assert(ngx.req.socket(true))
    
	-- 解析客户端请求
	local res, err = parse_request(downstream_sock)
	if not res then
		return exit("parse_request failed : " .. err)
	end
    
	-- 创建 redis 连接
	local red = redis:new()
	
	red:set_timeout(self._timeout)
	
	local ok, err = red:connect(self._ip, self._port)
	if not ok then
		return exit(err)
	end
        
	-- redis auth 授权
	if self._auth then
		local times = assert(red:get_reused_times())
		if times == 0 then
			local ok, err = red:auth(self._auth)
			if not ok then
				return exit("auth failed : " .. err)
			end
		end
	end
        
	-- 发送请求到 redis 
	local upstream_sock = rawget(red, "_sock")
	upstream_sock:send(res)
        
	-- 接收 redis 应答，并解析
	local res, err = parse_request(upstream_sock)
	if not res then
		return exit("receive from redis server error: " .. err)
	end
	
	-- 发送应答给客户端
	downstream_sock:send(res)
	
	-- 设置 redis 连接池
	local ok, err = red:set_keepalive(self._max_idle_timeout, self._pool_size)
	if not ok then
		ngx.log(ngx.ERR, "redis set_keepalive failed: [", err, "]")
	end
end

return _M
