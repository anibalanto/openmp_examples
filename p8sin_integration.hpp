//producer consume

#include "register_thread.hpp"
#include "function_loaded.hpp"
#include <math.h>

void psin_integration()
{

    static double ssumt;
    
   
    {
        RegisterThread regist("serial");

        double ssum = 0.0;

        regist.registrate("begin calculate");
        for (int i = 0; i < 10000000; i++)
        {
            ssum += sin(i*0.001);
        }
        regist.registrate("end calculate: " + std::to_string(ssum));

    }

    {
        RegisterThread regist("prod-cons-task");
        #pragma omp threadprivate(ssumt)

        double ssum = 0.0;
        
        #pragma omp parallel
        {
            ssumt = 0.0;
            

            #pragma omp single
            {
                regist.registrate("begin calculate");
                for (int i = 0; i < 10000000; i++) 
                {
                    #pragma omp task
                    {
                        //regist.registrate("begin task " + std::to_string(i));
                        ssumt += sin(i*0.001);
                    }
                }
            }

            #pragma omp taskwait
            //regist.registrate("task calculated");

            #pragma omp atomic
            ssum += ssumt;

        }

        regist.registrate("end calculate: " + std::to_string(ssum));
    }


    {
        omp_set_nested(1);
        RegisterThread regist("prod-cons-nested");
        #pragma omp threadprivate(ssumt)

        double ssum = 0.0;
        
        #pragma omp parallel
        {
            ssumt = 0.0;
            

            #pragma omp single
            {
                regist.registrate("begin calculate");
                for (int i = 0; i < 10000000; i++) 
                {
                    #pragma omp task
                    {
                        //regist.registrate("begin task " + std::to_string(i));
                        ssumt += sin(i*0.001);
                    }
                }
            }

            #pragma omp taskwait
            //regist.registrate("task calculated");

            #pragma omp atomic
            ssum += ssumt;

        }

        regist.registrate("end calculate: " + std::to_string(ssum));
    }
    


}

