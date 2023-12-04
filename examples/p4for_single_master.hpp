
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
                log << "parallel " << i << std::endl;

            
            #pragma omp parallel num_threads(2)
            {
                omp_log_inic_parented(log);
                #pragma omp single
                {
                    for(int i=0; i < rep; i++)
                        log << "NESTED SINGLE " << i << std::endl;
                }
            }

            #pragma omp master
            {
                for(int i=0; i < rep; i++)
                    log << "MASTER " << i << std::endl;
            }

            #pragma omp single
            {
                for(int i=0; i < rep; i++)
                    log << "SINGLE " << i << std::endl;
            }

            #pragma omp barrier
            
            #pragma omp single
            {
                log << "--barrier-----" << std::endl;
            }
            
            for(int i=0; i < rep; i++)
                log << "parallel 4." << i << std::endl;

        }
        omp_log_finalize();
    }
}

