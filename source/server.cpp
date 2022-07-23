#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <thread>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "graph.h"
#include <mutex>

mutex m;
// #define SERVER_ADDR "67.205.177.103"
#define SERVER_ADDR "127.0.0.1"



int BUFFER=1000;
int MYHOST=5000;
//int MYHOST=5001;

using namespace  std;



struct Repo{
    int id , socket ;
    string ip;
    bool flag = 0;
};


//key = id , value = Repo
map<int,Repo> repositories;
map<int,Repo> clientes;

Graph<string> g;


int idNA = 1;
int idNC = 1;


int hashFunction(int x, int SIZE)
{
    return (x % SIZE) + 1;
}

struct tipoConexion{
    int socket;
    bool myflag;
};



vector<tipoConexion> clientsAddr;

struct mystruct
{
    int mydouble;
    sockaddr_in to_send[5];
};


bool Repos_process_finished(){
    bool end=1;
    for (auto it=repositories.begin(); it!=repositories.end(); ++it){
        end = end && repositories[it->first].flag ;
    }
    return end;
}

void clear_process_flags(){
    for (auto it=repositories.begin(); it!=repositories.end(); ++it){
        repositories[it->first].flag = 0;
    }
}


void broadcast(string buffer){
    for (auto it=repositories.begin(); it!=repositories.end(); ++it){
        write(repositories[it->first].socket, buffer.c_str(), buffer.size());
        cout << "broadcast : " << buffer << endl;
    }
}


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

int stringToINT(string s)
{
    int res = 0;
    for (int i = 0; i < s.size(); i++)
    {
        res += (int)s[i];
    }
    return res;
}


int chooseREPO(string nodo)
{
    if (repositories.size()==0)
    {
        cout<<"ERROR, sin repositorios";
        return 0;
    }

    int repoID = hashFunction(int(nodo[0]), repositories.size());
    return repoID;
}


int chooseCLIENT(string client)
{
    if (repositories.size()==0)
    {
        cout<<"ERROR, sin clientes";
        return 0;
    }

    int repoID = hashFunction(int(client[0]), clientes.size());
    return repoID;
}

void send_Client_Path(){
    string buffer;
    g.print();
    cout<<endl;

    deque<string> paths;
    g.DFS(g.root,g.level,paths);

    buffer = "R" + int_to_string(paths.size(),6) ;

    for (auto it:paths)
    {
        buffer += int_to_string(it.size(),3) + it;
    }
    cout<<">> send to client "<<buffer<<endl;
    write(clientes[1].socket, buffer.c_str(),buffer.size());

}


//void read_thread(int socket, int n_repos, struct sockaddr_in)
void read_thread(int socket, string ip, int port){

    char recv_data[BUFFER];
    int n, rc, number_repo = 0;
    socklen_t addr_len;
    addr_len = sizeof(struct sockaddr);
    string c_id; // Store the position of the IP address in clientsAddr



    while(1){
        bzero(recv_data, BUFFER);

        n = read(socket,recv_data,BUFFER);

        if (n < 0)
        {
            perror("ERROR reading from socket");
        }


        //PROTOCOLO CONEXION SERVIDOR NODOALMACENAMIENTO
        else if (recv_data[0]=='N'){

              if(recv_data[1] == 'A')
              {
                  string buffer(recv_data, n);

                  Repo myrep;
                  myrep.id= idNA;
                  myrep.socket=socket;

                  cout<<"NODE [ "<< idNA << " ] created  with socket "<<myrep.socket<<" \n";
                  buffer = "NA" + int_to_string(to_string(idNA).size(),3) + to_string(idNA);
                  repositories[idNA++] = myrep;

                  cout<<">> "<<ip<<","<<port<<" : "<<buffer<<endl;
                  write(socket,buffer.c_str(),buffer.size());

              }
              else if(recv_data[1] == 'C'){

                  string buffer(recv_data, n);

                  Repo myrep;
                  myrep.id= idNC;
                  myrep.socket=socket;

                  cout<<" CLIENT [ "<< idNC << " ] created with socket "<<socket<<"\n";
                  clientes[idNC++] = myrep;

                  cout<<">> "<<ip<<","<<port<<" : "<<buffer<<endl;
              }
        }

        //PROTOCOLO CREATE
        else if ( recv_data[0] == 'C'  )
        {
            if(recv_data[1] == 'N')
            {
                string buffer(recv_data, n);
                string b_temp(recv_data, n);//copia buffer
                string nodo = buffer.substr(5);//quita CN000

//                cout << "client: " << buffer << endl;
                cout<<"<<"<<ip<<","<<port<<" : "<<buffer<<endl;
                int size_nodo = string_int(buffer.substr(2, 3)); //005 to int

                cout<< "letra: "<< nodo[0] << " ,rep_size: " << repositories.size() << " ,isalpha(nodo[0]): " << isalpha(nodo[0]) << endl;

                int res= chooseREPO(nodo);

                cout<<"res: "<<res<<endl ;
                cout<<"repositories[" << res << "].socket: "<< repositories[res].socket <<endl;
                int n = write(repositories[res].socket, b_temp.c_str(),b_temp.size());

                buffer.clear();
                b_temp.clear();

                //CLEAR
                g.clear();
                clear_process_flags();
            }

            else if(recv_data[1] == 'R'){
                //"CR005ujlio006recibe002rc"
                string buffer(recv_data, n);
                string b_temp(recv_data, n);

                //cout << "recibido->client: " << buffer << endl;
                //cout << "recibido->b_temp: " << b_temp << endl;

                cout<<"<<"<<ip<<","<<port<<" : "<<buffer<<endl;

                int size_nodo1 = string_int(buffer.substr(2,3));//005
                string nodo1= buffer.substr(5, size_nodo1);//julio
                cout<<"nodo1: "<<nodo1<<endl;


                int res=chooseREPO(nodo1);

                int n = write(repositories[res].socket, b_temp.c_str(),b_temp.size());

                buffer.clear();
                b_temp.clear();
                //CLEAR
                g.clear();
                clear_process_flags();
            }
            else if(recv_data[1] == 'A'){
                //"CA005ujlio006edad002quince"
                string buffer(recv_data, n);
                string b_temp(recv_data, n);

                //cout << "recibido->client: " << buffer << endl;
                //cout << "recibido->b_temp: " << b_temp << endl;

                cout<<"<<"<<ip<<","<<port<<" : "<<buffer<<endl;

                int size_nodo1 = string_int(buffer.substr(2,3));//005
                string nodo1= buffer.substr(5, size_nodo1);//julio
                cout<<"nodo1: "<<nodo1<<endl;


                int res=chooseREPO(nodo1);


                int n = write(repositories[res].socket, b_temp.c_str(),b_temp.size());

                buffer.clear();
                b_temp.clear();
            }
        }

        // READ
        else if(recv_data[0] == 'R'){

            string buffer(recv_data, n);
            string b_temp(recv_data, n);
            string nodo , level;
            cout<<"<<"<<ip<<","<<port<<" : "<<buffer<<endl;

            if(recv_data[1] == 'N'){
                int size_nodo = string_int(buffer.substr(2, 3));
                nodo  = buffer.substr(5,size_nodo);
                buffer = buffer.substr(5+size_nodo);
                int size_nivel = string_int(buffer.substr(0, 3));
                level  = buffer.substr(3,size_nivel);
                g.root = nodo;
                g.level = stoi(level);
                cout<<" nodo: "<<nodo<<" nivel: "<<level<<endl;

                if(Repos_process_finished())
                {
                    send_Client_Path();
                }else{
                    broadcast("RN");
                }
            }
            if(recv_data[1] == 'A'){
                int size_nodo = string_int(buffer.substr(2, 3));
                nodo  = buffer.substr(5);
                int id = chooseREPO(nodo);
                write(repositories[id].socket, b_temp.c_str(),b_temp.size());
            }
            else if(recv_data[1] == 'I'){
                int size_nodo = string_int(buffer.substr(2, 3));
                nodo  = buffer.substr(5);
                int id = chooseREPO(nodo);
                b_temp +=  to_string(id);
                write(repositories[id].socket, b_temp.c_str(),b_temp.size());
            }
        }

        //SEND TO CLIENT
        else if(recv_data[0] == 'A'){
            string buffer(recv_data, n);
            string b_temp(recv_data, n);
            string nodo;
            cout << "recibido->node: " << buffer << endl;
            cout<<">> send to client "<<b_temp<<endl;
            write(clientes[1].socket, b_temp.c_str(),b_temp.size());
        }
        else if(recv_data[0] == 'I'){
            string buffer(recv_data, n);
            string b_temp(recv_data, n);
            string nodo;
            cout << "recibido->node: " << buffer << endl;
            //I 003 012 1
            cout<<">> send to client "<<b_temp<<endl;
            write(clientes[1].socket, b_temp.c_str(),b_temp.size());
        }
        else if(recv_data[0] == 'E'){
            string buffer(recv_data, n);
            string b_temp(recv_data, n);
            string nodo1 , nodo2;
            cout << "recibido->node: " << buffer << endl;

            int totalRelations =  string_int(buffer.substr(1, 6));
            buffer = buffer.substr(7);

            // insert edges
            for (int i = 0; i < totalRelations; ++i) {
                int size= string_int(buffer.substr(0, 3));
                nodo1 = buffer.substr(3,size);
                buffer = buffer.substr(3+size);
                size= string_int(buffer.substr(0, 3));
                nodo2 = buffer.substr(3,size);
                buffer = buffer.substr(3+size);
                g.addEdge(nodo1,nodo2);
            }

            int id = chooseREPO(nodo1);

            // repo finished sending data
            repositories[id].flag = true;


            // did all repos finished sending data ?
            if(Repos_process_finished())
            {
                send_Client_Path();
            }
        }

        //UPDATE

        else if(recv_data[0] == 'U'){

            string buffer(recv_data, n);
            string b_temp(recv_data, n);
            string nodo;
            cout<<"<<"<<ip<<","<<port<<" : "<<buffer<<endl;

            if(recv_data[1] == 'N'){
//                UN005julio006julius
                broadcast(b_temp);
                //CLEAR
                g.clear();
                clear_process_flags();
            }
            else if(recv_data[1] == 'R'){
                //UR005julio008educador005redes008profesor
                broadcast(b_temp);
                //CLEAR
                g.clear();
                clear_process_flags();
            }
            else if(recv_data[1] == 'A'){
                //UA005julio003age00255
                broadcast(b_temp);
            }
        }


        //DELETE
        else if ( recv_data[0] == 'D')
        {
            //DR004data004mata004sata
            if(recv_data[1] == 'N')
            {
                string buffer(recv_data, n);
                string b_temp(recv_data, n);
                string nodo;

                cout<<"<<"<<ip<<","<<port<<" : "<<buffer<<endl;

                int size_nodo = string_int(buffer.substr(2, 3)); //005 to int
                buffer  = buffer.substr(5);//new buffer es omite el CN005 en el caso de julio
                nodo = buffer;

                int res=chooseREPO(nodo);

                int n = write(repositories[res].socket, b_temp.c_str(),b_temp.size());

                buffer.clear();
                //CLEAR
                g.clear();
                clear_process_flags();
            }
            else if(recv_data[1] == 'R'){
                string buffer(recv_data, n);
                string b_temp(recv_data, n);

                //cout << "recibido->client: " << buffer << endl;
                cout<<"<<"<<ip<<","<<port<<" : "<<buffer<<endl;

                int size_nodo1 = string_int(buffer.substr(2, 3));//005
                string nodo1 = buffer.substr(5, size_nodo1);//julio
                cout << "nodo1: " << nodo1 << endl;

                int res= chooseREPO(nodo1);

                int n = write(repositories[res].socket, b_temp.c_str(),b_temp.size());

                buffer.clear();
                b_temp.clear();
                //CLEAR
                g.clear();
                clear_process_flags();
            }
            else
            {
                //DA004DATA004SATA005cinco
                string buffer(recv_data, n);
                string b_temp(recv_data, n);

                //cout << "recibido->client: " << buffer << endl;
                cout<<"<<"<<ip<<","<<port<<" : "<<buffer<<endl;

                int size_nodo1 = string_int(buffer.substr(2, 3));//005
                string nodo1 = buffer.substr(5, size_nodo1);//julio
                cout << "nodo1: " << nodo1 << endl;

                int res= chooseREPO(nodo1);
                int n = write(repositories[res].socket, b_temp.c_str(),b_temp.size());

                buffer.clear();
                b_temp.clear();
            } //atributos
        }
    }



}


int main(void)
{
    cout<<"Ingrese Puerto: ";
    cin>>MYHOST;
    cin.ignore();


    struct sockaddr_in server_addr;
    int SocketServer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    char buffer[256];
    int n;

    if (-1 == SocketServer)
    {
        perror("can not create socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(MYHOST);
//    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);


    if (-1 == bind(SocketServer, (const struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)))
    {
        perror("error bind failed");
        close(SocketServer);
        exit(EXIT_FAILURE);
    }

    if (-1 == listen(SocketServer, 10))
    {
        perror("error listen failed");
        close(SocketServer);
        exit(EXIT_FAILURE);
    }

    printf("[%s] [%s : %hd]\n", "Servidor: ", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

    char myIP[16];
    socklen_t len = sizeof(server_addr);
    getsockname(SocketServer, (struct sockaddr *) &server_addr, &len);
    inet_ntop(AF_INET, &server_addr.sin_addr, myIP, sizeof(myIP));
    string ip = myIP;

    //printf("Local ip address: %s\n", myIP);

    for (;;) // muchos  clientes
    {
        int newSocketCli = accept(SocketServer, NULL, NULL);
        if (0 > newSocketCli)
        {
            perror("error accept failed");
            close(SocketServer);
            exit(EXIT_FAILURE);
        }
        //cout<< newSocketCli<<endl;
        tipoConexion temp;//guardamos socket client
        temp.socket=newSocketCli;
        clientsAddr.push_back(temp);

        //std::thread(read_thread, SocketServer, 5, server_addr).detach();
        std::thread(read_thread, newSocketCli, ip, ntohs(server_addr.sin_port) ).detach();
    }

    close(SocketServer);
}
