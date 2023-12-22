#pragma once
#include "../include/ompe/register_thread.hpp"
#include "../include/ompe/omp_log.hpp"

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


void pfor1bis()
{
    static long const cant = 30000;
    static long const show = 500;
    static long const cmod = cant/show;
    omp_log_inic();

    omp_logger << "serial" << std::endl;

    omp_logger << omp_log::begin_chrono('%');
    for (long i = 1; i < cant; i++)
    {
        if(i % (cmod) == 0 || i == 1)
            std::cout << "second for " << i << std::endl;
    }
    omp_logger << omp_log::end_chrono('%');


    omp_logger << "parallel" << std::endl;
    omp_logger << omp_log::begin_chrono('#');
    #pragma omp parallel for
    for (long i = 1; i < cant; i++ )
    {   
        omp_log_inic_for(&i);
        if(i % (cmod) == 0 || i == 1) {
            omp_logger << "first for " << i << std::endl;
        }
    }
    omp_log_end_for();
    omp_logger << omp_log::end_chrono('#');

    omp_logger << "end" << std::endl;
    omp_log_finalize();
}
