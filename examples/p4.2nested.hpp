#pragma once

#include <math.h>
#include "../include/ompe/omp_log.hpp"

long double factorial(unsigned int n)
{
    if( n > 0 )
        return(n * factorial( n-1 ) );
    return( 1.0 );
}


void pnested_compare()
{
    omp_set_dynamic(0);

    std::cout << std::endl
              << "----------" << std::endl;
    std::cout << "nested = 1" << std::endl;

    omp_set_nested(1);
    {

        omp_log_inic();
        omp_logger << "Lvl0: begin" << std::endl;

        unsigned int n = 0;
        omp_logger << "Lvl0:(n: " << n << ")" << std::endl;
        #pragma omp parallel num_threads(3)
        {
            omp_log_inic_parented();
            omp_logger << "Lvl1: begin" << std::endl;

                #pragma omp parallel num_threads(4)
                {
                    omp_log_inic_parented();

                    unsigned int m;
                    #pragma omp critical
                    {
                        m = n++;
                    }
                    omp_logger << "Lvl3: (m: " << m << ") begin" << std::endl;

                    auto res = factorial(m);

                    omp_logger << "Lvl3: (m: " << m << ") -> fact(m) = " << res << std::endl;

                }

            omp_logger << "Lvl1: end" << std::endl;
        }
        omp_logger << "Lvl0:(n: " << n << ")" << std::endl;

        omp_logger << "Lvl0: end" << std::endl;

        omp_log_finalize();

    }

}


void pbig_factorial_nested()
{
    omp_set_dynamic(0);
    omp_set_nested(1);

    omp_log_inic();

    omp_logger << omp_log::begin_chrono('#');

    long double r[5];
    #pragma omp parallel num_threads(5)
    {
        int tn1 = omp_get_thread_num();
        int nt1 = omp_get_num_threads();
        omp_log_inic_parented();

        long double p[4];

        #pragma omp parallel num_threads(4)
        {
            omp_log_inic_parented();

            int tn2 = omp_get_thread_num();
            int nt2 = omp_get_num_threads();

            long double m0 = 2 * (tn1 * nt2 + tn2) + 1;

            long double m1 = m0 + 1;

            auto res = m0 * m1;

            omp_logger << "\t: {" << m0 << ", " << m1 << "} -> \tMult(p) = " << res << std::endl;
            p[ tn2 ] = res;
        }
        long double res = 1.0;

        for (int i = 0; i < 4; i++)
        {
            res *= p[i];
        }

        r[ tn1 ] = res;

    }

    long double res = 1.0;
    for (int i = 0; i < 5; i++)
    {
        omp_logger << "p" << i << ", ";
        res *= r[i];
    }
    
    omp_logger << omp_log::end_chrono('#');
    
    omp_logger << "t\t:(n: " << 40 << ") -> pfactorial(n) = " << res << std::endl;
    
    omp_logger << omp_log::begin_chrono('%');

    res = factorial(40);
    omp_logger << "t\t:(n: " << 40 << ") -> factorial(n)  = " << res << std::endl;

    omp_logger << omp_log::end_chrono('%');
    
    omp_log_finalize();
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

    omp_log_inic();

    unsigned int n = 0;
    unsigned int i1 = 0;

    //omp_log.begin_chrono('#');

    long double r[N_THS_1];
    #pragma omp parallel num_threads(N_THS_1)
    {
        omp_log_inic_parented();

        unsigned int ri;
        #pragma omp critical
        {
            ri = i1++;
        }

        unsigned int i2 = 0;
        
        long double p[N_THS_2];

        #pragma omp parallel num_threads(N_THS_2)
        {
            omp_log_inic_parented();

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

            omp_logger << "p" << pi << "\t: {" << m0 << ", " << m1 << "} -> \tMult(p) = " << res << std::endl;
            #pragma omp critical
            {
                p[pi] = res;
            }
        }
        long double res = 1.0;

        omp_logger << "r" << ri << "\t: {";
        for (int i = 0; i < N_THS_2; i++)
        {
            omp_logger << "p" << i << ", ";
            res *= p[i];
        }
        omp_logger << "} -> \tMult(r) = " << res << std::endl;

        #pragma omp critical
        {
            r[ri] = res;
        }

    }

    long double res = 1.0;
    omp_logger << "t\t: {";
    for (int i = 0; i < N_THS_1; i++)
    {
        omp_logger << "r" << i << ", ";
        res *= r[i];
    }
    omp_logger << "} -> \tMult(t) = " << res << std::endl;
    
    n--;
    omp_logger << "t\t:(n: " << n << ") -> pmulsin(n) = " << res << std::endl;
    //omp_log.end_chrono('#');

    
    //omp_log.begin_chrono('%');
    res = mulsin(n);
    omp_logger << "t\t:(n: " << n << ") -> mulsin(n)  = " << res << std::endl;
    //omp_log.end_chrono('%');

}

