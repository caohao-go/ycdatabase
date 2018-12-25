local redis = require "resty.redis"

local assert = assert
local print = print
local rawget = rawget
local setmetatable = setmetatable
local tonumber = tonumber
local byte = string.byte
local sub = string.sub

-- 解析请求
local function parse(sock)
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
            local res, err = parse(sock)
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
    ngx.log(ngx.ERR, err)
    return ngx.exit(ngx.ERROR)
end

----------------------------------------
local _M = {}
_M._VERSION = "1.0"

function _M.new(self, config)
    local t = {
        _ip = config.ip or "127.0.0.1",
        _port = config.port or 6379,
        _timeout = config.timeout or 100,
        _size = config.size or 10,
        _auth = config.auth,
    }
    return setmetatable(t, { __index = _M })
end

function _M.run(self)
    local ip = self._ip
    local port = self._port
    local timeout = self._timeout
    local size = self._size
    local auth = self._auth
    local downstream_sock = assert(ngx.req.socket(true))
    
    while true do
    	-- 解析客户端请求
        local res, err = parse(downstream_sock)
        if not res then
            return exit(err)
        end
        
        -- 创建 redis 连接
        local red = redis:new()
        local ok, err = red:connect(ip, port)
        if not ok then
            return exit(err)
        end
        
        -- redis auth 授权
        if auth then
            local times = assert(red:get_reused_times())
            if times == 0 then
                local ok, err = red:auth(auth)
                if not ok then
                    return exit(err)
                end
            end
        end
        
        -- 发送请求到 redis 
        local upstream_sock = rawget(red, "_sock")
        upstream_sock:send(res)
        
        -- 接收 redis 应答，并解析
        local res, err = parse(upstream_sock)
        if not res then
            return exit(err)
        end
        
        -- 设置 redis 连接池
        red:set_keepalive(timeout, size)
        
        -- 发送应答给客户端
        downstream_sock:send(res)
    end
end

return _M
