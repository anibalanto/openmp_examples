/*
 * Ejemplo OpenMP
 * NestedThreads
 *
 * Documentación: https://docs.oracle.com/cd/E19205-01/819-5270/aewbc/index.html
 *
 * @autor Anibal Fernando Antonelli
 * Catedra: Sistemas De Computación Distribuidos
 * Facultad de Ingeniería
 * Universidad Nacional de Mar del Plata
 */

#pragma once
#include <iostream>
#include <omp.h>
#include "../include/ompe/omp_log.hpp"


void phello_nested_named( const std::string && name)
{

    omp_log_inic();

    #pragma omp parallel num_threads(4)
    {
        omp_log_inic_parented();

        omp_logger << "Ciudad numero "
                << omp_get_thread_num() << std::endl;

        #pragma omp parallel num_threads(4)
        {
            omp_log_inic_parented();

            omp_logger << "Casa numero "  << omp_get_thread_num() << " (round 1)" <<std::endl;

            int nts = omp_get_num_threads();
            int nt  = omp_get_thread_num();
            omp_logger << "Hola OpenMP! "
                    << "Yo soy "    << nt
                    << ". Vinimos " << nts
                    << std::endl;

        }

        #pragma omp parallel num_threads(5)
        {
            omp_log_inic_parented();

            omp_logger << "Casa numero "  << omp_get_thread_num() << " (round 2)" << std::endl;

            int nts = omp_get_num_threads();
            int nt  = omp_get_thread_num();
            omp_logger << "Hola OpenMP! "
                    << "Yo soy "    << nt
                    << ". Vinimos otra vez! Somos " << nts
                    << std::endl;

        }
    }

    omp_logger << "Nos vamos!" << std::endl;

    omp_log_finalize();
}

void phello_nested()
{
    /*omp_set_nested(0);
    phello_nested_named("nested descatived");*/

    omp_set_nested(1);
    phello_nested_named("nested actived");
}
