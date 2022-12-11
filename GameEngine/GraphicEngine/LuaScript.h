#pragma once

#include <string>

class LuaScript {

private:
	std::string script;

public:
	LuaScript();
	~LuaScript();

	int ExecuteScript();
	int LoadScript();
};