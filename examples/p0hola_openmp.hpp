#pragma once

#include <iostream>
#include <omp.h>
#include "../include/ompe/omp_log.hpp"

using namespace std;

void phola_openmp()
{
    cout << "Using incorrectly cout" << "\n";
    cout << "Beginning" << "\n";

    #pragma omp parallel num_threads(4)
    {
        int nts = omp_get_num_threads();
        int nt  = omp_get_thread_num();
        cout << "Hi OpenMP! "
                << "I'm "    << nt
                << ". Come " << nts
                << "\n";
    }

    cout << "When appear?" << "\n";
}

void pcritical_hola_openmp()
{
    cout << "Using correctly cout (critical section)" << "\n";
    cout << "Beginning" << "\n";

    #pragma omp parallel num_threads(4)
    {
        int nts = omp_get_num_threads();
        int nt  = omp_get_thread_num();
        #pragma omp critical
        {
            cout << "Hi OpenMP! "
                 << "I'm "    << nt
                 << ". Come " << nts
                 << "\n";
        }
    }

    cout << "When appear?" << "\n";
}

void phello_omp()
{
    cout << "Using correctly cout (visual asinc)" << "\n";

    omp_log_inic();

    omp_logger << "Beginning" << std::endl;

    #pragma omp parallel num_threads(4)
    {
        omp_log_inic_parented();

        int nts = omp_get_num_threads();
        int nt  = omp_get_thread_num();
        omp_logger << "Hi OpenMP! "
                << "I'm "    << nt
                << ". Come " << nts
                << std::endl;
    }

    cout << "When appear?" << "\n";

    omp_log_finalize();
}

void preport_omp()
{
    cout << "Using correctly cout (visual asinc)" << "\n";

    omp_log_inic();

    omp_logger << "Beginning" << std::endl;

    omp_set_nested(1);

    #pragma omp parallel num_threads(2)
    {
        omp_log_inic_parented();
        
        int nts = omp_get_num_threads();
        int nt  = omp_get_thread_num();
        omp_logger << "(" << nt <<"," << nts<<")" << std::endl;
        #pragma omp parallel num_threads(2)
        {
            omp_log_inic_parented();

            int nts = omp_get_num_threads();
            int nt  = omp_get_thread_num();
            omp_logger << "(" << nt <<"," << nts<<")" << std::endl;
        }
    }

    cout << "When appear?" << "\n";

    omp_log_finalize();
}



