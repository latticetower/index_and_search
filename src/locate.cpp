#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace po = boost::program_options;

#include <iostream>
#include <iterator>
using namespace std;

int main(int ac, char* av[])
{
    try {

        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("database", po::value <string > (), "database file with directory structure index")
            ("pattern", po::value <string>(), "PATTERN to search for in database file")
        ;

        po::positional_options_description p;
        p.add("pattern", 1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            return 0;
        }
        if (vm.count("database"))
        {
            cout << "Input database file is: "
                 << vm["database"].as< string>() << "\n";
        }

        if (vm.count("pattern"))
        {
            cout << "Search pattern is: "
                 << vm["pattern"].as< string >() << "\n";
        }

    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    }

    return 0;
}
