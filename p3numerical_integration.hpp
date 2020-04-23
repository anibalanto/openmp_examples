#pragma once
/*
Integración numerica
∫ ( 4 / (1+x^2) ) dx = π
*/



#include "register_thread.hpp"
#include "function_loaded.hpp"

void pnumerical_integration()
{
    static long num_steps = 100000;

    double step = 1.0/(double) num_steps;

    {
        RegisterThread regist("numerical_integration");
 
        regist.registrate("Calcular PI"); 

        int i;
        double x, pi, sum = 0.0;
        
        for (i=0;i< num_steps; i++)
        {
            x = (i+0.5)*step;
            sum = sum + 4.0/(1.0+x*x);
        }
        pi = step * sum;

        regist.registrate("doed > x = " 
                    + std::to_string(x) 
                    + " sum = "
                    + std::to_string(sum));

        regist.registrate("PI: " + std::to_string(pi)); 
    }


    {
        static int const th_cant = 4;

        RegisterThread regist("parallel_numerical_integration", th_cant);
        regist.registrate("Calcular PI"); 

        double sum[th_cant] = { 0.0 };
        double pi = 0.0;
        double x = 0.0;

        int npoints = num_steps;
        int iam, nt, ipoints, istart;
        //int i = 0;

        omp_set_num_threads(th_cant);
        #pragma omp parallel default(shared) private(iam,nt,ipoints,istart, x)
        {
            
            iam = omp_get_thread_num();
            nt =  omp_get_num_threads();
            ipoints = npoints / nt;    // size of partition 
            istart = iam * ipoints;  // starting array index 
            if (iam == nt-1)     // last thread may do more 
            {
                ipoints = npoints - istart;
            }
            regist.registrate(to_string(istart, ipoints) + "b>"
                        + " ipoints= " 
                        + std::to_string(ipoints) );
            x = 0.0;

            int i;
            for(i = 0; i< ipoints; i++)
            {
                x = ((i + istart) + 0.5)*step;
                sum[iam] = sum[iam] + 4.0/(1.0+x*x);
            }
            regist.registrate(to_string(istart, ipoints)
                    + "e>"
                    + " i = " 
                    + std::to_string(i) 
                    + " x = " 
                    + std::to_string(x) 
                    + " sum = "
                    + std::to_string(sum[iam]) 
                    + " ");
        }

        for(int i= 0; i< th_cant; i++)
        {
            pi += step * sum[i];
        }

        regist.registrate("PI calculado: " + std::to_string(pi)); 
    }
    
}

