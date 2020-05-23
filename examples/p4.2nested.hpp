#pragma once

#include <math.h>
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
    log << "Lvl0: begin" << std::endl;

    unsigned int n = 0;
    log << "Lvl0:(n: " << n << ")" << std::endl;
    #pragma omp parallel num_threads(3)
    {
        omp_log_inic_parented(log);
        log << "Lvl1: begin" << std::endl;

            #pragma omp parallel num_threads(4)
            {
                omp_log_inic_parented(log);

                unsigned int m;
                #pragma omp critical
                {
                    m = n++;
                    log << "Lvl3: (m: " << m << ") begin";
                }
                log << std::endl;

                auto res = factorial(m);

                log << "Lvl3: (m: " << m << ") -> fact(m) = " << res << std::endl;


            }

        log << "Lvl1: end" << std::endl;
    }
    log << "Lvl0:(n: " << n << ")" << std::endl;

    log << "Lvl0: end" << std::endl;

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


void pbig_factorial_nested()
{
    omp_set_dynamic(0);
    omp_set_nested(1);

    omp_log_inic(log);

    unsigned int n = 0;
    unsigned int i1 = 0;
    unsigned int i2 = 0;

    log.begin_chrono('#');

    std::map<int, long double> r;
    #pragma omp parallel num_threads(5)
    {
        omp_log_inic_parented(log);

        unsigned int ri;
        #pragma omp critical
        {
            ri = i1++;
        }
        //log << "r" << ri <<"\t: begin" << std::endl;

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
            //log << "p" << pi << "\t: begin" << std::endl;

            auto res = m0 * m1;

            log << "p" << pi << "\t: {" << m0 << ", " << m1 << "} -> \tMult(p) = " << res << std::endl;
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
        log << "} -> \tMult(r) = " << res << std::endl;
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
    log << "} -> \tMult(t) = " << res << std::endl;
    r.clear();
    
    log.end_chrono('#');
    
    n--;
    log << "t\t:(n: " << n << ") -> pfactorial(n) = " << res << std::endl;
    
    log.begin_chrono('%');
    res = factorial(n);
    log.end_chrono('%');
    
    log << "t\t:(n: " << n << ") -> factorial(n)  = " << res << std::endl;
    
    
}

long double mulsin(unsigned int n)
{
    if( n > 0 )
        return(sin(1/n) * mulsin( n-1 ) );
    return( sin(1.0) );
}

#define N_THS_1 4
#define N_THS_2 4

void pfactorial_nested()
{
    omp_set_dynamic(0);
    omp_set_nested(1);

    omp_log_inic(log);

    unsigned int n = 0;
    unsigned int i1 = 0;

    log.begin_chrono('#');

    long double r[N_THS_1];
    #pragma omp parallel num_threads(N_THS_1)
    {
        omp_log_inic_parented(log);

        unsigned int ri;
        #pragma omp critical
        {
            ri = i1++;
        }

        unsigned int i2 = 0;
        
        long double p[N_THS_2];

        #pragma omp parallel num_threads(N_THS_2)
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

            auto res = sin(1/m0) * sin(1/m1);

            log << "p" << pi << "\t: {" << m0 << ", " << m1 << "} -> \tMult(p) = " << res << std::endl;
            #pragma omp critical
            {
                p[pi] = res;
            }
        }
        long double res = 1.0;

        log << "r" << ri << "\t: {";
        for (int i = 0; i < N_THS_2; i++)
        {
            log << "p" << i << ", ";
            res *= p[i];
        }
        log << "} -> \tMult(r) = " << res << std::endl;

        #pragma omp critical
        {
            r[ri] = res;
        }

    }

    long double res = 1.0;
    log << "t\t: {";
    for (int i = 0; i < N_THS_1; i++)
    {
        log << "r" << i << ", ";
        res *= r[i];
    }
    log << "} -> \tMult(t) = " << res << std::endl;
    
    n--;
    log << "t\t:(n: " << n << ") -> pmulsin(n) = " << res << std::endl;
    log.end_chrono('#');

    
    log.begin_chrono('%');
    res = mulsin(n);
    log << "t\t:(n: " << n << ") -> mulsin(n)  = " << res << std::endl;
    log.end_chrono('%');
    
    
    
}

