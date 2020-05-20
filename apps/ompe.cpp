/*
 * OpenMP Example
 *
 * @autor Anibal Fernando Antonelli
 * Catedra: Sistemas De Computación Distribuidos
 * Facultad de Ingeniería
 * Universidad Nacional de Mar del Plata
 */

#include <vector>

#include "../include/ompe/function_loaded.hpp"

/*#include "p0hola_openmp.hpp"
#include "p1for.hpp"
#include "p2for.hpp"
#include "p3numerical_integration.hpp"
#include "p4.1nested.hpp"
#include "p5for_nowait.hpp"
#include "p6sections.hpp"
#include "p7task.hpp"
#include "p8sin_integration.hpp"
#include "pproducer_consumer.hpp"*/
#include "../examples/p4for_single_master.hpp"
#include "../examples/p4.2nested.hpp"

#include "../include/ompe/clipp.h"
#include "../include/ompe/table.hpp"

using namespace std;
using namespace clipp;

static string soft_name = "ompe";

#define omp_log_test

int main(int argc, char* argv[])
{
    vector<FunctionLoaded> functions
        { { /*loader(phola_openmp),
            loader(pcritical_hola_openmp),
            loader(phello_omp),
            loader(pfor1),
            loader(pfor2),
            loader(pnumerical_integration),
            loader(pfor3_single_master),
            loader(pnested),
            loader(pfor4_nowait),
            loader(psections),
            loader(ptask),
            loader(psin_integration),
            loader(pproducer_consumer)*/
            loader(pnested_compare),
            loader(pfactorial_nested),
            loader(p4for_single_master) } };

    int fn = -1;
    bool list = false,
         help = false;

    auto cli = group (
        opt_value("fnum", fn).doc("Número de función a ejecutar"),
        option("-l", "--list").set(list).doc("Lista de funciones con su número de función"),
        option("-h", "--help").set(help).doc("Ayuda")
     );

    auto usage = [&cli, &argv]() { return usage_lines(cli, soft_name); };

    if(parse(argc, argv, cli))
    {
        if(help)
        {
            auto fmt = doc_formatting{}
                        .first_column(7)
                        .doc_column(15)
                        .last_column(99);

            std::cout << "uso:" << usage() << '\n';
            std::cout << "Estos son comandos de ompe (OpenMP Example)\n"
                      << documentation(cli, fmt) << '\n';
        }
        else
        {
            if(fn >= 0)
            {
                if(fn < functions.size())
                {
                    functions[fn].func();
                }
                else
                {
                    std::cout << "No existe ese numero de función\n";
                }
            }
            if (list)
            {
                Table t {{6, 20}};

                std::cout << "Número función con su respectivo nombre\n";
                std::cout << t.header(0) << "fnum"
                          << t.header(1) << "Nombre"
                          << std::endl;

                for (auto i = 0; i < functions.size(); i++)
                {
                    std::cout << t.column(0) << i
                              << t.column(1) << functions[i].name
                              << std::endl;
                }
            }
        }
        if(fn < 0 && !list && !help)
        {
            std::cout << usage() << '\n';
        }
    }
    else
    {
        std::cout << "uso: " << usage() << '\n';
    }

    return 0;
}
