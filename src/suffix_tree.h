#pragma once
#include <memory>
#include <set>
#include <map>
#include <algorithm>
#include <vector>
#include <sstream>
#include "ivisitor.h"

class tree_vertex {
  public:
    //typedef std::map<char, std::shared_ptr<tree_vertex> >::iterator ChildIterator;
    tree_vertex(): is_root(true) {}
    tree_vertex(char l): is_root(false) {}

    //method returns pointer to found or newly created object
    tree_vertex* add_next(char l) {
      std::map<char, std::shared_ptr<tree_vertex> >::iterator iter = children.find(l);
      if (iter != children.end()) {
        return iter->second.get();
      }
      else {
        children[l] = std::shared_ptr<tree_vertex>(new tree_vertex(l));
        return children[l].get();
      }
    }

    tree_vertex* find_next(char l) {
      std::map<char, std::shared_ptr<tree_vertex> >::iterator iter = children.find(l);
      if (iter!= children.end()) {
        return iter->second.get();
      }
      return NULL;
    }

    //method collects and returns information about all occurencies - string_no and position
    std::vector<std::pair<size_t, size_t> > get_all_pairs() {
      std::vector<std::pair<size_t, size_t> > result;
      result.insert(result.begin(), positions.begin(), positions.end());
      for (std::map<char, std::shared_ptr<tree_vertex> >::iterator iter = children.begin(); iter != children.end(); ++iter) {
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
  private:
    //char letter;
    bool is_root;
    std::map<char, std::shared_ptr<tree_vertex> > children; // really im not sure now what to store
    std::vector<std::pair<size_t, size_t> > positions;

  //    std::set<size_t> start_positions;
};
class SuffixTree{
  protected:
    void addSuffix(size_t string_no, size_t index) {
      tree_vertex<SuffixTree> * next = &root;
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
    void addString(std::string const & str) {
      std::string _str = std::string(str.c_str());
      string_container.push_back(_str);
      for (size_t i = 0; i < _str.length(); i++) {
        addSuffix(string_container.size() - 1, i);
      }
    }

    std::map<std::string, std::set<size_t> > findPattern(std::string const& pattern) {
      std::map<std::string, std::set<size_t> > result;
      tree_vertex<SuffixTree> * next = &root;
      for (size_t i = 0; i < pattern.size(); i++){
        next = next->find_next(pattern[i]);
        if (next == NULL)
          return result; // string not found - return empty object
      }
      std::vector<std::pair<size_t, size_t> > all_pairs = next->get_all_pairs();
      for (std::vector<std::pair<size_t, size_t> >::iterator iter = all_pairs.begin(); iter != all_pairs.end(); ++ iter) {
        std::map<std::string, std::set<size_t> >::iterator iter2 = result.find(string_container[iter->first]);
        if (iter2 == result.end()) {

        }
        else {

        }
        result[string_container[iter->first]].insert(iter->second);
      }
      return result;
    }
  private:
    std::vector<std::string> string_container;
    tree_vertex<SuffixTree> root;
};
