#include "register_thread.hpp"

#define CANTX 10
#define CANTY 10
#define CANTZ 10

static RegisterThread regist("sections");

void xAxis()
{
    regist.registrate("inic x");
    for(int i = 0; i < CANTX; i++)
    {
        regist.registrate("oper x " + std::to_string(i));
    }
    regist.registrate("end  x");

}

void yAxis()
{        
    regist.registrate("inic y");
    for(int j = 0; j < CANTY; j++)
    {
        regist.registrate("oper y " + std::to_string(j));
    }
    regist.registrate("end  y");
}

void zAxis()
{
    regist.registrate("inic z");
    for(int k = 0; k < CANTZ; k++)
    {
        regist.registrate("oper z " + std::to_string(k));
    }
    regist.registrate("end  z");
}

int main()
{
    #pragma omp parallel sections
    {
        #pragma omp section
        xAxis();

        #pragma omp section
        yAxis();

        #pragma omp section
        zAxis();
    }
    return 0;
}

