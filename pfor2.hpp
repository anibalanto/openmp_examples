#pragma once
#include "register_thread.hpp"
#include "function_loaded.hpp"

void pfor2()
{
    static int const cant = 900000000;
    {
        RegisterThread regist("secuential");
        regist.registrate("begin > " + to_string(0, cant));
        for (int i = 0; i < cant; i++);
        regist.registrate("end   > " + to_string(0, cant));
    }
    
    {
        RegisterThread regist("parallel");
        int npoints = cant;
        int iam, nt, ipoints, istart;
        #pragma omp parallel default(shared) private(iam,nt,ipoints,istart)
        {
            iam = omp_get_thread_num();
            nt =  omp_get_num_threads();
            ipoints = npoints / nt;    /* size of partition */
            istart = iam * ipoints;  /* starting array index */
            if (iam == nt-1)     /* last thread may do more */
            {
                ipoints = npoints - istart;
            }
            regist.registrate("begin > " + to_string(istart, ipoints));
            int i = 0;
            for ( ; i < ipoints; i++);
            regist.registrate("end   > i= "
                            + std::to_string(i)
                            + " "
                            + to_string(istart, ipoints));
        }
    }
}
//functions.push_bnack(LOADER(pfor2));
/*FunctionLoaded load_pfor2()
{
    return {"pfor2", pfor2};
}*/