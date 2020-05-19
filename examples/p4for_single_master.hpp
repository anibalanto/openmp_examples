/*
Saluda por pantalla del ejemplo:

[0:] > @begin
0                       [0:] > first0
*───────────┐           [0:] > @fork
0           │           [0:0:] > first 1
│           1           [0:1:] > first 1
*───┬───┐   │           [0:0:] > @fork
0   │   │   │           [0:0:0:] > first 2
│   1   │   │           [0:0:1:] > first 2
│   │   │   *───┬───┐   [0:1:] > @fork
│   *─┐ │   │   │   │   [0:0:1:] > @fork
│   │ │ │   │   │   2   [0:1:2:] > first 2
*─┐ │ │ │   │   │   │   [0:0:0:] > @fork
0 │ │ │ │   │   │   │   [0:0:0:0:] > inic   factorial(15)
0 │ │ │ │   │   │   │   [0:0:0:0:] > result factorial(15) = 1307674368000
│ │ │ │ │   0   │   │   [0:1:0:] > first 2
│ │ │ │ 2   │   │   │   [0:0:2:] > first 2
│ │ │ │ │   │   │   *─┐ [0:1:2:] > @fork
│ │ │ │ │   *─┐ │   │ │ [0:1:0:] > @fork
│ 1 │ │ │   │ │ │   │ │ [0:0:0:1:] > inic   factorial(16)
│ 1 │ │ │   │ │ │   │ │ [0:0:0:1:] > result factorial(16) = 20922789888000
°─┘ │ │ │   │ │ │   │ │ [0:0:0:] > @join
│   │ │ │   │ │ │   0 │ [0:1:2:0:] > inic   factorial(15)
│   │ │ │   │ │ │   0 │ [0:1:2:0:] > result factorial(15) = 1307674368000
│   │ │ *─┐ │ │ │   │ │ [0:0:2:] > @fork
│   │ │ 0 │ │ │ │   │ │ [0:0:2:0:] > inic   factorial(15)
│   │ │ │ │ │ │ 1   │ │ [0:1:1:] > first 2
│   │ │ │ │ 0 │ │   │ │ [0:1:0:0:] > inic   factorial(15)
│   │ │ │ │ 0 │ │   │ │ [0:1:0:0:] > result factorial(15) = 1307674368000
│   │ 1 │ │ │ │ │   │ │ [0:0:1:1:] > inic   factorial(15)
│   │ 1 │ │ │ │ │   │ │ [0:0:1:1:] > result factorial(15) = 1307674368000
│   │ │ │ 1 │ │ │   │ │ [0:0:2:1:] > inic   factorial(16)
│   │ │ │ 1 │ │ │   │ │ [0:0:2:1:] > result factorial(16) = 20922789888000
│   │ │ │ │ │ │ *─┐ │ │ [0:1:1:] > @fork
│   │ │ 0 │ │ │ │ │ │ │ [0:0:2:0:] > result factorial(15) = 1307674368000
│   │ │ °─┘ │ │ │ │ │ │ [0:0:2:] > @join
│   │ │ │   │ 1 │ │ │ │ [0:1:0:1:] > inic   factorial(16)
│   │ │ 2   │ │ │ │ │ │ [0:0:2:] > last 2
│   │ │ │   │ │ 0 │ │ │ [0:1:1:0:] > inic   factorial(16)
│   │ │ │   │ │ 0 │ │ │ [0:1:1:0:] > result factorial(16) = 20922789888000
│   0 │ │   │ │ │ │ │ │ [0:0:1:0:] > inic   factorial(16)
│   0 │ │   │ │ │ │ │ │ [0:0:1:0:] > result factorial(16) = 20922789888000
│   │ │ │   │ │ │ 1 │ │ [0:1:1:1:] > inic   factorial(15)
│   │ │ │   │ │ │ 1 │ │ [0:1:1:1:] > result factorial(15) = 1307674368000
0   │ │ │   │ │ │ │ │ │ [0:0:0:] > last 2
│   °─┘ │   │ │ │ │ │ │ [0:0:1:] > @join
│   │   │   │ │ °─┘ │ │ [0:1:1:] > @join
│   │   │   │ 1 │   │ │ [0:1:0:1:] > result factorial(16) = 20922789888000
│   │   │   °─┘ │   │ │ [0:1:0:] > @join
│   1   │   │   │   │ │ [0:0:1:] > last 2
°───┴───┘   │   │   │ │ [0:0:] > @join
│           0   │   │ │ [0:1:0:] > last 2
0           │   │   │ │ [0:0:] > last 1
│           │   │   │ 1 [0:1:2:1:] > inic   factorial(16)
│           │   1   │ │ [0:1:1:] > last 2
│           │   │   │ 1 [0:1:2:1:] > result factorial(16) = 20922789888000
│           │   │   °─┘ [0:1:2:] > @join
│           │   │   2   [0:1:2:] > last 2
│           °───┴───┘   [0:1:] > @join
│           1           [0:1:] > last 1
°───────────┘           [0:] > @join
0                       [0:] > last 0
[0:] > @end


*/


#pragma once
#include "../include/ompe/omp_log.hpp"

void p4for_single_master()
{
    static int const rep = 3;
    {
        int num_ths = 5;

        omp_set_nested(1);
        omp_set_dynamic(0);

        omp_log_inic(log);
        #pragma omp parallel num_threads(num_ths)
        {
            omp_log_inic_parented(log);

            for(int i=0; i < rep; i++)
                log << "parallel " << i << omp_log::endl();

            
            #pragma omp parallel num_threads(2)
            {
                omp_log_inic_parented(log);
                #pragma omp single
                {
                    for(int i=0; i < rep; i++)
                        log << "NESTED SINGLE " << i << omp_log::endl();
                }
            }

            #pragma omp master
            {
                for(int i=0; i < rep; i++)
                    log << "MASTER " << i << omp_log::endl();
            }

            #pragma omp single
            {
                for(int i=0; i < rep; i++)
                    log << "SINGLE " << i << omp_log::endl();
            }

            #pragma omp barrier
            
            #pragma omp single
            {
                log << "--barrier-----" << omp_log::endl();
            }
            
            for(int i=0; i < rep; i++)
                log << "parallel 4." << i << omp_log::endl();

        }
    }
}

