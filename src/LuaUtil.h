/**
 * Utility functions for interacting with Lua through the Lua C API
 */

#ifndef LUA_UTIL_H
#define LUA_UTIL_H

#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <vector>
#include <lua/lua.hpp>


namespace lua {

    /* Run the compiled Lua script in binary buffer */
    void runBinary(lua_State *L, const char* bin, size_t n, const char* script);

    /* Compile and run a Lua script */
    void loadAndRunFile(lua_State *L, const char* script);

    /* Close erroring Lua state and print user-formatted error message */
    void error (lua_State *L, const char *msg, ...);

    /* Lua callback for writing compiled script binary to a buffer */
    int  binaryWriter(lua_State *L, const void* p, size_t n, void* user_data);

    /* Helper function to display the current state of a Lua state's stack */
    void stackDump (lua_State *L);
};

#endif // LUA_UTIL_H
