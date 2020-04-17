#include "register_thread.hpp"

#define CANT 30

void pfor4_nowait()
{
    {
        RegisterThread regist("parallel");
        omp_set_nested(1);
        omp_set_dynamic(0);
        #pragma omp parallel
        {
            #pragma omp for
            for(int i=0; i < CANT; i++)
            {
                regist.registrate("for 1." + std::to_string(i));
            }
            #pragma omp for
            for(int i=0; i < CANT; i++)
            {
                regist.registrate("for 2." + std::to_string(i));
            }
            #pragma omp for
            for(int i=0; i < CANT; i++)
            {
                regist.registrate("for 3." + std::to_string(i));
            }
        }
    }
    {
        RegisterThread regist("parallel-nowait");
        omp_set_nested(1);
        omp_set_dynamic(0);
        #pragma omp parallel
        {
            #pragma omp for nowait
            for(int i=0; i < CANT; i++)
            {
                regist.registrate("nowait 1." + std::to_string(i));
            }
            #pragma omp for nowait
            for(int i=0; i < CANT; i++)
            {
                regist.registrate("nowait 2." + std::to_string(i));
            }
            #pragma omp for nowait
            for(int i=0; i < CANT; i++)
            {
                regist.registrate("nowait 3." + std::to_string(i));
            }
        }
    }
}
