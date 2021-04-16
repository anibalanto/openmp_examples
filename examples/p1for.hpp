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
    static int const cant = 8000;
    {
        omp_log_inic();
        omp_log << "secuential" << std::endl;
        for (int i=1; i<cant; i++)
        {
            if(i % (cant/30) == 0 || i == 1)
                omp_log << i << std::endl;
        }
    }
    std::cout << "--------------"  << std::endl;
    {
        omp_log_inic();
        omp_log << "parallel" << std::endl;
        //OmpLogInt i2 = OmpLogInt(1, omp_log);
        #pragma omp parallel for
        for (auto i = 1; i < cant; i++)
        {
            omp_log_inic_for(&i);
            if(i % (cant/30) == 0 || i == 1)
                omp_log << "first for " << i << std::endl;
        }
        omp_log_end_for();

        omp_log << "intermedate" << std::endl;

        #pragma omp parallel for
        for (auto i = 1; i < cant; i++)
        {
            omp_log_inic_for(&i);
            if(i % (cant/30) == 0 || i == 1)
                omp_log << "second for " << i << std::endl;
        }
        omp_log_end_for();

        omp_log << "end" << std::endl;
    }
}
