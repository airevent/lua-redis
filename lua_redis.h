// redis

#include "lua_pd.h"

//

#define RESP_REPLY_MARK_STRING '+'
#define RESP_REPLY_MARK_ERROR '-'
#define RESP_REPLY_MARK_INTEGER ':'
#define RESP_REPLY_MARK_BULK '$'
#define RESP_REPLY_MARK_ARRAY '*'
#define EOL "\r\n"
#define EOL_CHAR1 '\r'
#define EOL_CHAR2 '\n'

//

LUAMOD_API int luaopen_redis( lua_State *L );

static int lua_redis_ping( lua_State *L );
static int lua_redis_unpack( lua_State *L );

static void lua_redis_unpack_string( lua_State *L, size_t *pos, size_t len, const char *str, size_t *records_found );
static void lua_redis_unpack_error( lua_State *L, size_t *pos, size_t len, const char *str, size_t *records_found );
static void lua_redis_unpack_integer( lua_State *L, size_t *pos, size_t len, const char *str, size_t *records_found );
static void lua_redis_unpack_bulk( lua_State *L, size_t *pos, size_t len, const char *str, size_t *records_found );
static void lua_redis_unpack_array( lua_State *L, size_t *pos, size_t len, const char *str, size_t *records_found );

//

static const luaL_Reg __index[] = {
    {"ping", lua_redis_ping},
    {"unpack", lua_redis_unpack},
    {NULL, NULL}
};
