#ifndef DBSCHEMA_H_
#define DBSCHEMA_H_

#include "ORM2.hpp"

struct Condition {
  enum Operator {EQUAL, LESS, GREATER, LIKE};

  std::string key;
  std::string value;
  Operator op;
  bool is_or;
  
  Condition() {}
  Condition(const std::string& _key, const std::string& _value, char _op, char _lo)
      : key(_key), value(_value)  {
        is_or = _lo == '|' ? true : false;
        if (_op == '=') op = EQUAL;
        if (_op == '<') op = LESS;
        if (_op == '>') op = GREATER;
        if (_op == '%') op = LIKE;
      }
  std::string op_to_string() {
    if (op == EQUAL) return "=";
    if (op == LESS) return "<";
    if (op == GREATER) return ">";
    if (op == LIKE) return "%";
    return "~";
  }
  std::string is_or_to_string() {
    return is_or ? "|" : "&";
  }
};

struct Node {
  int id;
  std::string name;
  std::string attribute_name;
  std::string attribute_value;

  Node() {}
  explicit Node(const std::string& name) : name(name) {}
  explicit Node(int id, const std::string& name, const std::string& attribute_name , const std::string& attribute_value) : id(id), name(name), attribute_name(attribute_name), attribute_value(attribute_value) {}
};

struct Attribute {
  std::string node_name;
  std::string key;
  std::string value;

  Attribute() {}
  Attribute(const std::string& node_name, const std::string& key,
            const std::string& value) :
    node_name(node_name), key(key), value(value) {}
};

struct Edge {
  std::string node_from;
  std::string relation_name;
  std::string node_to;

  Edge() {}
  Edge(const std::string& _node_from, const std::string& _relation_name, const std::string& _node_to)
    : node_from(_node_from), relation_name(_relation_name), node_to(_node_to) {}
};

inline auto InitStorage(const std::string& path) {
  using namespace sqlite_orm;
  auto storage = make_storage(path,
                              make_table("nodes",
                                         make_column("id",
                                             &Node::id,
                                             primary_key()),
                                         make_column("name",
                                             &Node::name),
                                         make_column("attribute_name",
                                             &Node::attribute_name),
                                         make_column("attribute_value",
                                             &Node::attribute_value)),
                              make_table("edges",
                                         make_column("node_from",
                                             &Edge::node_from),
                                         make_column("relation_name",
                                             &Edge::relation_name),
                                         make_column("node_to",
                                             &Edge::node_to)));

  storage.sync_schema();

  return storage;
}

#endif  // DBSCHEMA_H_