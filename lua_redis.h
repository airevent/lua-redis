// redis

#include "lua_pd.h"

//

#define RESP_REPLY_MARK_STRING '+'
#define RESP_REPLY_MARK_ERROR '-'
#define RESP_REPLY_MARK_INTEGER ':'
#define RESP_REPLY_MARK_BULK '$'
#define RESP_REPLY_MARK_ARRAY '*'

//

LUAMOD_API int luaopen_redis( lua_State *L );

static int lua_redis_pack( lua_State *L );
static int lua_redis_unpack( lua_State *L );

//

static const luaL_Reg __index[] = {
    {"pack", lua_redis_pack},
    {"unpack", lua_redis_unpack},
    {NULL, NULL}
};
