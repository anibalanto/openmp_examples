#pragma once
#include "register_thread.hpp"

std::string show_ths_ids(int first, int second = -1, int third = -1)
{
    std::string s_first      = std::to_string(first);
    std::string s_second  = (second != -1)?  std::to_string(second)  : " ";
    std::string s_third = (third != -1)? std::to_string(third) : " ";

    s_first =      "(" + s_first      + ") " ;
    s_second =  "[" + s_second  + "] ";
    s_third = "{" + s_third + "}";

    return s_first+ s_second + s_third;
}

void func_nested(const std::string &name) 
{
    RegisterThread regist(name, 6, false);

    regist.registrate(" * id del thread 1er nivel");
    regist.registrate(" |   * id del thread 2do nivel");
    regist.registrate(" |   |   * id del thread 3er nivel");
    regist.registrate(" |   |   |");

    omp_set_dynamic(0);
    #pragma omp parallel num_threads(2)
    {
        int th_id1 = omp_get_thread_num();
        regist.registrate(
                        show_ths_ids(th_id1)
                    + " 1 st LEVEL");

        #pragma omp parallel num_threads(2)
        {
            int th_id2 = omp_get_thread_num();
            regist.registrate(
                    show_ths_ids(th_id1, th_id2)
                    + " 2 nd LEVEL"
                    );
            #pragma omp parallel num_threads(2)
            {
                int th_id3 = omp_get_thread_num();
                regist.registrate(
                    show_ths_ids(th_id1, th_id2, th_id3)
                    + " 3 th LEVEL"
                    );
            }
        }
    }
}

void pnested()
{

    omp_set_nested(1);
    func_nested("nested actived");

    omp_set_nested(0);
    func_nested("nested desactived");
}