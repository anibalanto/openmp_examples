#include <vector>

#include "function_loaded.hpp"

#include "p0hola_openmp.hpp"
#include "p1for.hpp"
#include "p2for.hpp"
#include "p3numerical_integration.hpp"
#include "p4for_single_master.hpp"
#include "p5for_nowait.hpp"
#include "p6sections.hpp"
#include "p7task.hpp"
#include "p8sin_integration.hpp"

#include "clipp.h"
#include "table.hpp"

#include "joaquin/p0hola_openmp.hpp"
#include "Juan/p0hola_openmp.hpp"
#include "Rocío/p0hola_openmp.hpp"
#include "Luciana/p0hola_openmp.hpp"
#include "GastonMigone/phello_omp_gaston.hpp"

using namespace std;
using namespace clipp;

static string soft_name = "omp_examp";

int main(int argc, char* argv[])
{
    vector<FunctionLoaded> functions 
        { { loader(phola_openmp),
            loader(pcritical_hola_openmp),
            loader(phello_omp),
            loader(pfor1),
            loader(pfor2),
            loader(pnumerical_integration),
            loader(pfor3_single_master),
            loader(pfor4_nowait),
            loader(psections),
            loader(ptask),
            loader(psin_integration),
            loader(phello_omp_juan),
            loader(phello_omp_gaston),
            loader(phello_omp_rocio),
            loader(phello_omp_joaquin),
            loader(phello_omp_Luciana) } };

    int fn = -1;
    bool list = false,
         help = false;

    auto cli = group ( 
        opt_value("fnum", fn).doc("number of function to execute"),
        option("-l", "--list").set(list).doc("list functions"),
        option("-h", "--help").set(help).doc("help")
     );

    auto usage = [&cli, &argv]() { return usage_lines(cli, soft_name); };

    if(parse(argc, argv, cli))
    {
        if(fn >= 0 && fn < functions.size())
        {
            functions[fn].func();
        }
        if (list)
        {
            Table t {{6, 20}};

            std::cout << t.header(0) << "num"
                      << t.header(1) << "name"
                      << std::endl;

            for (auto i = 0; i < functions.size(); i++)
            {
                std::cout << t.column(0) << i
                          << t.column(1) << functions[i].name
                          << std::endl;
            }
        }
        if(help)
        {
            auto fmt = doc_formatting{}
                        .first_column(7)
                        .doc_column(15)
                        .last_column(99);

            std::cout << usage() << '\n';
            std::cout << documentation(cli, fmt) << '\n';
        }
    }
    else
    {
        std::cout << usage() << '\n';
    }

    return 0;
}