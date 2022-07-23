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
#include <sstream>
#include <string>
#include <vector>


using namespace std;
#define MSERVER "127.0.0.1"
// #define MSERVER "67.205.177.103"


int BUFFER = 1000;
int MYHOST=5000;
//int MYHOST=5001;

string int_to_string(int number, int digits)
{
    string ans = "";
    while (number)
    {
        ans.push_back('0' + (number % 10));
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


vector<string> parser(string s){
    vector<string> tokens;
    std::string delimiter = " ";
    s+=delimiter;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    return tokens;
}



enum actions { Q , CN , CR, CA,DN, DR, DA,UN,UR,UA,RA,RI,RN};

int scanner(string command, vector<string> &tokens){
    tokens = parser(command);

    if(command == "Q"  ){
        return Q;
    }
    if (tokens[0] == "CN" && tokens.size() == 2){
        return CN;
    }
    if(tokens[0] == "CR"  && tokens.size() ==4){
        return CR;
    }
    if(tokens[0] == "CA" && tokens.size()==4 )
    {
        return CA;
    }
    if(tokens[0] == "DN" && tokens.size()==2 )
    {
        return DN;
    }
    if(tokens[0] == "DR" && tokens.size()==4 )
    {
        return DR;
    }
    if(tokens[0] == "DA" && tokens.size()==3 )
    {
        return DA;
    }
    if(tokens[0] == "RN" && tokens.size() == 3){
        return RN;
    }
    if(tokens[0] == "RA" && tokens.size()==2 )
    {
        return RA;
    }
    if(tokens[0] == "RI" && tokens.size()==2 )
    {
        return RI;
    }
    if(tokens[0] == "UN" && tokens.size()==3){
        return UN;
    }
    if(tokens[0] == "UR" && tokens.size()==5){
        return UR;
    }
    if(tokens[0] == "UA" && tokens.size()==4){
        return UA;
    }
}






void WRITE(int socket_cliente, string ip, int host){

    string command;
    string buff_tx;

    string buff = "NC";
    int n = write(socket_cliente,buff.c_str(), buff.size());
    if (n < 0) perror("ERROR writing to server");

    do{
        //get command
        cout<<"\nGDBU >";
        getline(cin,command);

        int action;
        vector<string> tokens;
        action = scanner(command,tokens);//que tipo de accion

        if(action == Q){
            buff_tx += "000" ;
            write(socket_cliente,buff_tx.c_str(),4);
            break;
        }

        //CREATE
        if(action == CN){

            buff_tx = "CN" + int_to_string(tokens[1].size(),3) + tokens[1];
            cout<<">>"<<ip<<","<<host<<" : "<<buff_tx<<endl;
            int n = write(socket_cliente, buff_tx.c_str(), buff_tx.size());
            if (n < 0) perror("ERROR writing to server");
        }
        if(action == CR){
            buff_tx = "CR" + int_to_string(tokens[1].size(),3)+tokens[1]
                    +int_to_string(tokens[2].size(),3)+tokens[2]
                    +int_to_string(tokens[3].size(),3)+tokens[3];
            cout<<">>"<<ip<<","<<host<<" : "<<buff_tx<<endl;
            int n = write(socket_cliente, buff_tx.c_str(), buff_tx.size());
            if (n < 0) perror("ERROR writing to server");
        }
        if(action == CA){
            buff_tx = "CA" + int_to_string(tokens[1].size(),3)+tokens[1]
                      +int_to_string(tokens[2].size(),3)+tokens[2]
                      +int_to_string(tokens[3].size(),3)+tokens[3];
            cout<<">>"<<ip<<","<<host<<" : "<<buff_tx<<endl;
            int n = write(socket_cliente, buff_tx.c_str(), buff_tx.size());
            if (n < 0) perror("ERROR writing to server");
        }

        //READ
        if(action == RN){
            buff_tx = "RN" + int_to_string(tokens[1].size(),3) + tokens[1]
                    + int_to_string(tokens[2].size(),3) + tokens[2];
            cout<<">>"<<ip<<","<<host<<" : "<<buff_tx<<endl;
            int n = write(socket_cliente, buff_tx.c_str(), buff_tx.size());
            if (n < 0) perror("ERROR writing to server");
        }
        if(action == RA){
            buff_tx = "RA" + int_to_string(tokens[1].size(),3) + tokens[1];
            cout<<">>"<<ip<<","<<host<<" : "<<buff_tx<<endl;
            int n = write(socket_cliente, buff_tx.c_str(), buff_tx.size());
            if (n < 0) perror("ERROR writing to server");
        }
        if(action == RI){
            buff_tx = "RI" + int_to_string(tokens[1].size(),3) + tokens[1];
            cout<<">>"<<ip<<","<<host<<" : "<<buff_tx<<endl;
            int n = write(socket_cliente, buff_tx.c_str(), buff_tx.size());
            if (n < 0) perror("ERROR writing to server");
        }


        //UPDATE
        if(action == UN){
            buff_tx = "UN" + int_to_string(tokens[1].size(),3) + tokens[1] + int_to_string(tokens[2].size(),3) + tokens[2];
            cout<<">>"<<ip<<","<<host<<" : "<<buff_tx<<endl;
            int n = write(socket_cliente, buff_tx.c_str(), buff_tx.size());
            if (n < 0) perror("ERROR writing to server");
        }
        if(action == UR){
            buff_tx = "UR" ;
            for (int i = 1; i < 5 ; ++i) {
                buff_tx += int_to_string(tokens[i].size(),3) + tokens[i];
            }
            cout<<">>"<<ip<<","<<host<<" : "<<buff_tx<<endl;
            int n = write(socket_cliente, buff_tx.c_str(), buff_tx.size());
            if (n < 0) perror("ERROR writing to server");
        }
        if(action == UA){
            buff_tx = "UA";
            for (int i = 1; i < 4 ; ++i) {
                buff_tx += int_to_string(tokens[i].size(),3) + tokens[i];
            }
            cout<<">>"<<ip<<","<<host<<" : "<<buff_tx<<endl;
            int n = write(socket_cliente, buff_tx.c_str(), buff_tx.size());
            if (n < 0) perror("ERROR writing to server");
        }

        //DELETE
        if(action == DN)
        {
            buff_tx = "DN" + int_to_string(tokens[1].size(),3) + tokens[1];
            cout<<">>"<<ip<<","<<host<<" : "<<buff_tx<<endl;
            int n = write(socket_cliente, buff_tx.c_str(), buff_tx.size());
            if (n < 0) perror("ERROR writing to server");
        }
        if(action == DR)
        {
            buff_tx = "DR" + int_to_string(tokens[1].size(),3)+tokens[1]
                      +int_to_string(tokens[2].size(),3)+tokens[2]
                      +int_to_string(tokens[3].size(),3)+tokens[3];
            cout<<">>"<<ip<<","<<host<<" : "<<buff_tx<<endl;
            int n = write(socket_cliente, buff_tx.c_str(), buff_tx.size());
            if (n < 0) perror("ERROR writing to server");
        }
        if(action == DA)
        {
            buff_tx = "DA" + int_to_string(tokens[1].size(),3)+tokens[1]
                      +int_to_string(tokens[2].size(),3)+tokens[2];
                    //   +int_to_string(tokens[3].size(),3)+tokens[3];
            cout<<">>"<<ip<<","<<host<<" : "<<buff_tx<<endl;
            int n = write(socket_cliente, buff_tx.c_str(), buff_tx.size());
            if (n < 0) perror("ERROR writing to server");
        }


    }while(true);

    shutdown(socket_cliente,SHUT_RDWR);
    close(socket_cliente);
    cout << "\nWrite_thread termino.\n";

}


void READ(int socket, string ip, int host){

    char b_envio[BUFFER];
    char recv_data[BUFFER];
    int  rc, number_repo = 0;
    int n;

    while(1) {
        n = read(socket, recv_data, BUFFER);
        recv_data[n] = '\0';
        string temp = recv_data;
//        cout << "recibido:  " << temp << endl;

        if (recv_data[0] == 'A') {
            cout << "server:  " << temp << endl;
            "A002 004name 005julio 004name005aaron";

            string buffer(recv_data, n);

            int totalAttributes =  string_int(buffer.substr(1, 3));
            buffer = buffer.substr(4);

            for (int i = 0; i < totalAttributes; ++i) {
                int size= string_int(buffer.substr(0, 3));
                string attribute = buffer.substr(3,size);
                buffer = buffer.substr(3+size);
                size= string_int(buffer.substr(0, 3));
                string value = buffer.substr(3,size);
                buffer = buffer.substr(3+size);
                cout<<"\t"<<attribute<<": "<<value<<endl;
            }

        }
        else if(recv_data[0] == 'I'){
            //I 003 012 1
            cout << "server:  " << temp << endl;
            string buffer(recv_data, n);

            int numAttributes =  string_int(buffer.substr(1, 3));
            int numRelations =  string_int(buffer.substr(4, 3));
            int idNode = string_int(buffer.substr(7));

            cout<<"INFO:\n";
            cout<<"\t- # Atributos: "<<numAttributes<<"\n"
                <<"\t- # Relaciones: "<<numRelations<<"\n"
                <<"\t- Id Node: "<<idNode<<"\n";
        }
        else if(recv_data[0] == 'R'){
            //R 000003 015 -> a -> c -> g 015 -> a -> c -> f 015 -> a -> c -> h
            cout << "server:  " << temp << endl;
            string buffer(recv_data, n);


            int totalRelations =  string_int(buffer.substr(1, 6));
            buffer = buffer.substr(7);

            cout<<"RELACIONES POR NIVEL :\n";
            for (int i = 0; i < totalRelations; ++i) {
                int size= string_int(buffer.substr(0, 3));
                string path = buffer.substr(3,size);
                buffer = buffer.substr(3+size);
                cout<<"\t"<<path<<endl;
            }

        }

    }

}







int main()
{
    cout<<"Ingrese Puerto: ";
    cin>>MYHOST;
    cin.ignore();

    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//creo el socket
    int n;
    char buffer[1000];

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

     stSockAddr.sin_addr;

    char str[INET_ADDRSTRLEN];
    socklen_t len = sizeof(stSockAddr);
    int err = getpeername(SocketFD, (struct sockaddr*)&stSockAddr, &len);
    inet_ntop(AF_INET, &stSockAddr.sin_addr, str, sizeof(str));
    string ip = str;

    //printf("Peer IP address: %s\n", str);
    //cout << ntohs(stSockAddr.sin_port)<<endl;
    //printf("[%s] [%s : %hd]\n", "Servidor: ", inet_ntoa(stSockAddr.sin_addr), ntohs(stSockAddr.sin_port));
    printf("[%s] [%s : %hd]\n", "Peer IP address: ", str, ntohs(stSockAddr.sin_port));

    //std::thread(read_c,SocketFD).detach(); //socket descriptor de la funcion connect
    std::thread(WRITE,SocketFD, ip, ntohs(stSockAddr.sin_port)).detach();
    std::thread(READ,SocketFD, ip, ntohs(stSockAddr.sin_port)).detach();

    //ya que es un thread detachable solo espera, no habra un join
    while(true){};

    return 0;
}
