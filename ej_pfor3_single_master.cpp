#include "register_thread.hpp"

#define REP 5

int main()
{
    {
        RegisterThread regist("singel_master");
        omp_set_nested(1);
        omp_set_dynamic(0);
        #pragma omp parallel num_threads(7)
        {
            for(int i=0; i < REP; i++)
                regist.registrate("parallel 1." + std::to_string(i));
            #pragma omp parallel
            {
                #pragma omp single
                {
                    for(int i=0; i < REP; i++)
                        regist.registrate("single 1." + std::to_string(i));
                }
            }
            for(int i=0; i < REP; i++)
                regist.registrate("parallel 2." + std::to_string(i));
            #pragma omp master
            {
                for(int i=0; i < REP; i++)
                    regist.registrate("master 1." + std::to_string(i));
            }
            for(int i=0; i < REP; i++)
                regist.registrate("parallel 3." + std::to_string(i));
            #pragma omp barrier
            #pragma omp single
            {
                regist.registrate("----------------barrier------------------");
            }
            for(int i=0; i < REP; i++)
                regist.registrate("parallel 4." + std::to_string(i));

        }
    }
    return 0;
}
