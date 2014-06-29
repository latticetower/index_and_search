#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>

#include "suffix_tree.h"

namespace fs = boost::filesystem;


template<class FolderRunner, class SuffixTree>
class ThreadPool{
  public:

    ThreadPool(): io_service(), work(new boost::asio::io_service::work(io_service)) {
        num_of_threads = boost::thread::hardware_concurrency();
        for (size_t i = 0; i < num_of_threads; i++)
            thread_group.create_thread(boost::bind(&boost::asio::io_service::run, &io_service));
    }

    void stop() {
      io_service.stop();
    }

    void wait_all() {
      work.reset();
      thread_group.join_all();
    }

    ~ThreadPool() {
      stop();
      wait_all();
    }

    void addTask(FolderRunner* fr, std::string const& folder) {
        io_service.post(boost::bind(&FolderRunner::process_folder, fr, folder));
    }

    void addTask(SuffixTree* tree, std::string const& full_path, std::string const & short_path) {
        io_service.post(boost::bind(&SuffixTree::addString, tree, full_path, short_path));
    }

  private:
    boost::asio::io_service io_service;
    boost::thread_group thread_group;
    std::unique_ptr<boost::asio::io_service::work> work;
    size_t num_of_threads;
};

class FolderRunner{
  public:
    FolderRunner(std::string const& root_path, SuffixTree & tree): root_folder_path(root_path), _tree(tree) {
    }

    //this method makes all computations
    void run() {
        thread_pool.addTask(this, root_folder_path);
        thread_pool.wait_all();
    }

    void process_folder(std::string const& folder_name) {
        fs::path processed_path(folder_name);
        if (!fs::exists(processed_path))
            throw std::runtime_error("Directory " + folder_name + " not found");
        if (!is_directory(processed_path))
            throw std::runtime_error(folder_name + " is not a directory");
        for (fs::directory_iterator iter = fs::directory_iterator(processed_path);
                                    iter != fs::directory_iterator(); ++iter) {
            if (fs::is_symlink(iter->path()))
                continue;

            thread_pool.addTask(&_tree, iter->path().native(), iter->path().filename().native() );

            if (fs::is_directory(iter->path().native())) {
                thread_pool.addTask(this, iter->path().native());
            }
        }
    }

  private:
    std::string root_folder_path;
    ThreadPool<FolderRunner, SuffixTree> thread_pool;
    SuffixTree & _tree;
};
