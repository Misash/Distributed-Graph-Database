#include <iostream>
#include <list>
#include <typeinfo>
#include <string>
#include "schema.hpp"
using namespace std;
using namespace sqlite_orm;

int main() {
  auto storage = InitStorage("db.sqlite");

  // Node node{-1,"Jorge","nacionalidad","peruana"};

  // auto insertedId = storage.insert(node);

  // auto allIds = storage.select(&Node::attribute_value, where(c(&Node::name) == "Jonh"));    
  // cout << "allIds count = " << allIds.size() << endl; //  allIds is std::vector<int>
  // for(auto &attribute_value : allIds) {
  //     cout << attribute_value << endl;
  // }

  
  // cout << "limited5 count = " << limited5.size() << endl;

  // auto allUsersList = storage.select(&Node::name, where(c(&Node::name) == "Jorge")); 

  // // cout << storage << endl;
  // string aux = "";
  // // auto node;

  // cout << storage.iterate<Node>().first() << endl;


  auto limited5 = storage.get_all<Node>(where(c(&Node::name) == "Jonh"),
                                      order_by(&Node::id));
  std::vector<std::pair<std::string, std::string>> result;

  for(auto &node : limited5) {
    result = storage.dump_okidoki(node);
    cout << result[2].second << " " << result[3].second << endl;
  }

  // tuple<int, double> f;
  // get<0>( f ) = 7;
  // get<1>( f ) = 3.14;
  // std::cout << get<0>(f) << "," << get<1>(f) << "\n";

  // for(auto &node : limited5) {
  //   cout << storage.dump(node) << endl;
  //     // cout << storage.dump(node) << endl;
  // }
} 