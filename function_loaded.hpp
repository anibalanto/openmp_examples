#pragma once
#include <string>
#include <functional>

struct FunctionLoaded
{
    std::string name;
    std::function<void ()> func;
};