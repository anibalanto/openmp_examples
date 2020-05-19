#pragma once
#include "../include/ompe/omp_log.hpp"

long long int factorial(long long int n)
{
    if(n<0)
        return(-1); /*Wrong value*/
    if(n==0)
        return(1);  /*Terminating condition*/
    else
        return(n*factorial(n-1));
}

void pnested()
{

    omp_log_inic(log);
    log << "Level 0: begin" << omp_log::endl();

    #pragma omp parallel num_threads(2)
    {
        omp_log_inic_parented(log);
        log << "Level 1: begin" << omp_log::endl();

        #pragma omp parallel num_threads(2)
        {
            omp_log_inic_parented(log);
            log << "Level 2: begin" << omp_log::endl();

            unsigned long long n = 15;
            #pragma omp parallel num_threads(3)
            {
                omp_log_inic_parented(log);

                unsigned long long v;
                log << "Level 3: calc  factorial(";

                #pragma omp critical
                    v = n++;

                log << v << ")" << omp_log::endl();
                auto f = factorial(v);

                log << "Level 3: result = " << f << omp_log::endl();

            }

            log << "Level 2: end" << omp_log::endl();
        }

        log << "Level 1: end" << omp_log::endl();
    }

    log << "Level 0: end" << omp_log::endl();

}

void pnested_compare()
{
    omp_set_dynamic(0);

    std::cout << std::endl
              << "----------" << std::endl;
    std::cout << "nested = 1" << std::endl;

    omp_set_nested(1);
    pnested();

    std::cout << std::endl
              << "----------" << std::endl;
    std::cout << "nested = 0" << std::endl;

    omp_set_nested(0);
    pnested();
}

