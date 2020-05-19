#pragma once
#include "register_thread.hpp"

#define omp_log_block(name, log) \
auto name = name->inic_block(log)

void pnested2()
{

    omp_set_nested(1);
    omp_set_dynamic(0);

    OmpLog reg("nested2");
    #pragma omp parallel num_threads(2)
    {
        omp_log_block(reg, "1st LEVEL");
        reg << "begin";

        #pragma omp parallel num_threads(2)
        {
            omp_log_block(reg, "2md LEVEL");
            reg << "begin";

            #pragma omp parallel num_threads(2)
            {
                omp_log_block(reg, "3th LEVEL");
                reg << "begin";
                reg << "end";
            }
            reg << "end";
        }
        reg << "end";
    }
}




