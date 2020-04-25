#pragma once
#include "register_thread.hpp"
#include "function_loaded.hpp"

void pfor3_single_master()
{
    static int const rep = 3;
    {
        {
            omp_set_nested(1);
            omp_set_dynamic(0);
            #pragma omp parallel num_threads(2)
            {
                RegisterThread reg_parent("test");
                int to_regist_a = 0;
                #pragma omp parallel num_threads(2)
                {
                    RegisterThread reg_child(reg_parent);
                    #pragma omp critical
                    {
                        ++to_regist_a;
                        reg_child.regist("hilo dice A" + std::to_string(to_regist_a));
                    }
                }
                int to_regist_b = 0;
                #pragma omp parallel num_threads(2)
                {
                    RegisterThread reg_child(reg_parent);
                    #pragma omp single
                    {
                        reg_child.regist << "hilo dice B";
                    }
                    #pragma omp critical
                    {
                        ++to_regist_b;
                        reg_child.regist("hilo dice B" + std::to_string(to_regist_b));
                    }
                }
        }
    }
}
