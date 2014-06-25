#pragma once
//#include <memory>
#include <set>
#include <map>
#include <algorithm>
#include <vector>
#include <fstream>
#include <boost/shared_ptr.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/split_member.hpp>


class tree_vertex;

typedef boost::shared_ptr<tree_vertex> tree_vertex_shared_ptr;

class tree_vertex {
  private:
    friend class boost::serialization::access;

template<class Archive>
void serialize(Archive & ar, const unsigned int version)
{
    ar & string_numbers;
    ar & children;
}
    /*
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const {
        ar & string_numbers;
        size_t array_size = children.size();
        ar & array_size;
        for (std::map<char, tree_vertex_shared_ptr >::const_iterator iter = children.begin();
             iter!= children.end(); ++iter) {
            ar & iter->first;
            ar & (*iter->second.get());
        }
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int version) {
        ar & string_numbers;
        size_t arr_size, map_size;
        ar & arr_size;
        char letter;
        tree_vertex buf;

        for (size_t i = 0; i < arr_size; i++) {
            ar & letter;
            ar & buf;
            children[letter] = tree_vertex_shared_ptr(new tree_vertex(buf));
        }
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER() */

  public:
    tree_vertex() {}

    tree_vertex(tree_vertex const & orig) {
        string_numbers.clear();
        string_numbers.insert(orig.string_numbers.begin(), orig.string_numbers.end());
        children.clear();
        for (std::map<char, tree_vertex_shared_ptr >::const_iterator iter = orig.children.begin();
             iter!= orig.children.end(); ++ iter) {
          children[iter->first] = iter->second;
        }

    }
  public:
    //method returns pointer to found or newly created object
    tree_vertex* add_next(char l) {
        std::map<char, tree_vertex_shared_ptr >::iterator iter = children.find(l);
        if (iter != children.end()) {
            return iter->second.get();
        }
        else {
            children[l] = tree_vertex_shared_ptr(new tree_vertex());
            return children[l].get();
        }
    }

    tree_vertex* find_next(char l) {
        std::map<char, tree_vertex_shared_ptr >::iterator iter = children.find(l);
        if (iter != children.end()) {
          return iter->second.get();
        }
        return NULL;
    }

  public:
    //method collects and returns information about all occurencies - string_no and position
    std::set<size_t> get_all_string_numbers() {
        std::set<size_t> result;
        result.insert(string_numbers.begin(), string_numbers.end());
        for (std::map<char, tree_vertex_shared_ptr >::iterator iter = children.begin(); iter != children.end(); ++iter) {
            std::set<size_t> r = iter->second->get_all_string_numbers();
            result.insert(r.begin(), r.end());
        }
        return result;
    }

    void clear() {
        children.clear();
        string_numbers.clear();
    }

    //method adds some information about tree node to leaf
    void add_info(size_t string_no) {
        string_numbers.insert(string_no);
    }
    bool operator == (tree_vertex & comp) {
        if (string_numbers.size() != comp.string_numbers.size()) {
            std::cout << "string_numbers size\n";
            return false;
        }
        for (std::set<size_t>::iterator iter = string_numbers.begin();
                                      iter != string_numbers.end(); ++iter) {
            if (comp.string_numbers.find(*iter) == comp.string_numbers.end()) {
                std::cout << "string_numbers element\n";
                return false;
            }
        }
        for (std::map<char, tree_vertex_shared_ptr >::iterator iter = children.begin();
                        iter!= children.end(); ++iter) {
            if (comp.children.find(iter->first) == comp.children.end()) {
              std::cout << "children keys " << iter->first << std::endl;
                for (std::map<char, tree_vertex_shared_ptr >::iterator iter2 = children.begin();
                                iter2!= children.end(); ++iter2) {
                std::cout << iter2->first << " ";
                                }
                std::cout << "\niter 2: " ;
                for (std::map<char, tree_vertex_shared_ptr >::iterator iter2 = comp.children.begin();
                                iter2!= comp.children.end(); ++iter2) {
                std::cout << iter2->first << " ";
                                }
std::cout <<std::endl;
                return false;
            }
            if ((*comp.children[iter->first].get()) != (*iter->second.get())) {
                std::cout << "children values\n";
                return false;
            }
        }
        return true;

    }
    bool operator != (tree_vertex & comp) {
        return !(*this == comp);
    }
  private:
    std::map<char, tree_vertex_shared_ptr > children; // really im not sure now what to store
    std::set<size_t> string_numbers;
};

BOOST_SERIALIZATION_SHARED_PTR(tree_vertex)

class SuffixTree {
  private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & string_container;
        ar & references_container;
        ar & root;
    }
  protected:
    void addSuffix(size_t string_no, size_t index) {
        tree_vertex * next = &root;
        for (size_t i = index; i < string_container[string_no].size(); i++){
          next = next->add_next(string_container[string_no][i]);
        }
        next->add_info(string_no);
    }
  public:
    SuffixTree() {}

    //method clears all resources
    void init() {
        string_container.clear();
        references_container.clear();
        root.clear();
    }

    void addString(std::string const & reference_str, std::string const & str) {
        string_container.push_back(str);
        references_container.push_back(reference_str);
        for (size_t i = 0; i < str.length(); i++) {
            addSuffix(string_container.size() - 1, i);
        }
    }

    std::set<std::pair<std::string, std::string> > findPattern(std::string const& pattern) {
        std::set<std::pair<std::string, std::string> > result;
        tree_vertex * next = &root;
        for (size_t i = 0; i < pattern.size(); i++){
            next = next->find_next(pattern[i]);
            if (next == NULL)
                return result; // string not found - return empty object
        }
        std::set<size_t> all_strings = next->get_all_string_numbers();
        for (std::set<size_t >::iterator iter = all_strings.begin(); iter != all_strings.end(); ++ iter) {
            result.insert(std::make_pair(references_container[*iter], string_container[*iter]));
        }
        return result;
    }

    static void save_to_file(SuffixTree tree, std::string const & output_file_name) {
        std::ofstream ofs(output_file_name.c_str());
        boost::archive::text_oarchive oa(ofs);
        oa << tree;
    }

    static void load_from_file(SuffixTree& tree, std::string const& input_file_name) {
        std::ifstream ifs(input_file_name.c_str());
        boost::archive::text_iarchive ia(ifs);
        ia >> tree;
    }

    bool operator == (SuffixTree & comp) {
      std::cout << "in ==\n";
        if (string_container.size() != comp.string_container.size()) {
            std::cout << "string container size\n";
            return false;
        }
        for (size_t i = 0; i < string_container.size(); i++) {
            if (string_container[i] != comp.string_container[i]) {
                std::cout << "string container element\n";
                return false;
            }
            if (references_container[i] != comp.references_container[i]) {
                std::cout << "references_container element\n";
                return false;
            }
        }
        return root == comp.root;
    }
    bool operator != (SuffixTree & comp) {
      return !(*this==comp);
    }
  private:
    std::vector<std::string> string_container, references_container;
    tree_vertex root;
};
