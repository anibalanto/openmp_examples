#pragma once
#include "register_thread.hpp"
#include "function_loaded.hpp"

void pfor1()
{
    static int const cant = 200000000;
    {
        RegisterThread regist("secuential");
        for (int i=1; i<cant; i++)
        {

            if(i % (cant/15) == 0 || i == 1)
                regist.registrate(std::to_string(i));
        }
    }
    
    {
        RegisterThread regist("parallel");
        #pragma omp parallel for
        for (int i=1; i<cant; i++)
        {
            if(i % (cant/15) == 0 || i == 1)
                regist.registrate(std::to_string(i));
        }
    }
}

//
/*;
FunctionLoaded load_pfor1()
{
    return {"pfor1", pfor1};
}*/
