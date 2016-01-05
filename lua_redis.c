// redis

#include "lua_redis.h"

LUAMOD_API int luaopen_redis( lua_State *L ) {
    luaL_newlib(L, __index);

    lua_newtable(L);
        #include "lua_redis_flags.c"
    lua_setfield(L, -2, "f");

    return 1;
}

//

static int lua_redis_pack( lua_State *L ) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_settop(L, 1);

    lua_pushstring(L, "TODO\r\n");
    return 1;
}

static int lua_redis_unpack( lua_State *L ) {
    size_t len;
    size_t pos = 0;
    const char *str = lua_tolstring(L, 1, &len);

    (void)len;
    (void)pos;
    (void)str;
    lua_fail(L, "TODO", 0);
}

