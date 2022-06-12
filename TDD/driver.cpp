#include <iostream>
#include <string>
#include "./hash/rsa.hpp"
#include "./orm/orm.hpp"

using namespace std;

int main () {
  int number_pcs = 7;
  RSA receptor(number_pcs);

  for (int i = 0; i < number_pcs; i++) {
    string name = "pc" + to_string(i + 1);
    ORM orm(name + ".db");
  }

  int option;

  int pc_number;
  string pc_db_pivot;

  while(true){
    cout << "1. Insert node" << endl;
    cout << "2. Insert edge" << endl;
    cout << "3. Select node" << endl;
    cout << "4. Drop Node" << endl;
    cin >> option;
    switch (option) {
      case 1: {
        string node, attribute, value;
        cout << "Node: ";
        cin >> node;
        cout << "Attribute: ";
        cin >> attribute;
        cout << "Value: ";
        cin >> value;
        pc_number = receptor.hashCipher(node);
        pc_db_pivot = "pc" + to_string(pc_number) + ".db";
        ORM orm(pc_db_pivot);
        orm.insert_node(node, attribute, value) ? cout << "Node inserted in " << pc_db_pivot << endl : cout << "Node not inserted" << endl;;     
        break;
      }
      case 2: {
        string nodeFrom, relation, nodeTo;
        cout << "NodeFrom: ";
        cin >> nodeFrom;
        cout << "Relation: ";
        cin >> relation;
        cout << "NodeTo: ";
        cin >> nodeTo;
        pc_number = receptor.hashCipher(nodeFrom);
        pc_db_pivot = "pc" + to_string(pc_number) + ".db";
        ORM orm(pc_db_pivot);
        orm.insert_edge(nodeFrom, relation, nodeTo) ? cout << "Edge inserted in " << pc_db_pivot << endl : cout << "Edge not inserted" << endl;
        break;
      }
      case 3: {
        string node_, attribute_, value_;
        cout << "Node: ";
        cin >> node_;
        cout << "Attribute: ";
        cin >> attribute_;
        cout << "Value: ";
        cin >> value_;
        pc_number = receptor.hashCipher(node_);
        pc_db_pivot = "pc" + to_string(pc_number) + ".db";
        ORM orm(pc_db_pivot);
        orm.select_node(node_, attribute_, value_) ? cout << "Node selected in " << pc_db_pivot << endl : cout << "Node not selected" << endl;
        break;
      }
      case 4: {
        string node_;
        cout << "Node: ";
        cin >> node_;
        pc_number = receptor.hashCipher(node_);
        pc_db_pivot = "pc" + to_string(pc_number) + ".db";
        ORM orm(pc_db_pivot);
        orm.drop_node(node_) ? cout << "Node dropped in " << pc_db_pivot << endl : cout << "Node not dropped" << endl;
        break;
      }
    default:
      break;
    }
  }
}