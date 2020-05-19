#pragma once
#include "../include/ompe/omp_log.hpp"

void pnested2()
{

    omp_set_nested(1);
    omp_set_dynamic(0);


    omp_log_inic(log);
    log << "first0" << omp_log::msg::end;
    #pragma omp parallel num_threads(2)
    {
        omp_log_inic_parented(log);
        log << "first 1" << omp_log::msg::end;
        #pragma omp parallel num_threads(4)
        {
            omp_log_inic_parented(log);
            log << "first 2" << omp_log::msg::end;
            #pragma omp parallel num_threads(4)
            {
                omp_log_inic_parented(log);
                log << "first 3" << omp_log::msg::end;
                for(int i = 0; i < 30; i++)
                    log << "for 3:" << i << omp_log::msg::end;
                log << "last 3" << omp_log::msg::end;
            }
            log << "last 2" << omp_log::msg::end;
        }
        log << "last 1" << omp_log::msg::end;
    }
    log << "last 0" << omp_log::msg::end;

}




