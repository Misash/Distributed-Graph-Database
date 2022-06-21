#include <iostream>
#include <string>
#include <sqlite3.h>

using namespace std;

int callback(void *NotUsed, int argc, char **argv, char **azColName){
    // Return successful
    return 0;
}

struct ORM {
    sqlite3* db;
    char *zErrMsg = 0;
    int rc = 0;

    ORM (string database) {
        rc = sqlite3_open(database.c_str(), &db);
        create_table("NODES", "id INTEGER PRIMARY KEY, NODE TEXT NOT NULL, ATTRIBUTE  TEXT NOT NULL, VALUE TEXT NOT NULL");      // attribute & value NAME´S CONFLICT
        create_table("EDGES", "NODE_FROM TEXT NOT NULL, RELATION  TEXT NOT NULL , NODE_TO TEXT NOT NULL");
        check();
    }

    bool check(string _message="Connection to database successful") {
        if (rc) {
            cout << "Can't open database: " << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
            return false;
        } else {
            cout << _message << endl;
            return true;
        }
    }

    bool create_table(string table, string columns) {
        string sql = "CREATE TABLE IF NOT EXISTS " + table + " (" + columns + ");";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        return check("Table creation successful") ? true : false;
    }


    bool insert_node(string node, string _attribute="", string _value="") {
        string sql = "INSERT INTO NODES ('NODE','ATTRIBUTE','VALUE') VALUES ('" + node + "' , '" + _attribute + "' , '" + _value + "');";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        return check("Node insertion successful") ? true : false;
    }

    bool insert_edge(string _nodeFrom, string _relation, string _nodeTo) {
        // verificar si el NODE_FROM existe en la tabla NODES
        // verificar si el NODE_TO existe en la tabla NODES
        string sql = "INSERT INTO EDGES ('NODE_FROM' , 'RELATION' ,'NODE_TO')  VALUES ('" + _nodeFrom + "' , '" + _relation + "' , '" + _nodeTo + "');";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        return check("Edge insertion successful") ? true : false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // CHECAR ESTO DESPUES VER VIDEO INICIAL

    bool select_node(string node, string _attribute, string _value) {
        string sql = "SELECT * FROM NODES WHERE NODE = '" + node + "' AND ATTRIBUTE = '" + _attribute + "' AND VALUE = '" + _value + "';";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        return check("Node selection successful") ? true : false;
    }

    bool select_node(string node, string _attribute) {
        string sql = "SELECT * FROM NODES WHERE NODE = '" + node + "' AND ATTRIBUTE = '" + _attribute + "';";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        return check("Node selection successful") ? true : false;
    }

    bool select_node(string node) {
        string sql = "SELECT * FROM NODES WHERE NODE = '" + node + "';";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        return check("Node selection successful") ? true : false;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool select_edge(string _nodeFrom, string _relation, string _nodeTo) {
        string sql = "SELECT * FROM EDGES WHERE NODE_FROM = '" + _nodeFrom + "' AND RELATION = '" + _relation + "' AND NODE_TO = '" + _nodeTo + "';";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        return check("Edge selection successful") ? true : false;
    }

    bool update_node(string node, string _attribute, string _value) {
        string sql = "UPDATE NODES SET VALUE = '" + _value + "' WHERE NODE = '" + node + "' AND ATTRIBUTE = '" + _attribute + "';";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        return check("Node update successful") ? true : false;
    }

    bool update_edge(string _nodeFrom, string _relation, string _nodeTo) {
        string sql = "UPDATE EDGES SET NODE_TO = '" + _nodeTo + "' WHERE NODE_FROM = '" + _nodeFrom + "' AND RELATION = '" + _relation + "';";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        return check("Edge update successful") ? true : false;
    }

    // especial case for delete node
    bool delete_node(string node, string _attribute, string _value) {
        string sql = "DELETE FROM NODES WHERE NODE = '" + node + "' AND ATTRIBUTE = '" + _attribute + "' AND VALUE = '" + _value + "';";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        return check("Node deletion successful") ? true : false;
    }

    bool delete_edge(string _nodeFrom, string _relation, string _nodeTo) {
        string sql = "DELETE FROM EDGES WHERE NODE_FROM = '" + _nodeFrom + "' AND RELATION = '" + _relation + "' AND NODE_TO = '" + _nodeTo + "';";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        return check("Edge deletion successful") ? true : false;
    }

    // delete only NODE but not EDGES and NODES with the same NAME
    bool drop_node(string node) {
        string sql = "DELETE FROM NODES WHERE NODE = '" + node + "';";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        return check("Node deletion successful") ? true : false;
    }

    // general delete function
    bool drop_table(string table_) {
        string sql = "DROP TABLE " + table_ + ";";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        return check("Table deletion successful") ? true : false;
    }

    ~ORM() {
        sqlite3_close(db);
    }
};