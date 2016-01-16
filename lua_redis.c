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

static int lua_redis_ping( lua_State *L ) {
    lua_pushstring(L, "PING" EOL);
    return 1;
}

// arg#1 - str
// will parse str and divide records
// will return records and the remaining unused bytes count
static int lua_redis_unpack( lua_State *L ) {
    size_t len;
    const char *str = lua_tolstring(L, 1, &len);

    size_t pos = 0;
    size_t records_found = 0;

    lua_newtable(L);

    while ( pos + 2 < len ) {
        switch ( (char)str[pos] ) {
            case RESP_REPLY_MARK_STRING:
                lua_redis_unpack_string(L, &pos, len, str, &records_found);
                break;
            case RESP_REPLY_MARK_ERROR:
                lua_redis_unpack_error(L, &pos, len, str, &records_found);
                break;
            case RESP_REPLY_MARK_INTEGER:
                lua_redis_unpack_integer(L, &pos, len, str, &records_found);
                break;
            case RESP_REPLY_MARK_BULK:
                lua_redis_unpack_bulk(L, &pos, len, str, &records_found);
                break;
            case RESP_REPLY_MARK_ARRAY:
                lua_redis_unpack_array(L, &pos, len, str, &records_found);
                break;
        }
    }

    if ( !records_found ) {
        lua_pushnil(L);
    }

    lua_pushinteger(L, len-pos);

    return 2;
}

static void lua_redis_unpack_string( lua_State *L, size_t *pos, size_t len, const char *str, size_t *records_found ) {
    size_t eol_pos = *pos + 1;

    while ( eol_pos + 1 < len && str[eol_pos] != EOL_CHAR1 ) {
        eol_pos++;
    }

    if ( eol_pos + 1 < len && str[eol_pos] == EOL_CHAR1 && str[eol_pos+1] == EOL_CHAR2 ) {
        (*records_found)++;
        lua_pushinteger(L, *records_found);
        lua_pushlstring(L, &str[*pos+1], eol_pos-*pos-1);
        lua_rawset(L, -3);
        *pos = eol_pos + 2;
    } else {
        *pos = len;
    }
}

static void lua_redis_unpack_error( lua_State *L, size_t *pos, size_t len, const char *str, size_t *records_found ) {
    size_t eol_pos = *pos + 1;

    while ( eol_pos + 1 < len && str[eol_pos] != EOL_CHAR1 ) {
        eol_pos++;
    }

    if ( eol_pos + 1 < len && str[eol_pos] == EOL_CHAR1 && str[eol_pos+1] == EOL_CHAR2 ) {
        (*records_found)++;
        lua_pushinteger(L, *records_found);
        lua_newtable(L);
            lua_pushstring(L, "error");
            lua_setfield(L, -2, "type");
            lua_pushlstring(L, &str[*pos+1], eol_pos-*pos-1);
            lua_setfield(L, -2, "data");
        lua_rawset(L, -3);
        *pos = eol_pos + 2;
    } else {
        *pos = len;
    }
}

static void lua_redis_unpack_integer( lua_State *L, size_t *pos, size_t len, const char *str, size_t *records_found ) {
    short is_negative = 0;
    int chunk_len = 0;
    size_t eol_pos = *pos + 1;

    if ( eol_pos < len && str[eol_pos] == '-' ) {
        eol_pos++;
        is_negative = 1;
    }

    while ( eol_pos + 1 < len && str[eol_pos] != EOL_CHAR1 ) {
        chunk_len = (chunk_len*10)+(str[eol_pos] - '0');
        eol_pos++;
    }

    if ( eol_pos + 1 < len && str[eol_pos] == EOL_CHAR1 && str[eol_pos+1] == EOL_CHAR2 ) {
        (*records_found)++;
        lua_pushinteger(L, *records_found);
        if ( is_negative ) {
            lua_pushinteger(L, -chunk_len);
        } else {
            lua_pushinteger(L, chunk_len);
        }
        lua_rawset(L, -3);
        *pos = eol_pos + 2;
    } else {
        *pos = len;
    }
}

static void lua_redis_unpack_bulk( lua_State *L, size_t *pos, size_t len, const char *str, size_t *records_found ) {
    int chunk_len = 0;
    size_t eol_pos = *pos + 1;

    if ( eol_pos < len && str[eol_pos] == '-' ) {
        if ( str[eol_pos+2] == EOL_CHAR1 && str[eol_pos+3] == EOL_CHAR2 ) {
            (*records_found)++;
            lua_pushinteger(L, *records_found);
            lua_pushboolean(L, 0);
            lua_rawset(L, -3);
            *pos = eol_pos + 4;
        } else {
            *pos = len;
        }
    } else {
        while ( eol_pos + 1 < len && str[eol_pos] != EOL_CHAR1 ) {
            chunk_len = (chunk_len*10)+(str[eol_pos] - '0');
            eol_pos++;
        }

        if ( eol_pos + 1 + chunk_len + 2 < len && str[eol_pos] == EOL_CHAR1 && str[eol_pos+1] == EOL_CHAR2 ) {
            (*records_found)++;
            lua_pushinteger(L, *records_found);
            lua_pushlstring(L, &str[eol_pos+2], chunk_len);
            lua_rawset(L, -3);
            *pos = eol_pos + chunk_len + 4;
        } else {
            *pos = len;
        }
    }
}

static void lua_redis_unpack_array( lua_State *L, size_t *pos, size_t len, const char *str, size_t *records_found ) {
    int chunk_len = 0;
    size_t eol_pos = *pos + 1;

    if ( eol_pos < len && str[eol_pos] == '-' ) {
        if ( str[eol_pos+2] == EOL_CHAR1 && str[eol_pos+3] == EOL_CHAR2 ) {
            (*records_found)++;
            lua_pushinteger(L, *records_found);
            lua_pushboolean(L, 0);
            lua_rawset(L, -3);
            *pos = eol_pos + 4;
        } else {
            *pos = len;
        }
    } else {
        while ( eol_pos + 1 < len && str[eol_pos] != EOL_CHAR1 ) {
            chunk_len = (chunk_len*10)+(str[eol_pos] - '0');
            eol_pos++;
        }

        printf("array len: %d\n", chunk_len);
        *pos = len;
    }
}
