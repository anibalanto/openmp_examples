#pragma once
#include "register_thread.hpp"
#include "function_loaded.hpp"

void phello_omp()
{

    static int const th_cant = 4;
    
    RegisterThread regist("hello", th_cant);
    
    regist.registrate("Entrando"); 

    omp_set_num_threads(th_cant);
    #pragma omp parallel
    {
        regist.registrate("Hola OpenMP!");
    }

    regist.registrate("Todo hecho"); 
}

FunctionLoaded load_phello_omp()
{
    return {"phello_omp", phello_omp};
}