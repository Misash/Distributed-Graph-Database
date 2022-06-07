/*************************************************************************************/
/* @file    client_1.c                                                               */
/* @brief   This clients connects,                                                   */
/*          sends a text, reads what server and disconnects                          */
/*************************************************************************************/

// CPP
#include <iostream>
#include <string>
#include <bits/stdc++.h> //stl


#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>

/*Multi threading*/
#include <thread>


/*headers*/
#include "crud.h"


using namespace std;

//#define SERVER_ADDRESS "127.0.0.1" /* server IP */
//#define PORT 8000

//#define PORT 45012            /* port */
#define SERVER_ADDRESS "127.0.0.1" /* IP, only IPV4 support  */




inline string zeros(int num, int digits){
    string strNum = to_string(num);
    return string(digits - strNum.size(),'0') + strNum;
}

bool isDirectMsg(string msg){
    std::size_t found = msg.find(',');
    if (found!=std::string::npos) return 1;
    return 0;
}


void splitNickMsg(string txt ,string& nick , string& msg){
    string delimiter = ",";
    nick = txt.substr(0,txt.find(delimiter));
    nick.erase(remove(nick.begin(),nick.end(),' '),nick.end());
    msg = txt.substr(txt.find(delimiter)+1,txt.size());
}



void splitNickFile(string buff,string& nick,string& fileName){

    buff.erase(remove(buff.begin(),buff.end(),' '),buff.end());
    nick = buff.substr(buff.find("(")+1,buff.find(",") - buff.find("(") -1);
    fileName = buff.substr(buff.find(',')+1,buff.find(')') - buff.find(',')-1);

}


bool isSendFile(string buff){
    buff.erase(remove(buff.begin(),buff.end(),' '),buff.end());
    return buff.size()> 6 && buff[0] == 'F' && buff[1] == '(' && buff.find(',') !=std::string::npos && buff[buff.size()-1] == ')';
}


void READ(int sockfd){

    char buff_rx[1010];

    for(;;){

        bzero(buff_rx,1010); //clean buffer
        read(sockfd , buff_rx , 4); // read action and size

        if(buff_rx[0] == 'M' ){
            int size = atoi(&buff_rx[1]);
            bzero(buff_rx,1010); //clean buffer
            read(sockfd,buff_rx,size);
            cout<<"\n "<<buff_rx<<" \n";
//            printf("\n[ SERVER ] : %s\n" ,buff_rx);
        }
        else if(buff_rx[0] == 'F'){
            //read file Name
            string fileName;
            int size = atoi(&buff_rx[1]);
            bzero(buff_rx,1000); //clean buffer
            read(sockfd,buff_rx,size);
            fileName = buff_rx;

            //readNick
            string nick;
            bzero(buff_rx,1010); //clean buffer
            read(sockfd,buff_rx,2);
            size = atoi(&buff_rx[0]);
            bzero(buff_rx,1010); //clean buffer
            read(sockfd,buff_rx,size);
            nick = buff_rx;


            //readfile Size
            bzero(buff_rx,1000); //clean buffer
            read(sockfd,buff_rx,9);
            int file_size = atoi(&buff_rx[0]);


            //read bytes
            char *buffer;
            int SIZE = 1024;
            fileName = "./recibidos/" + fileName;

            ofstream file;
            file.open(fileName , ios::out);

            while ( file_size) {

                if (file_size < SIZE) {
                    SIZE = file_size;
                }

                buffer = new char[SIZE];

                if (read(sockfd, buffer, SIZE) > 0) {
                    file.write(buffer,SIZE);
                }
                file_size -= SIZE;
            }
            string msg = "\n[ " + nick + " ] <private>:  send you the file : (" + fileName +")\n" ;
            cout<<msg;

            file.close();

        }
        else if(buff_rx[0] == 'Q'){
            printf("\n server left the chat \n");
            break;
        }
        else if(buff_rx[0] == 'E'){
            int size = atoi(&buff_rx[1]);
            bzero(buff_rx,1010); //clean buffer
            read(sockfd,buff_rx,size);
            cout<<"\n "<<buff_rx<<" \n";
        }
        else if(buff_rx[0] == 'l'){
            string nick ;
            int nickSize ;
            int size = atoi(&buff_rx[1]);
            cout<<"\n User List: \n";
            while(size--) {
                //read nickname size
                bzero(buff_rx,1010); //clean buffer
                read(sockfd,buff_rx,2);
                int nickSize = atoi(&buff_rx[0]);

                //read nickname
                bzero(buff_rx,1010); //clean buffer
                read(sockfd,buff_rx,nickSize);
                nick = buff_rx;
                cout<<"\t-> "<<nick<<" \n";
            }
        }

    }

    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);

    cout << "\nRead_thread termino.\n";
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

enum actions { Q , CN , CE};

int scanner(string command, vector<string> &tokens){
    tokens = parser(command);
    cout<<endl;
    for(string s:tokens){
        cout<<s<<"  ";
    }


    if(command == "Q"  ){
        return Q;
    }
    if (tokens[0] == "C" && tokens.size() == 2){
        return CN;
    }
    if(tokens[0] == "C"  && tokens.size() >2){
        return CE;
    }
}



void WRITE(int sockfd){

    string command;
    string buff_tx;


    string nickname = "guest";


    buff_tx = "NN" + zeros(nickname.size(),3) + nickname;
    int n = write(sockfd, buff_tx.c_str(), buff_tx.size());
    if (n < 0) perror("ERROR writing to server");


    do{
        //get command
        cout<<"\nGDBU > ";
        getline(cin,command);

        int action;
        vector<string> tokens;
        action = scanner(command,tokens);


        if(action == Q){
            buff_tx += "000" ;
            write(sockfd,buff_tx.c_str(),4);
            break;
        }
        if(action == CN){
            buff_tx = "CN" + zeros(command.size(),3) + tokens[1];
            int n = write(sockfd, buff_tx.c_str(), buff_tx.size());
            if (n < 0) perror("ERROR writing to server");
        }
        if(action == CE){
            //
        }


    }while(true);

    shutdown(sockfd,SHUT_RDWR);
    close(sockfd);
    cout << "\nWrite_thread termino.\n";

}


int main()
{
    int sockfd;
    int PORT=0;
    struct sockaddr_in servaddr;
    cout<<"Ingrese Puerto: ";
    cin>>PORT;
    cin.ignore();
//    / Socket creation /
    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == -1)
    {
        printf("CLIENT: socket creation failed...\n");
        return -1;
    }
    else
    {
        printf("CLIENT: Socket successfully created..\n");
    }

    // clear socket
    memset(&servaddr, 0, sizeof(servaddr));

//    / assign IP, PORT /
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    servaddr.sin_port = htons(PORT);

//    / try to connect the client socket to server socket /
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        return -1;
    }

    printf("connected to the server..\n");



    thread th1(READ, sockfd);
    thread th2(WRITE, sockfd);

    th1.join();
    th2.join();

    return 0;
}



