/** 
 * C++ driver for running embedded Lua scripts
 * 
 * Note:
 *  To maintain good performance, try not to capture more than 500 external
 *  variables per script.
 */

#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H

#include <string>
#include <vector>
#include <memory>

// Lua C API utility functions
#include "LuaUtil.h"


// Preprocessor macro to retrieve the name of a variable
#define NAME_OF( v ) #v


// Macros do simplify variable capturing
/* Receive any numeric type and invoke the appropriate capture method */
#define capture( var ) setGlobal( var, NAME_OF( var ))

/* Receive a c-string or an STL string and invoke approriate capture method */
#define captureString( var ) setGlobalString( var, NAME_OF( var ))

/* Invoke capture method for boolean values */
#define captureBool( var )   setGlobalBoolean( var, NAME_OF( var ))


/** 
 * Each instance of shared_var stores the user-end name of the variable and
 * a pointer to the value. Subclasses implement push and get methods to push
 * and get their value to and from the Lua stack, maintaining type information.
 * Current types supported:
 *  - Integers
 *  - Floats
 *  - Doubles
 *  - C style strings
 *  - STL strings
 *  - Booleans
 */
class shared_var {
public:
    shared_var(const char* n): name(n) {}
    virtual void push(lua_State* L) = 0;
    virtual void get(lua_State* L) = 0;
    const char* name;
};

/* Handle captured integer variables */
class shared_int : public shared_var {
public:
    shared_int(const char* n): shared_var(n) {}
    void push(lua_State* L) { lua_pushnumber(L, *value); }
    void get (lua_State* L) { *value = (int)lua_tonumber(L, -1); }
    int* value;
};

/* Handle captured float variables */
class shared_float : public shared_var {
public:
    shared_float(const char* n): shared_var(n) {}
    void push(lua_State* L) { lua_pushnumber(L, *value); }
    void get (lua_State* L) { *value = (float)lua_tonumber(L, -1); }
    float* value;
};

/* Handle captured double variables */
class shared_double : public shared_var {
public:
    shared_double(const char* n): shared_var(n) {}
    void push(lua_State* L) { lua_pushnumber(L, *value); }
    void get (lua_State* L) { *value = lua_tonumber(L, -1); }
    double* value;
};

/* Handle captured c-style string variables */
class shared_cstring : public shared_var {
public:
    shared_cstring(const char* n): shared_var(n) {}
    void push(lua_State* L) { lua_pushstring(L, *value); }
    void get (lua_State* L) { *value = lua_tostring(L, -1); }
    const char** value;
};

/* Handle captured STL string variables */
class shared_string : public shared_var {
public:
    shared_string(const char* n): shared_var(n) {}
    void push(lua_State* L) { lua_pushstring(L, value->c_str()); }
    void get (lua_State* L) { *value = std::string(lua_tostring(L, -1)); }
    std::string* value;
};

/* Handle captured boolean variables */
class shared_boolean : public shared_var {
public:
    shared_boolean(const char* n): shared_var(n) {}
    void push(lua_State* L) { lua_pushboolean(L, *value); }
    void get (lua_State* L) { *value = (bool)lua_toboolean(L, -1); }
    bool* value;
};

// For internal use
using std::make_shared, std::shared_ptr;

/**
 * Encapsulates Lua scripts and provides an interface to capture values from
 * user code.
 */
class Script {
public:

    /* Compiles and saves the Lua script to the object */
    Script(const std::string filename);
    
    /* Free Lua state */
    ~Script();

    /** 
     * Pass captured values to the associated Lua state, run the compiled Lua 
     * script, and return the captured values to client code.
     */
    void run();

    /* Recompile the script located at the given filename */
    void reload();

    /* Capture functions to pass values to Lua state */
    void setGlobal(int& i, const char* name);
    void setGlobal(float& f, const char* name);
    void setGlobal(double& d, const char* name);
    void setGlobal(const char*& s, const char* name);
    void setGlobal(std::string& s, const char* name);
    void setGlobal(bool& b, const char* name);

    /* Pointer to the underlying lua_State */
    lua_State* state() { return L; };

    /* The size of the scripts compiled binary (in bytes) */
    int binarySize() { return binary.size(); }

private:

    /* The Lua state */
    lua_State *L;

    /* Path to the Lua script associated with this object */
    std::string scriptPath;

    /* Variable size buffer containing compiled Lua bytecode */
    std::vector<char> binary;

    /* List of client variables to be captured by Lua script */
    std::vector<shared_ptr<shared_var>> variables;

    /* Compiles the Lua script from the file associated with this object */
    void compileScript();
};

#endif // LUA_SCRIPT_H
