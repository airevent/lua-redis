--

local trace = require "trace"
local net = require "net"
local redis = require "redis"
local std = require "std"

local t1, t2
local r, es, r2, es2

--[[

local s = assert(net.ip4.tcp.socket())
t1 = std.microtime()
    r, es = s:connect("127.0.0.1", 6379)
t2 = std.microtime()

]]

local s = assert(net.unix.socket())
t1 = std.microtime()
    r, es = s:connect("/var/run/redis/master.sock")
t2 = std.microtime()

print("connect time:", (t2-t1)*1000*1000, "ns")
assert(r, es)

t1 = std.microtime()
r, es = s:send("PING\r\nSET a -20\r\nDECR a\r\nGET a\r\nPING\r\nEXISTS a\r\nEXISTS wtf\r\n")
r2, es2 = s:recv()
t2 = std.microtime()

print("response time:", (t2-t1)*1000*1000, "ns")
assert(not es, es)
assert(r2, es)
trace(r2)
trace(redis.unpack(r2))
