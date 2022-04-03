#include "LuaUtil.h"

using Buffer = std::vector<char>;
using Data   = std::pair<int, Buffer>;

namespace lua {

    void runBinary(lua_State *L, const char* bin, size_t n, const char* script)
    {
        if(luaL_loadbufferx (L, bin, n, script, "b")) {
            error(L, "Could not load lua script \"%s\" binary", script);
        } else {
            if (lua_pcall(L, 0, 0, 0)) {
                error(L, "Could not run lua script \"%s\"", script);
            }
        }
    }

    void loadAndRunFile(lua_State *L, const char* script)
    {
        if(luaL_loadfile(L, script)) {
            error(L, "Could not compile script \"%s\"", script);
        } else {
            if (lua_pcall(L, 0, 0, 0)) {
                error(L, "Could not run lua script \"%s\"", script);
            }
        }
    }

    void error (lua_State *L, const char *msg, ...)
    {
        va_list argp;
        va_start(argp, msg);
        fprintf(stderr, "[LUA ERROR]: ");
        vfprintf(stderr, msg, argp);
        fprintf(stderr, "\n");
        va_end(argp);
        lua_close(L);
        exit(EXIT_FAILURE);
    }

    int binaryWriter(lua_State *L, const void* p, size_t n, void* user_data)
    {
        Buffer* binary = static_cast<Buffer*>(user_data);
        size_t pos = binary->size();
        binary->resize(pos + n);
        char* data = binary->data();
        memcpy(&data[pos], p, n);
        return 0;
    }


    // from: https://www.lua.org/pil/24.2.3.html
    void stackDump (lua_State *L) {
        int i;
        int top = lua_gettop(L);
        for (i = 1; i <= top; i++) {  /* repeat for each level */
            int t = lua_type(L, i);
            switch (t) {
        
            case LUA_TSTRING:  /* strings */
                printf("`%s'", lua_tostring(L, i));
                break;
        
            case LUA_TBOOLEAN:  /* booleans */
                printf(lua_toboolean(L, i) ? "true" : "false");
                break;
        
            case LUA_TNUMBER:  /* numbers */
                printf("%g", lua_tonumber(L, i));
                break;
        
            default:  /* other values */
                printf("%s", lua_typename(L, t));
                break;
        
            }
            printf("  ");  /* put a separator */
        }
        printf("\n");  /* end the listing */
    }

};
