#include<queue>
#include <iostream>

void pproducer_consumer()
{
    {
        std::queue<int> q;
        int v = 0,
            cant = 100,
             njobs = 0;
        bool flag_end = false;
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                for(int i = 0; i < cant; i++)
                {
                    #pragma omp flush (njobs)
                    while (njobs < 10)
                    {
                        #pragma omp flush (njobs)
                    }

                    q.push(v++);

                    #pragma omp flush
                    njobs++;
                    #pragma omp flush (njobs)
                }
                #pragma omp flush
                flag_end = true;
                #pragma omp flush (flag_end)
            }
            #pragma omp section
            {
                #pragma omp flush (flag_end)
                while (!flag_end)
                {
                    #pragma omp flush (njobs)
                    while (njobs == 0)
                    {
                        #pragma omp flush (njobs)
                    }

                    auto r = q.front();
                    std::cout << r << "\n";
                    q.pop();
                    #pragma omp flush
                    njobs--;
                    #pragma omp flush (njobs)
                }


                #pragma omp flush
                while (!q.empty())
                {
                    auto r = q.front();
                    std::cout << r << "\n";
                    q.pop();
                    #pragma omp flush
                    njobs--;
                    #pragma omp flush (njobs)
                }
            }
        }
    }
}
