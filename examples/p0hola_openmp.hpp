#pragma once
#include "register_thread.hpp"
#include "function_loaded.hpp"

using namespace std;

void phola_openmp()
{
    cout << "Permiso" << "\n";

    omp_set_num_threads(4);
    #pragma omp parallel
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

    omp_set_num_threads(4);
    #pragma omp parallel
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

    static int const th_cant = 4;
 
    RegisterThread regist("hello", th_cant);
    
    regist.registrate("Entramos"); 

    omp_set_num_threads(th_cant);
    #pragma omp parallel
    {
        regist.registrate("Hola OpenMP!");
    }

    regist.registrate("Salimos"); 
}

FunctionLoaded load_phello_omp()
{
    return {"phello_omp", phello_omp};
}