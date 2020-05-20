#pragma once
#include "../include/ompe/omp_log.hpp"

long double factorial(unsigned int n)
{
    if( n > 0 )
        return(n * factorial( n-1 ) );
    return( 1.0 );
}

void pnested()
{

    omp_log_inic(log);
    log << "Lvl0: begin" << omp_log::endl();

    unsigned int n = 0;
    log << "Lvl0:(n: " << n << ")" << omp_log::endl();
    #pragma omp parallel num_threads(3)
    {
        omp_log_inic_parented(log);
        log << "Lvl1: begin" << omp_log::endl();

            #pragma omp parallel num_threads(4)
            {
                omp_log_inic_parented(log);

                unsigned int m;
                #pragma omp critical
                {
                    m = n++;
                    log << "Lvl3: (m: " << m << ") begin";
                }
                log << omp_log::endl();

                auto res = factorial(m);

                log << "Lvl3: (m: " << m << ") -> fact(m) = " << res << omp_log::endl();


            }

        log << "Lvl1: end" << omp_log::endl();
    }
    log << "Lvl0:(n: " << n << ")" << omp_log::endl();

    log << "Lvl0: end" << omp_log::endl();

}

void pfactorial_nested()
{
    omp_set_dynamic(0);
    omp_set_nested(1);

    omp_log_inic(log);

    unsigned int n = 0;
    unsigned int i1 = 0;
    unsigned int i2 = 0;

    //log << "t\t: begin" << omp_log::endl();

    std::map<int, long double> r;
    #pragma omp parallel num_threads(5)
    {
        omp_log_inic_parented(log);

        unsigned int ri;
        #pragma omp critical
        {
            ri = i1++;
        }
        //log << "r" << ri <<"\t: begin" << omp_log::endl();

        std::map<int, long double> p;

        #pragma omp parallel num_threads(4)
        {
            omp_log_inic_parented(log);

            unsigned int pi;
            long double m0, m1;
            #pragma omp critical
            {
                pi = i2++;
                m0 = (n == 0) ? 1 : n;
                n++;
                m1 = n++;
            }
            //log << "p" << pi << "\t: begin" << omp_log::endl();

            auto res = m0 * m1;

            log << "p" << pi << "\t: {" << m0 << ", " << m1 << "} -> \tMult(p) = " << res << omp_log::endl();
            #pragma omp critical
            {
                p[pi] = res;
            }
        }
        long double res = 1.0;

        log << "r" << ri << "\t: {";
        for (auto pres : p)
        {
            log << "p" << pres.first << ", ";
            res *= pres.second;
        }
        log << "} -> \tMult(x) = " << res << omp_log::endl();
        p.clear();
        #pragma omp critical
        {
            r[ri] = res;
        }

    }

    long double res = 1.0;
    log << "t\t: {";
    for (auto rres : r)
    {
        log << "r" << rres.first << ", ";
        res *= rres.second;
    }
    log << "} -> \tMult(x) = " << res << omp_log::endl();
    r.clear();
    n--;
    log << "t\t:(n: " << n << ") -> pfactorial(n) = " << res << omp_log::endl();
    log << "t\t:(n: " << n << ") -> factorial(n)  = " << factorial(n) << omp_log::endl();

}

void pnested_compare()
{
    omp_set_dynamic(0);

    std::cout << std::endl
              << "----------" << std::endl;
    std::cout << "nested = 0" << std::endl;

    omp_set_nested(0);
    pnested();

    std::cout << std::endl
              << "----------" << std::endl;
    std::cout << "nested = 1" << std::endl;

    omp_set_nested(1);
    pnested();
}

