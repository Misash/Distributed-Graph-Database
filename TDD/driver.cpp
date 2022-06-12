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
    default:
      break;
    }
  }
}