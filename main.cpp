#include <vector>

#include "function_loaded.hpp"

#include "phello_omp.hpp"
#include "pfor1.hpp"
#include "pfor2.hpp"
#include "pfor3_single_master.hpp"
#include "pfor4_nowait.hpp"
#include "psections.hpp"
#include "ptask.hpp"
#include "pnumerical_integration.hpp"


#include "clipp.h"
#include "table.hpp"

using namespace std;
using namespace clipp;

static string soft_name = "omp_examp";

int main(int argc, char* argv[])
{
    vector<FunctionLoaded> functions { { loader(phello_omp),
                                         loader(pfor1),
                                         loader(pfor2),
                                         loader(pfor3_single_master),
                                         loader(pfor4_nowait),
                                         loader(psections),
                                         loader(ptask),
                                         loader(pnumerical_integration) } };

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
