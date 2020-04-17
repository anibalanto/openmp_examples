#include "register_thread.hpp"

#define CANT 200000000

void pfor1()
{
    {
        RegisterThread regist("secuential");
        for (int i=1; i<CANT; i++)
        {

            if(i % (CANT/15) == 0 || i == 1)
                regist.registrate(std::to_string(i));
        }
    }
    
    {
        RegisterThread regist("parallel");
        #pragma omp parallel for
        for (int i=1; i<CANT; i++)
        {
            if(i % (CANT/15) == 0 || i == 1)
                regist.registrate(std::to_string(i));
        }
    }
}

