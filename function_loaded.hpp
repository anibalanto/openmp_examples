#pragma once
#include <string>
#include <functional>
#include <vector>

struct FunctionLoaded
{
    std::string name;
    std::function<void ()> func;
};

#define loader(name) {#name, name}
