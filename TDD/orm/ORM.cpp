#include <iostream>
#include <string>
#include "orm.hpp"

using namespace std;

int main () {
    ORM orm("test.db");
    int option;
    while(1){
        cout << "1. Insert node" << endl;
        cout << "2. Insert edge" << endl;
        cout << "3. Select node" << endl;
        cout << "4. Drop Node" << endl;
        cin >> option;
        switch(option){
            case 1: {
                string node, attribute, value;
                cout << "Node: ";
                cin >> node;
                cout << "Attribute: ";
                cin >> attribute;
                cout << "Value: ";
                cin >> value;
                orm.insert_node(node, attribute, value);
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
                orm.insert_edge(nodeFrom, relation, nodeTo);
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
                orm.select_node(node_, attribute_, value_);
                break;
            }
            case 4: {
                string node_;
                cout << "Node: ";
                cin >> node_;
                orm.drop_node(node_);
                break;
            }
            default: {
                cout << "Invalid option" << endl;
                break; 
            }           
        }
    }

    return 0;
}
