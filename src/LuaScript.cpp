
#include "LuaScript.h"


Script::Script(const std::string filename): 
    scriptPath(filename) 
{
    // Create new Lua state (virtual machine, basically)
    L = luaL_newstate();

    /** Link basic Lua libraries for use in scripts:
    * io
    * string
    * math
    * table
    * coroutine
    * os
    * utf8
    * debug
    * package
    */
    luaL_openlibs(L);

    // Compile the Lua script
    compileScript();
}

void Script::run() {

    // Set captured variables in Lua global environment
    for (auto& var : variables) {
        var->push(L);
        lua_setglobal(L, var->name);
    }

    // Run the compiled script binary (to avoid recompiling)
    lua::runBinary(L, binary.data(), binary.size(), scriptPath.c_str());

    // Get captured variables from Lua global environment
    for (auto& var : variables) {
        lua_getglobal(L, var->name);
        var->get(L);
        lua_pop(L, 1);
    }
}

void Script::reload() {
    compileScript();    // Recompile script to binary
}

void Script::setGlobal(int& i, const char* name) {
    auto v = make_shared<shared_int>(name); // Create shared variable object
    v->value = &i;                          // Point value to argument address
    variables.push_back(v);                 // Add shared object to captures
}

void Script::setGlobal(double& d, const char* name) {
    auto v = make_shared<shared_double>(name);
    v->value = &d;
    variables.push_back(v);
}

void Script::setGlobal(float& f, const char* name) {
    auto v = make_shared<shared_float>(name);
    v->value = &f;
    variables.push_back(v);
}


void Script::setGlobal(const char*& s, const char* name) {
    auto v = make_shared<shared_cstring>(name);
    v->value = &s;
    variables.push_back(v);
}

void Script::setGlobal(std::string& s, const char* name) {
    auto v = make_shared<shared_string>(name);
    v->value = &s;
    variables.push_back(v);
}

void Script::setGlobal(bool& b, const char* name) {
    auto v = make_shared<shared_boolean>(name);
    v->value = &b;
    variables.push_back(v);
}


void Script::compileScript() {
    // Load the script from file
    if(luaL_loadfile(L, scriptPath.c_str())) {
        lua::error(L, "Could not compile script \"%s\"", scriptPath.c_str());
    }

    // Delete previously compiled binary
    binary.clear();

    // Write newly compiled script to binary
    if (lua_dump(L, lua::binaryWriter, (void*)&binary, 0)) {
        lua::error(L, "Could not write script \"%s\" to binary form", scriptPath.c_str());
    }
}




Script::~Script() {
    lua_close(L);
}

