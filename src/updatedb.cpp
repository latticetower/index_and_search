#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

#include <iostream>
//#include <iterator>
#include <string>
#include "folder_runner.h"
#include "suffix_tree.h"

//using namespace std;

void process_folder(SuffixTree & tree, std::string const& root_folder) {
    try
  	{
      FolderRunner runner(root_folder);
      runner.run();
      std::vector<std::pair<std::string, std::string> > const paths = runner.get_paths();

      for (std::vector<std::pair<std::string, std::string> >::const_iterator iter = paths.begin();
           iter != paths.end(); ++iter) {
        tree.addString(iter->first, iter->second);
      }
    }

    catch (const fs::filesystem_error& ex)
    {
      std::cout << ex.what() << '\n';
    }

}

bool parse_cmd_options(int argc, char*argv[], std::string & database_root_folder, std::string & output_file_name) {
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("database-root", po::value <std::string > (), "FOLDER to build index for")
      ("output", po::value <std::string>(), "FILE to save resulting index")
  ;
  po::positional_options_description p;
  p.add("output", 1);
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
            options(desc).positional(p).run(), vm);
  po::notify(vm);
  if (vm.count("help")) {
      std::cout << desc << "\n";
      return false;
  }
  if (vm.count("database-root"))
  {
    std::cout << "root folder is: " << vm["database-root"].as< std::string>() << "\n";
    database_root_folder = vm["database-root"].as< std::string>();
  }
  else
    return false;

  if (vm.count("output"))
  {
      std::cout << "File for saving information is: " << vm["output"].as< std::string >() << "\n";
      output_file_name = vm["output"].as<std::string>();
  }
  else
    return false;
  return true;
}



int main(int argc, char* argv[])
{
    try {
      std::string database_root_folder, output_file_name;
      if (!parse_cmd_options(argc, argv, database_root_folder, output_file_name))
        return 1;
      SuffixTree tree;
      process_folder(tree, database_root_folder);
      std::set<std::string> res = tree.findPattern("task");
      for (std::set<std::string >::const_iterator iter = res.begin();
				   iter!= res.end(); ++iter) {
            std::cout << *iter << std::endl;
      }
			SuffixTree::save_to_file(tree, output_file_name);

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
