#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace po = boost::program_options;

#include <iostream>
#include <iterator>
#include "suffix_tree.h"


bool parse_cmd_options(int argc, char*argv[], std::string & database_file, std::string & pattern) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("database", po::value <std::string > (), "database file with directory structure index")
        ("pattern", po::value <std::string>(), "PATTERN to search for in database file")
    ;
    po::positional_options_description p;
    p.add("pattern", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

    if (vm.count("database")) {
        database_file = vm["database"].as<std::string>();
    }
    else {
        std::cout  << "database key was not provided\n" << desc << "\n";
        return false;
    }

    if (vm.count("pattern")) {
        pattern = vm["pattern"].as<std::string>();
    }
    else {
        std::cout  << "pattern key was not provided\n" << desc << "\n";
        return false;
    }
    return true;
}


int main(int argc, char* argv[])
{
    try {
        std::string database_file,  pattern;
        if (!parse_cmd_options(argc, argv, database_file, pattern))
          return 1;
        SuffixTree tree;
        SuffixTree::load_from_file(tree, database_file);
        std::set<std::string> res = tree.findPattern(pattern);
        for (std::set<std::string >::const_iterator iter = res.begin();
             iter!= res.end(); ++iter) {
          std::cout << *iter << std::endl;
        }
    }
    catch(std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        std::cerr << "Exception of unknown type!\n";
    }

    return 0;
}
