#pragma once

#include <iostream>
#include <omp.h>
#include "../include/ompe/omp_log.hpp"

using namespace std;

void phola_openmp()
{
    cout << "Permiso" << "\n";

    #pragma omp parallel num_threads(4)
    {
        int nts = omp_get_num_threads();
        int nt  = omp_get_thread_num();
        cout << "Hola OpenMP! "
             << "Yo soy "    << nt
             << ". Vinimos " << nts
             << "\n";
    }

    cout << "Nos vamos!" << "\n";
}

void pcritical_hola_openmp()
{
    cout << "Permiso" << "\n";

    #pragma omp parallel num_threads(4)
    {
        int nts = omp_get_num_threads();
        int nt  = omp_get_thread_num();
        #pragma omp critical
        {
            cout << "Hola OpenMP! "
                 << "Yo soy "    << nt
                 << ". Vinimos " << nts
                 << "\n";
        }
    }

    cout << "Nos vamos!" << "\n";
}

void phello_omp()
{

    omp_log_inic();

    omp_log << "Permiso" << std::endl;

    #pragma omp parallel num_threads(4)
    {
        omp_log_inic_parented();

        int nts = omp_get_num_threads();
        int nt  = omp_get_thread_num();
        omp_log << "Hola OpenMP! "
                << "Yo soy "    << nt
                << ". Vinimos " << nts
                << std::endl;
    }

    cout << "Nos vamos!" << "\n";
}


