#pragma once
#include <memory>
#include <set>
#include <map>
#include <algorithm>
#include <vector>
#include <sstream>
#include "ivisitor.h"
/*
bool compare_pairs(std::pair<size_t, size_t> a, std::pair<size_t, size_t> b) {
    return a.second > b.second;
 }

//ititially we assume that all vertices describe the same string substrings
template <class Graph>
class tree_vertex {
private:
  int get_id() {
    return (int)this;
  }
  std::shared_ptr<tree_vertex> next_vertex; // really im not sure now what to store
  std::shared_ptr<tree_vertex> neighbour; //next neighbour for current vertex
  size_t length;
  Graph * owner;

  std::set<size_t> start_positions;
public:
  static const unsigned int MIN_REPEATS_COUNT = 2;
  
  void set_parent(Graph * g) {
    owner = g;
  }
  
  ~tree_vertex() {
    next_vertex.reset();
    neighbour.reset();
  }
  tree_vertex(Graph * g, size_t start, size_t _length = 1): next_vertex(NULL), neighbour(NULL), length(_length), owner(g) { 
    start_positions.clear();
    start_positions.insert(start);
  }

  tree_vertex(tree_vertex & orig) {
    this->start_positions.clear();//TODO: check this
    for (std::set<size_t> ::iterator iter = orig->start_positions.begin(); iter != orig->start_positions.end(); ++iter) {
     this->start_positions.insert(*iter);
    }
    //this->start_pos = orig->start_pos;
    this->length = orig->length;
    this->next_vertex = orig->next_vertex;
    this->owner = orig->owner;
    this->neighbour = orig->neighbour;
  }

  tree_vertex() {
    tree_vertex(NULL, 0);
  }

  tree_vertex<Graph>* add_next(size_t start, size_t length = 1) {
    if (next_vertex == NULL) {
      next_vertex = std::shared_ptr<tree_vertex<Graph> >(new tree_vertex<Graph>(owner, start, length));
      return next_vertex.get();
    }
    return next_vertex->add_neighbour(start, length);

  }

  tree_vertex<Graph> * add_neighbour(size_t start, size_t _length = 1) {
    if (owner->compare(*start_positions.begin(), length, start, _length)) {
      start_positions.insert(start);
      return this;
    }
    if (neighbour == NULL) {
      neighbour = std::shared_ptr<tree_vertex<Graph> >(new tree_vertex<Graph>(owner, start, length));
      return neighbour.get();
    } 
    return neighbour->add_neighbour(start, length);
  }
  bool operator == (tree_vertex<Graph> const& obj) {
    return (owner == obj.owner) && 
      (start_positions.size() == obj.start_positions.size())  ;
  }

  void simplify() {
    if (next_vertex == NULL)
      return;
    next_vertex.get()->simplify();
    if (*this == *next_vertex.get() && next_vertex->neighbour == NULL) {
      this->length += next_vertex.get()->length;
      next_vertex = next_vertex.get()->next_vertex;
    }
    if (neighbour != NULL)
      neighbour.get()->simplify();
  }
  


  std::pair<size_t, size_t> traverse(std::set<size_t>& prefixes, size_t const & prefix_pos, size_t acc) {
    if (this->start_positions.size() < MIN_REPEATS_COUNT) {
      if (this->start_positions.find(prefix_pos - 1) != this->start_positions.end() && 
        this->start_positions.find(prefix_pos - 1 + acc) != this->start_positions.end() &&
          prefixes.find(prefix_pos - acc) != prefixes.end()) {// == *iter + length) {
            return std::make_pair(prefix_pos, acc );
      }
      else
        return std::make_pair(0, 0);
    }
    std::vector<std::pair<size_t, size_t> > candidates;
    if (neighbour != NULL) {
      if (this->start_positions.find(prefix_pos) != this->start_positions.end()) {

      } else {
        if (this->neighbour->start_positions.size() < MIN_REPEATS_COUNT) {
          if (this->start_positions.find(prefix_pos - 1) != this->start_positions.end() && 
        this->start_positions.find(prefix_pos - 1 + acc) != this->start_positions.end() &&
          prefixes.find(prefix_pos - acc) != prefixes.end()) {// == *iter + length) {
            candidates.push_back(std::make_pair(prefix_pos, acc));
          }
        }
        candidates.push_back(this->neighbour->traverse(prefixes, prefix_pos, acc));
      }
    }
    //1. проходим по всем таким участкам: если следующий за ними элемент находится в списке префиксов, то это наш кандидат
    //for (std::set<size_t>::iterator iter = this->start_positions.begin(); iter != start_positions.end(); ++iter) {
      if (this->start_positions.find(prefix_pos - length) != this->start_positions.end() && 
        this->start_positions.find(prefix_pos + acc) != this->start_positions.end() &&
          prefixes.find(prefix_pos - acc -length) != prefixes.end()) {// == *iter + length) {
        candidates.push_back(std::make_pair(prefix_pos, acc + length));
      }
      candidates.push_back(this->next_vertex->traverse(prefixes, prefix_pos, acc + length));
    
    //}
    std::sort(candidates.begin(), candidates.end(), compare_pairs);
    if (candidates.size() == 0)
      return std::make_pair(0, 0);
    return candidates.at(0);
  }
  
  std::pair<size_t, size_t> traverse() {
    std::pair<size_t,size_t> r1 = std::make_pair(0, 0);
    std::pair<size_t, size_t> r2 ;
    if (next_vertex != NULL) {
      for (std::set<size_t>::iterator iter = next_vertex->start_positions.begin(); iter != next_vertex->start_positions.end(); ++ iter) {
        r2 = next_vertex->traverse(next_vertex->start_positions, *iter, 0);
        if (r2.second > r1.second)
          r1 = r2;
      }
    }
    tree_vertex<Graph> * current_neighbour = next_vertex->neighbour.get();
    while (current_neighbour != NULL) {
      for (std::set<size_t>::iterator iter = current_neighbour->start_positions.begin(); iter != current_neighbour->start_positions.end(); ++ iter) {
        r2 = current_neighbour->traverse(current_neighbour->start_positions, *iter, 0);
        if (r2.second > r1.second)
          r1 = r2;
      }
      current_neighbour = current_neighbour->neighbour.get();
    }
    return r1;
  }
  void acceptVisitor(IVisitor * visitor) {
    return visitor->visit(this);
  }
  tree_vertex<Graph> * getNext() {
    return next_vertex.get();
  }
  tree_vertex<Graph> * getNeighbour() {
    return neighbour.get();
  }

  std::string to_s() {
    std::stringstream ss;
    std::set<size_t>::iterator iter = this->start_positions.begin();
    ss << "leaf[ (" << *iter; 
    ++iter;
    for (; iter != start_positions.end(); ++iter) {
      ss << "," << *iter;
    }
    ss <<") - " << this->length << "] ";
    //if (this->next_vertex != NULL)
    //  ss << next_vertex->to_s() << ";" << std::endl;
    //else ss<< "[]" <<std::endl;

    return ss.str();
  }
};


class SuffixTree {
public:
  bool compare(size_t s1, size_t l1, size_t s2, size_t l2) {
    if (l1 != l2)
      return false;
    for (size_t i = 0; i < l1; i++) {
      if (_str[s1 + i] != _str[s2 + i])
        return false;
    }
    return true;
  }
private:
  std::string _str;
  std::shared_ptr<tree_vertex<SuffixTree> > root;
public:
  tree_vertex<SuffixTree>* getRoot() {
    return root.get();
  }
  
  SuffixTree() {
    
  }
  
  void Simplify() {
    root->simplify();
  }
  std::pair<size_t, size_t> Traverse() {
    if (root == NULL)
      return std::make_pair(0, 0);
    return root->traverse();
    
  }
protected:
  void AddSuffix(std::string const& s, size_t start_index) {
    tree_vertex<SuffixTree>* current_vertex = root.get();
    for (size_t i = start_index; i < _str.length(); i++) {
      current_vertex = current_vertex->add_next(i);
    }

  }
public:
  void acceptVisitor(IVisitor * visitor) {
    return visitor->visit(this);
  }

  void addString(std::string const &str) {
    _str = std::string(base_str.c_str());
    root = std::shared_ptr<tree_vertex<SuffixTree> > (new tree_vertex<SuffixTree>(this, 0) );
    for (size_t i = 0; i < _str.length(); i++) {
      AddString(i);
    }
  } 
};
*/
template<class Graph>
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
    Graph * owner;

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