#include "register_thread.hpp"

#define CANT 900000000

std::string to_string(int istart, int ipoints)
{
    return "from " + std::to_string(istart) + " to " + std::to_string(istart + ipoints);
}

void pfor2()
{
    {
        RegisterThread regist("secuential");
        regist.registrate("begin > " + to_string(0, CANT));
        for (int i = 0; i < CANT; i++);
        regist.registrate("end   > " + to_string(0, CANT));
    }
    {
        RegisterThread regist("parallel");
        int npoints = CANT;
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
            for (int i = 0; i < ipoints; i++);
            regist.registrate("end   > " + to_string(istart, ipoints));
        }
    }    
}
