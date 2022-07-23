#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "SQLite.h"
#include <algorithm>
#include <iostream>
#include <thread>
#include <sstream>
#include <string>
#include <vector>
#include "ORM.hpp"

#include "schema.hpp"

using namespace std;
using namespace sqlite_orm;


#define MSERVER "127.0.0.1"
// #define MSERVER "67.205.177.103"

int BUFFER = 1000;
//int MYHOST=45001;
int MYHOST=5000;



string int_to_string(int number, int digits)
{
    string ans = "";
    while (number)
    {
        ans.push_back('0' + (number % 10));//saca el ultimo digito
        number /= 10;
    }
    reverse(ans.begin(), ans.end());
    return string(digits - ans.size(), '0') + ans;
}


int string_int(string s)
{
    stringstream geek(s);
    int x = 0;
    geek >> x;
    return x;
}

int stringToASCII(string s)
{
    int res = 0;
    for (int i = 0; i < s.size(); i++)
    {
        res += (int)s[i];
    }
    return res;
}

void inserciones(string campos, string name_db)
{
    ORM db(name_db);
    // db.init();
    // db.insert_data("'relaciones'", campos);

}

void delete_(string campo1, string valor1, string campo2, string valor2, string campo3,string valor3, string name_db)
{
    SQlite db(name_db);
    db.init();

    db.delete2_db("'relaciones'",campo1,valor1,campo2,valor2,campo3,valor3);
}



string id = "";

void read_r(int socket){

    char b_envio[BUFFER];
    char recv_data[BUFFER];
    int  rc, number_repo = 0;
    string name_db;


    string buff = "NA";
    int n = write(socket,buff.c_str(), buff.size());
    if (n < 0) perror("ERROR writing to server");


    while(1) {
        n = read(socket, recv_data, BUFFER);
        recv_data[n]='\0';
        string temp = recv_data;
//        cout << "server:  " <<temp << endl;


        if(recv_data[0] == 'N' && recv_data[1] == 'A'){
            cout << "server:  " <<temp << endl;


            string buffer(recv_data, n);
            

            //NA00254;
            int size_id = string_int(buffer.substr(2, 3));

            id = buffer.substr(5);
            name_db = "node-" + id + ".db";
            cout << "DB created : " << name_db << endl;



            ORM db(name_db);

            // este no es mi ORM
            // db.init();
        }


        else if (recv_data[0] == 'C') {            
            if (recv_data[1] == 'N') {
                cout << "server:  " <<temp << "DB NAME" << recv_data << endl;
//                cout<<"entro!!";
                //n = read(socket, recv_data,tam);
//                cout << recv_data << '\n';

                string buffer(recv_data, n);
                string b_temp(recv_data, n);
                string nodo;
                
                //cn005julio;
                int size_nodo = string_int(buffer.substr(2, 3)); //005 to int
                buffer = buffer.substr(5);//new buffer es omite el CN005 en el caso de julio
                nodo = buffer;//julio

                string campos = "'" + nodo +"','',''";
                // cout << "ID TEST: " << nodo << endl;

                name_db = "node-" + id + ".db";
                auto storage = InitStorage(name_db);
                Node node{nodo};
                auto insertedId = storage.insert(node);

                // Node node{-1,"Jorge","nacionalidad","peruana"};

                // ORM db_connection(name_db);

                // if (db_connection.check_if_exist("nodes", "name", nodo) == true)
                // {
                //     cout << "Nodo ya existe" << endl;
                // }
                // else
                // {
                //     cout << "entre a insertar : ->" << name_db<<endl;
                //     db_connection.insert_node(nodo);
                // }

                

                // CN005julio
                //insert query
//                inserciones(campos,name_db );

                bzero(recv_data, BUFFER);

            } else if (recv_data[1] == 'R') {
                cout << "server:  " <<temp << endl;
                //"CR005ujlio006recibe002rc"
                string buffer(recv_data, n);
                string b_temp(recv_data, n);


                int size_nodo1 = string_int(buffer.substr(2, 3));//005
                string nodo1 = buffer.substr(5, size_nodo1);//julio
                cout << "nodo1: " << nodo1 << endl;

                b_temp = b_temp.substr(5 + size_nodo1); //006recibe002rc
                int size_relacion = string_int(b_temp.substr(1, 3));//006
                string relacion = b_temp.substr(3, size_relacion);//recibe
                cout << "relacion: " << relacion << '\n';

                b_temp = b_temp.substr(3 + size_relacion);//002rc
                int size_nodo2 = string_int(b_temp.substr(1, 3));
                string nodo2 = b_temp.substr(3);
                cout << "nodo2: " << nodo2 << '\n';

                string campos = "'" + nodo1 +"',"+"'"+relacion+"'," + "'" + nodo2 + "'";
                cout << campos << '\n';

                name_db = "node-" + id + ".db";
                auto storage = InitStorage(name_db);
                Edge edge{nodo1, relacion, nodo2};
                auto insertedId = storage.insert(edge);


                // ORM db_connection(name_db);

                // if (db_connection.check_if_exist("nodes", "name", nodo1) == true && db_connection.check_if_exist("nodes", "name", nodo2) == true)
                // {
                //     cout << "entre a insertar : relacion" <<endl;
                //     db_connection.insert_edge(nodo1, relacion, nodo2);
                // }
                // else
                // {
                //     cout << "uno de los nodos no existe" << endl;
                // }

                // name_db = "node-" + id + ".db";
                // ORM db_connection(name_db);

                // db_connection.insert_edge(nodo1, relacion, nodo2);

//                inserciones(campos,name_db );

                buffer.clear();
                b_temp.clear();
            }
            else if(recv_data[1] == 'A'){
                string buffer(recv_data, n);
                string b_temp(recv_data, n);

                cout << buffer << '\n';

                int size_name_node = string_int(buffer.substr(2,3));
                string node2= buffer.substr(5, size_name_node);                
                b_temp = b_temp.substr(5 + size_name_node);

                int size_attr_name = string_int(b_temp.substr(1,3));
                string attr_name = b_temp.substr(3, size_attr_name);

                int size_attr_value = string_int(b_temp.substr(3 + size_attr_name,3));
                string attr_value;
                attr_value = b_temp.substr(6 + size_attr_name, size_attr_value);
                

                // cout<<"node: "<<node<<endl;
                // cout << "attr_name: " << attr_name << '\n';
                // cout << "attr_value: " << attr_value << '\n';

                name_db = "node-" + id + ".db";
                auto storage = InitStorage(name_db);
                Node node{-1,node2,attr_name,attr_value};
                auto insertedId = storage.insert(node);

                // name_db = "node-" + id + ".db";
                // ORM db_connection(name_db);

                // if (db_connection.check_if_exist("nodes", "name", node) == true) {
                //     cout << "entre a insertar : -> " << name_db<<endl;
                //     db_connection.insert_node_attribute(node, attr_name, attr_value);
                // }
                // else
                // {
                //     cout << "No se encontro el : ->"<<endl;
                // }


                buffer.clear();
                b_temp.clear();
            }
        }
        //READ
        else if(recv_data[0] == 'R'){

            string buffer(recv_data, n);
            string b_temp(recv_data, n);
            string nodo;

            if(recv_data[1] == 'N'){
                //RN
                cout << "server :  " <<temp << endl;
                //insert query here!!!!
                //obtener las relaciones y enviar nodo1 nodo2

                // vector<string> nodos1 ={"i","i","i","o","o","f","f","c","c","c","c"};
                // vector<string> nodos2 ={"h","e","o","i","p","c","b","a","g","f","h"};

                // buffer = "E" + int_to_string(nodos1.size(),6);

                // for (int i = 0; i < nodos1.size(); ++i) {
                //     buffer += int_to_string(nodos1[i].size(),3) + nodos1[i];
                //     buffer += int_to_string(nodos2[i].size(),3) + nodos2[i];
                // }

                name_db = "node-" + id + ".db";
                auto storage = InitStorage(name_db);
                auto data_query = storage.get_all<Edge>();
                std::vector<std::pair<std::string, std::string>> result;
            
                buffer.clear();
                int total = 0;

                for(auto &node : data_query){
                    result = storage.dump_okidoki(node);
//                    cout << result[0].second << " si estoy imprimiendo " << result[2].second << endl;
                    buffer += int_to_string( result[0].second.size(),3) +  result[0].second;
                    buffer += int_to_string( result[2].second.size(),3) +  result[2].second;
                     total++;
                }

                buffer = "E" + int_to_string(total,6) + buffer;

//                cout << "result: "<<result.size() <<" total:"<<total<< endl;
                // A total nodo1 nodo2 ..
                cout<<">>  "<<buffer<<endl;
                 write(socket,buffer.c_str(),buffer.size());
            }
            if(recv_data[1] == 'A'){
                // READ ATRIBUTO
                //RA005julio
                cout << "server:  " <<temp << endl;
                int size_nodo = string_int(buffer.substr(2, 3));
                nodo  = buffer.substr(5);

                // string attributes = "A003004name005julio003dni00885964125004name005aaron";
                name_db = "node-" + id + ".db";
                auto storage = InitStorage(name_db);
                auto data_query = storage.get_all<Node>(where(c(&Node::name) == nodo),
                                      order_by(&Node::id));
                std::vector<std::pair<std::string, std::string>> result;
            
                buffer.clear();
                int total = -1;
                for(auto &node : data_query) {
                    result = storage.dump_okidoki(node);
                    cout <<total<<": "<< result[2].second << " " << result[3].second << endl;
                    if(total != -1){
                        buffer += int_to_string( result[2].second.size(),3) +  result[2].second;
                        buffer += int_to_string( result[3].second.size(),3) +  result[3].second;
                    }
                    total++;
                }

                //  A002 2 RA 005dario 000000 006 ciudad 004 lima

                buffer = "A" + int_to_string(total,3) + buffer;

                cout<<"total: "<<total<<endl;
                cout<<">>  "<<buffer<<endl;
                write(socket,buffer.c_str(),buffer.size());

                buffer.clear();
                b_temp.clear();

            }
            else if(recv_data[1] == 'I'){
                // READ NODE INFO
                // Numero count;
                cout << "server:  " <<temp << endl;
                int size_nodo = string_int(buffer.substr(2, 3)); //005 to int
                nodo  = buffer.substr(5, size_nodo);

                name_db = "node-" + id + ".db";
                auto storage = InitStorage(name_db);               

                // numero de atributos
                cout << "nombre nodo: " << nodo << endl;
                int usersAtt = storage.count<Node>(where(c(&Node::name) == nodo));
                cout << "atributos count = " << usersAtt << endl;

                // numero de relaciones
                int edgesCount = storage.count<Edge>(where(c(&Edge::node_from) == nodo));
                cout << "edges count = " << edgesCount << endl;


                
                //insert query here!!!!
                //send attributes to server
                // I sizeattributes sizerelacionles idNode
                // # DE RELACIONES
                // # DE ATRIBUTOS
                // ID NODO
                string info = "I" + int_to_string(usersAtt,3) + int_to_string(edgesCount,3)  + id;  // 2 ATRIBUTOS   12 -> 12  RELACIONES   ID   --> NODO 1
                cout<<">>  "<<info<<endl;
                 write(socket,info.c_str(),info.size());
            }
        }
        else if(recv_data[0] == 'U'){
            //UPDATE 
            string buffer(recv_data, n);
            string b_temp(recv_data, n);
            string nodo;

            if(recv_data[1] == 'N'){
                //UPDATE NODE
                //UN005julio006julius
                cout << "server:  " <<temp << endl;
                int size_nodo1 = string_int(buffer.substr(2, 3));
                string nodo1  = buffer.substr(5,size_nodo1);
                buffer = buffer.substr(5+size_nodo1);
                int size_nodo2 = string_int(buffer.substr(0, 3));
                string nodo2  = buffer.substr(3);

                name_db = "node-" + id + ".db";
                ORM db_connection(name_db);

                if (db_connection.check_if_exist("nodes", "name", nodo1) == true) {
                    cout << "Eliminando todas las relaciones del nodo " << nodo1 <<endl;
                    db_connection.update_node(nodo1, nodo2);
                    db_connection.update_edge(nodo1, nodo2);
                    db_connection.update_edge_to(nodo1, nodo2);
                }

                //insert query here!!!!
                cout<<nodo1<<"-"<<nodo2<<endl;
            }
            else if(recv_data[1] == 'R'){
                // JULIO EDUCADOR DE REDES 
                // JULIO PROFESOR DE REDES
                //UPDATE RELATION
                cout << "server:  " <<temp << endl;
                //UR005julio008profesor005redes008educador
                string values[4];
                buffer = buffer.substr(2);

                for (int i = 0; i < 4 ; ++i) {
                    int size = size = string_int(buffer.substr(0, 3));
                    values[i] = buffer.substr(3,size);
                    buffer = buffer.substr(3+size);
                }

                name_db = "node-" + id + ".db";
                ORM db_connection(name_db);
                if (db_connection.check_if_exist("nodes", "name", values[0]) == true) {
                    // cout << "Eliminando todas las relaciones del nodo " << nodo1 <<endl;
                    db_connection.update_relation(values[0], values[1], values[3], values[2]);
                }

                //insert query here!!!!
                // cout<<"nodo1: "<<values[0]
                //     <<" relation :"<<values[1]
                //     <<" nodo2 :"<<values[2]
                //     <<" newRelation :"<<values[3]<<endl;

            }
            else if(recv_data[1] == 'A'){
                // JULIO EMAIL EMAIL@.COM
                // JULIO EMAIL TILI@EMAIL.COM
                //UPDATE ATRIBUTO 
                //UA005julio003age00255
                cout << "server:  " <<temp << endl;
                string values[3];
                buffer = buffer.substr(2);

                for (int i = 0; i < 3 ; ++i) {
                    int size = size = string_int(buffer.substr(0, 3));
                    values[i] = buffer.substr(3,size);
                    buffer = buffer.substr(3+size);
                }

                
                name_db = "node-" + id + ".db";
                ORM db_connection(name_db);
                if (db_connection.check_if_exist("nodes", "name", values[0]) == true) {
                    // cout << "Eliminando todas las relaciones del nodo " << nodo1 <<endl;
                    db_connection.update_attribute(values[0], values[1], values[2]);
                }

                //insert query here!!!!
                cout<<"nodo: "<<values[0]
                    <<" atributo :"<<values[1]
                    <<" newValue :"<<values[2]<<endl;

            }
        }
        else if ( recv_data[0] == 'D')
        {
            //DELETE
            //DR004data004mata004sata
            if(recv_data[1] == 'N')
            {
                //DELETE NODE
                cout << "server:  " <<temp << endl;
                string buffer(recv_data, n);
                string b_temp(recv_data, n);
                string nodo;

                int size_nodo = string_int(buffer.substr(2, 3)); //005 to int
                buffer  = buffer.substr(5);//new buffer es omite el CN005 en el caso de julio
                nodo = buffer;

                // cout << "nodo a eliminar ->" << nodo << endl;

                name_db = "node-" + id + ".db";
                ORM db_connection(name_db);

                if (db_connection.check_if_exist("nodes", "name", nodo) == true) {
                    cout << "Eliminando nodo " << nodo <<endl;
                    db_connection.delete_node(nodo);
                    db_connection.delete_edge_from(nodo);
                    db_connection.delete_edge_to(nodo);
                }
                else
                {
                    cout << "No se encontro el : nodo ->"<< nodo <<endl;
                }

//                delete_("nodo_inicial", nodo, "relacion","","nodo_final","",name_db);

                buffer.clear();
            }
            else if(recv_data[1] == 'R'){
                // DELETE RELATION
                cout << "server:  " <<temp << endl;
                string buffer(recv_data, n);
                string b_temp(recv_data, n);


                int size_nodo1 = string_int(buffer.substr(2, 3));//005
                string nodo1 = buffer.substr(5, size_nodo1);//julio
                cout << "nodo1: " << nodo1 << endl;

                b_temp = b_temp.substr(5 + size_nodo1); //006recibe002rc
                int size_relacion = string_int(b_temp.substr(1, 3));//006
                string relacion = b_temp.substr(3, size_relacion);//recibe
                cout << "relacion: " << relacion << '\n';

                b_temp = b_temp.substr(3 + size_relacion);//002rc
                int size_nodo2 = string_int(b_temp.substr(1, 3));
                string nodo2 = b_temp.substr(3);
                cout << "nodo2: " << nodo2 << '\n';

                name_db = "node-" + id + ".db";
                ORM db_connection(name_db);

                if (db_connection.check_if_exist("nodes", "name", nodo1) == true) {
                    cout << "Eliminando todas las relaciones del nodo " << nodo1 <<endl;
                    db_connection.delete_edge(nodo1);
                }
                else
                {
                    cout << "No se encontro el : nodo ->"<< nodo1 <<endl;
                }

//                delete_("nodo_inicial", nodo1, "relacion",relacion,"nodo_final",nodo2,name_db);

                buffer.clear();
                b_temp.clear();
            }
            else
            {
               
                cout << "server:  " <<temp << endl;
                //DA008eduarado006numero006933399
                string buffer(recv_data, BUFFER);
                string b_temp(recv_data, BUFFER);


                int size_nodo1 = string_int(buffer.substr(2, 3));
                string nodo1 = buffer.substr(5, size_nodo1);
                cout << "nodo1: " << nodo1 << endl;

                b_temp = buffer.substr(5 + size_nodo1);//006numero006 93399
                int size_relacion = string_int(b_temp.substr(1, 3));//006 chapa size
                string atributo = b_temp.substr(3, size_relacion);//numero
                cout << "atributo: " << atributo << '\n';

                // b_temp = b_temp.substr(3+size_relacion);//006 93399
                // int size_valor = string_int(b_temp.substr(1,3));
                // string valor = b_temp.substr(3,size_valor);
                // cout << "valor: " << valor << '\n';

                name_db = "node-" + id + ".db";
                ORM db_connection(name_db);

                db_connection.delete_node_with_attributes(nodo1, atributo);

                // if (db_connection.check_if_exist("nodes", "name", nodo1) == true) {
                //     cout << "Eliminando nodo " << nodo1 <<endl;
                //     db_connection.delete_node_with_attributes(nodo1, atributo, valor);
                // }
                // else
                // {
                //     cout << "No se encontro el : nodo ->"<< nodo1 <<endl;
                // }

                // name_db = "node-" + id + ".db";
                // auto storage = InitStorage(name_db);
                // auto data_query = storage.get_all<Node>(where(c(&Node::name) == nodo1 and c(&Node::attribute_name) == atributo and c(&Node::attribute_value) == valor));
                // std::vector<std::pair<std::string, std::string>> result;

                // for(auto &node : data_query) {
                    
                //     result = storage.dump_okidoki(node);
                //     cout << "nodo: "<< result[1].second << endl;
                //     cout << "atributo: "<< result[2].second << endl;
                //     cout << "valor: "<< result[3].second << endl;

                //     storage.remove<Node>(result[0].second);
                // }
                
                // write(socket,buff.c_str(), buff.size());
                buffer.clear();
                b_temp.clear();
            } //atributos
        }
    }
}


int main(int argc, char** argv)
{
    cout<<"Ingrese Puerto: ";
    cin>>MYHOST;
    cin.ignore();

    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//creo el socket
    int n;
    char buffer[1000];

//    cout << argv[1]<<endl;
//    cout<< "Socket: "<<SocketFD<<endl;

    if (-1 == SocketFD)
    {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(MYHOST);

    Res = inet_pton(AF_INET, MSERVER, &stSockAddr.sin_addr);


    if (0 > Res)
    {
        perror("error: first parameter is not a valid address family");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    else if (0 == Res)
    {
        perror("char string (second parameter does not contain valid ipaddress");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    //conecto el socket creado con una IP adress
    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    printf("REPOSITORIO [%s : %hd]\n", inet_ntoa(stSockAddr.sin_addr), ntohs(stSockAddr.sin_port));


    std::thread(read_r,SocketFD).detach(); //socket descriptor de la funcion connect
    //std::thread(write_r,SocketFD).detach();

    while(true){};
}
