#pragma once
#include "register_thread.hpp"
#include "function_loaded.hpp"

void pfor3_single_master()
{
    static int const rep = 3;
    {
        int num_ths = 5;
        RegisterThread regist("singel_master", num_ths);
        omp_set_nested(1);
        omp_set_dynamic(0);
        #pragma omp parallel num_threads(num_ths)
        {
            for(int i=0; i < rep; i++)
                regist.registrate("parallel " + std::to_string(i));

            //parent thread number
            int p_th_n = omp_get_thread_num();
            
            #pragma omp parallel num_threads(2)
            {
                #pragma omp single
                {
                    for(int i=0; i < rep; i++)
                        regist.registrate("[" 
                                        + std::to_string(p_th_n)
                                        + "] NESTED SINGLE " 
                                        + std::to_string(i));
                }
            }

            #pragma omp master
            {
                for(int i=0; i < rep; i++)
                    regist.registrate("MASTER " + std::to_string(i));
            }

            #pragma omp single
            {
                for(int i=0; i < rep; i++)
                    regist.registrate("SINGLE " + std::to_string(i));
            }

            #pragma omp barrier
            
            #pragma omp single
            {
                regist.registrate("--barrier-----");
            }
            
            for(int i=0; i < rep; i++)
                regist.registrate("parallel 4." + std::to_string(i));

        }
    }
}

FunctionLoaded load_pfor3_single_master()
{
    return {"pfor3_single_master", pfor3_single_master};
}