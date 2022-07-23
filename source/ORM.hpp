#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>

using namespace std;

std::vector<std::string> allArgs;

int callback(void *NotUsed, int argc, char **argv, char **azColName){
    // Return successful
    return 0;
}



static int callback2(void* data, int argc, char** argv, char** azColName){
    int i;
    std::vector<std::string> allArgsAux;
    allArgsAux.clear();
    // fprintf(stderr, "%s: ", (const char*)data);
  
    for (i = 0; i < argc; i++) {
      allArgsAux.push_back(argv[i] ? argv[i] : "");
        // printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
  
    allArgs = allArgsAux;
    // printf("\n");
    return 0;
}

struct ORM {
  sqlite3* db;
  char *zErrMsg = 0;
  int rc = 0;

  int sqlite3_table_column_metadata(
    sqlite3 *db,                /* Connection handle */
    const char *zDbName,        /* Database name or NULL */
    const char *zTableName,     /* Table name */
    const char *zColumnName,    /* Column name */
    char const **pzDataType,    /* OUTPUT: Declared data type */
    char const **pzCollSeq,     /* OUTPUT: Collation sequence name */
    int *pNotNull,              /* OUTPUT: True if NOT NULL constraint exists */
    int *pPrimaryKey,           /* OUTPUT: True if column part of PK */
    int *pAutoinc               /* OUTPUT: True if column is auto-increment */
  );

  ORM(string database) {
    rc = sqlite3_open(database.c_str(), &db);
    create_table("nodes", "name VARCHAR NOT NULL, attribute_name VARCHAR, attribute_value VARCHAR");      // attribute & value NAME´S CONFLICT
    create_table("edges", "node_from VARCHAR NOT NULL, relation_name VARCHAR NOT NULL , node_to VARCHAR NOT NULL");
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

  bool check_if_exist(string table, string column, string value) {
    allArgs.clear();
    string data("CALLBACK FUNCTION");
    string sql = "SELECT * FROM " + table + " WHERE " + column + " = '" + value + "'";
    rc = sqlite3_exec(db, sql.c_str(), callback2, (void*)data.c_str(), NULL);
    // check("Node selection successful");
    // for (int i = 0; i < allArgs.size(); i++) {
    //   cout << allArgs[i] << endl;      
    // }

    if (allArgs.size() > 0) {
      return true;
    } else {
      return false;
    }
  }

  bool create_table(string table, string columns) {
    string sql = "CREATE TABLE IF NOT EXISTS " + table + " (" + columns + ");";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    return check("Table creation successful") ? true : false;
  }

  bool insert_node(string node_name) {
    string sql = "INSERT INTO nodes(name) VALUES ('" + node_name + "');";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    return check("Node insertion successful") ? true : false;
  }

  bool insert_node_attribute(string name, string attribute_name, string attribute_value) {
    string sql = "INSERT INTO nodes (name, attribute_name, attribute_value) VALUES ('" + name + "', '" + attribute_name + "', '" + attribute_value + "');";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    return check("Node insertion successful") ? true : false;
  }

  std::vector<std::string> select_node(string name) {
    allArgs.clear();
    std::vector<std::string> res;
    res = allArgs;
    string data("CALLBACK FUNCTION");
    string sql = "SELECT * FROM nodes WHERE name = '" + name + "';";
    //rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    rc = sqlite3_exec(db, sql.c_str(), callback2, (void*)data.c_str(), NULL);
    // for (int i = 0; i < allArgs.size(); i++) {
    //   cout << allArgs[i] << endl;
    // }
    check("Node selection successful");
    return res;
  }

  std::vector<std::string> select_edge(string node) {
    std::vector<std::string> res;
    res = allArgs;
    string data("CALLBACK FUNCTION");
    string sql = "SELECT * FROM edges WHERE node_from = '" + node + "';";
    //rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    rc = sqlite3_exec(db, sql.c_str(), callback2, (void*)data.c_str(), NULL);
    for (int i = 0; i < allArgs.size(); i++) {
      cout << allArgs[i] << endl;
    }
    check("Edge selection successful");
    return res;
  }

  std::vector<std::string> select_edge(string node_from, string relation_name, string node_to) {
    std::vector<std::string> res;
    res = allArgs;
    string data("CALLBACK FUNCTION");
    string sql = "SELECT * FROM edges WHERE node_from = '" + node_from + "' AND relation_name = '" + relation_name + "' AND node_to = '" + node_to + "';";
    //rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    rc = sqlite3_exec(db, sql.c_str(), callback2, (void*)data.c_str(), NULL);
    for (int i = 0; i < allArgs.size(); i++) {
      cout << allArgs[i] << endl;
    }
    check("Edge selection successful");
    return res;
  }

  bool insert_edge(string node_from, string relation_name, string node_to) {
    string sql = "INSERT INTO edges (node_from, relation_name, node_to) VALUES ('" + node_from + "', '" + relation_name + "', '" + node_to + "');";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    return check("Edge insertion successful") ? true : false;
  }

  bool delete_node(string name) {
    string sql = "DELETE FROM nodes WHERE name = '" + name + "';";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    return check("Node deletion successful") ? true : false;
  }

  bool delete_edge_from(string node_from ) {
    string sql = "DELETE FROM edges WHERE node_from = '" + node_from + "';";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    return check("Edge deletion successful") ? true : false;
  }

  bool delete_edge_to(string node_to) {
    string sql = "DELETE FROM edges WHERE node_to = '" + node_to + "';";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    return check("Edge deletion successful") ? true : false;
  }

  bool update_node(string name, string _new_name) {
    string sql = "UPDATE nodes SET name = '" + _new_name + "' WHERE name = '" + name + "';";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    return check("Node update successful") ? true : false;
  }

  bool update_edge_to(string node_from, string new_node_from) {
    string sql = "UPDATE edges SET node_from = '" + new_node_from + "' WHERE node_from = '" + node_from + "';";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    return check("Edge update successful") ? true : false;
  }

  bool update_edge(string node_to, string _new_node_to) {
    string sql = "UPDATE edges SET node_to = '" + _new_node_to + "' WHERE node_to = '" + node_to + "';";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    return check("Edge update successful") ? true : false;
  }

  bool update_relation(string _node_from, string _relation, string _new_relation, string _node_to){
    string sql = "UPDATE edges SET relation_name = '" + _new_relation + "' WHERE node_from = '" + _node_from + "' AND relation_name = '" + _relation + "' AND node_to = '" + _node_to + "';";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    return check("Edge update successful") ? true : false;
  }

  bool update_attribute(string _node, string _attribute_name, string _new_attribute_value){
    string sql = "UPDATE nodes SET attribute_value = '" + _new_attribute_value + "' WHERE name = '" + _node + "' AND attribute_name = '" + _attribute_name + "';";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    return check("Node update successful") ? true : false;
  }

  bool delete_node_with_attributes(string name, string name_sttr) {
    // string sql = "DELETE FROM nodes WHERE name = '" + name + "' AND attribute_name = '" + name_sttr + "' AND attribute_value = '" + value_attr + "';";
    string sql = "DELETE FROM nodes WHERE name = '" + name + "' AND attribute_name = '" + name_sttr + "';";
    cout << sql << endl;
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    return check("Node deletion successful") ? true : false;
  }

  bool delete_edge(string node_from) {
    string sql = "DELETE FROM edges WHERE node_from = '" + node_from + "';";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    return check("Edge deletion successful") ? true : false;
  }

  bool delete_edge(string node_from, string relation_name, string node_to) {
    string sql = "DELETE FROM edges WHERE node_from = '" + node_from + "' AND relation_name = '" + relation_name + "' AND node_to = '" + node_to + "';";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    return check("Edge deletion successful") ? true : false;
  }

  ~ORM() {
    sqlite3_close(db);
  }
};