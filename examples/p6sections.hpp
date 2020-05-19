#pragma once
#include "register_thread.hpp"
#include "function_loaded.hpp"

void psections()
{
    static int const cant = 10;
    
    RegisterThread regist("sections");

    // creo una variable instanciandola con una
    // función lambda que captura por referencia
    // la variable regist
    auto operate = [&regist](const std::string name, int cant)
    {
        regist.registrate("inic " + name);
        for(int i = 0; i < cant; i++)
        {
            regist.registrate(name + std::to_string(i));
        }
        regist.registrate("end " + name);
    };

    #pragma omp parallel sections
    {

        #pragma omp section
        operate("x", cant);

        #pragma omp section
        operate("y", cant);

        #pragma omp section
        operate("z", cant);
    }
}


FunctionLoaded load_psections()
{
    return {"psections", psections};
}