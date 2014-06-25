#pragma once
#include <set>
#include <map>
#include <algorithm>
#include <vector>
#include <sstream>
#include <boost/shared_ptr.hpp>

class tree_vertex;

typedef boost::shared_ptr<tree_vertex> tree_vertex_shared_ptr;

class tree_vertex {
  private:
    //char letter;
    bool is_root;
    std::map<char, tree_vertex_shared_ptr > children; // really im not sure now what to store
    std::vector<std::pair<size_t, size_t> > positions;
  public:
    //typedef std::map<char, tree_vertex_shared_ptr >::iterator ChildIterator;
    tree_vertex(): is_root(true) {}
    tree_vertex(char l): is_root(false) {}
  public:
    //method returns pointer to found or newly created object
    tree_vertex* add_next(char l) {
      std::map<char, tree_vertex_shared_ptr >::iterator iter = children.find(l);
      if (iter != children.end()) {
        return iter->second.get();
      }
      else {
        children[l] = tree_vertex_shared_ptr(new tree_vertex(l));
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
    std::vector<std::pair<size_t, size_t> > get_all_pairs() {
      std::vector<std::pair<size_t, size_t> > result;
      result.insert(result.begin(), positions.begin(), positions.end());
      for (std::map<char, tree_vertex_shared_ptr >::iterator iter = children.begin(); iter != children.end(); ++iter) {
        std::vector<std::pair<size_t, size_t> > r = iter->second->get_all_pairs();
        result.insert(result.begin(), r.begin(), r.end());
      }
      return result;
    }

    void clear() {
      children.clear();
    }
    //methods adds some information about tree node to leaf
    void add_info(size_t string_no, size_t index) {
      positions.push_back(std::make_pair(string_no, index));
    }
};

class SuffixTree {
  protected:
    void addSuffix(size_t string_no, size_t index) {
      tree_vertex * next = &root;
      for (size_t i = index; i < string_container[string_no].size(); i++){
        next = next->add_next(string_container[string_no][i]);
      }
      next->add_info(string_no, index);
    }
  public:
    SuffixTree() {}
    //method clears all resources
    void init() {
      string_container.clear();
      root.clear();
    }
    void addString(std::string const & reference_str, std::string const & str) {
      string_container.push_back(str);
      references_container.push_back(reference_str);
      for (size_t i = 0; i < _str.length(); i++) {
        addSuffix(string_container.size() - 1, i);
      }
    }

    std::set<std::string > findPattern(std::string const& pattern) {
      std::set<std::string> result;
      tree_vertex * next = &root;
      for (size_t i = 0; i < pattern.size(); i++){
        next = next->find_next(pattern[i]);
        if (next == NULL)
          return result; // string not found - return empty object
      }
      std::vector<std::pair<size_t, size_t> > all_pairs = next->get_all_pairs();
      for (std::vector<std::pair<size_t, size_t> >::iterator iter = all_pairs.begin(); iter != all_pairs.end(); ++ iter) {
        result.insert(references_container[iter->first]);
      }
      return result;
    }
  private:
    std::vector<std::string> string_container, references_container;
    tree_vertex root;
};
