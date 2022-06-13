#include <iostream>
#include <string>
#include <sqlite3.h>
#include <string>
#include<stdio.h>

using namespace std;



// Create a callback function
int callback(void *NotUsed, int argc, char **argv, char **azColName){
    // Return successful
    return 0;
}



struct crud{

    // Pointer to SQLite connection
    sqlite3* db;

    // Save any error messages
    char *zErrMsg = 0;

    // Save the connection result
    int rc = 0;

    string sql;

    crud(int id){
        create_db(to_string(id)+".db");
        //create table edges
        sql = "CREATE TABLE EDGES ( NODE_1 TEXT NOT NULL, RELATION  TEXT NOT NULL , NODE_2 TEXT NOT NULL);";
        sql3_exec();
        //create table nodes
        sql = "CREATE TABLE NODES ( NODE TEXT NOT NULL, ATTRIBUTE  TEXT NOT NULL , VALUE TEXT NOT NULL);";
        sql3_exec();
    }

    ~crud(){
        sqlite3_close(db);
    }

    void sql3_exec(){
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        check();
    }

    void check(){
        if (rc) {
            cout << sql+"\tERROR:  " << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
        } else {
            cout << sql + "\tSUCCESS!! " << endl;
        }
    }

    void create_db(string database){
        remove(database.c_str());
        rc = sqlite3_open(database.c_str(), &db);
    }

    void insertEdge(string node1 , string relation , string node2){
        sql = "INSERT INTO EDGES ('NODE_1' , 'RELATION' ,'NODE_2')  VALUES ('"+node1+"' , '"+relation+"' , '"+node2+"');";
        sql3_exec();
    }

    void insertNode(string node, string attribute ="", string value=""){
        sql = "INSERT INTO NODES ('NODE' , 'ATTRIBUTE','VALUE') VALUES ('"+node+"' , '"+attribute+"' , '"+value+"');";
        sql3_exec();
    }


};

//
//int main()
//{
//
//    crud bd(1);
//
//    bd.insertNode("julio","telefono","999");
//    bd.insertEdge("julio","profesor","redes");
//
//    return (0);
//}

