#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>

#include "suffix_tree.h"

namespace fs = boost::filesystem;


template<class FolderRunner>
class ThreadPool{
  public:
    typedef void (FolderRunner::* processor_func_ptr)(std::string const & );

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

    void addTask(FolderRunner& fr, std::string const& folder) {
      io_service.post(boost::bind(&FolderRunner::process_folder, &fr, folder));
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
        thread_pool.addTask(*this, root_folder_path);
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

            _mutex.lock();
            _tree.addString(iter->path().native(), iter->path().filename().native() );
            _mutex.unlock();

            if (fs::is_directory(iter->path().native())) {
                thread_pool.addTask(*this, iter->path().native());
            }
        }
    }

  private:
    std::string root_folder_path;
    ThreadPool<FolderRunner> thread_pool;
    boost::mutex _mutex;
    SuffixTree & _tree;
};
