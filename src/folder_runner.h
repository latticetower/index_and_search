#pragma once
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class FolderRunner{
  public:
    FolderRunner(std::string const& root_path): root_folder_path(root_path) {
    }

    //this method makes all computations
    void run() {
        paths.clear();
        process_folder(root_folder_path);
    }

    void process_folder(std::string const& folder_name) {
        fs::path processed_path(folder_name);
        if (!fs::exists(processed_path))
            throw std::runtime_error("Directory " + folder_name + " not found");
        if (!is_directory(processed_path))
            throw std::runtime_error(folder_name + " is not a directory");
        for (fs::directory_iterator iter = fs::directory_iterator(processed_path);
                                    iter != fs::directory_iterator(); ++iter) {
            if (fs::is_directory(iter->path().native())) {
                paths.push_back(std::make_pair(iter->path().native(), iter->path().filename().native()));
                process_folder(iter->path().native());
            }
            if (fs::is_regular_file(iter->path().native())) {
              std::cout << iter->path().filename().native()<<std::endl;
                paths.push_back(std::make_pair(iter->path().native(), iter->path().filename().native()));
            }
        }
    }

    std::vector<std::pair<std::string, std::string> > const & get_paths() {
        return paths;
    }
  private:
    std::string root_folder_path;
    std::vector<std::pair<std::string, std::string> > paths;
};
