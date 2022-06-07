#include <iostream>

/*standard symbols */
#include <unistd.h>

/* sockets */
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

/* strings / errors*/
#include <errno.h>
#include <stdio.h>
#include <string.h>

/* server parameters */
//#define SERV_PORT 8080             /* port */
#define SERV_HOST_ADDR "127.0.0.1" /* IP, only IPV4 support  */
         /* port */
//#define SERV_HOST_ADDR "5.253.235.219" /* IP, only IPV4 support  */
#define BUF_SIZE 100               /* Buffer rx, tx max size  */
#define BACKLOG 10                  /* Max. client pending connections  */

/*Multi threading*/
#include <thread>
#include<map>
#include <bits/stdc++.h>
#include <mutex>


using namespace std;

mutex m;

map<int , string> room;

inline string zeros(int num, int digit=3){
    string strNum = to_string(num);
    return string(digit - strNum.size(),'0') + strNum;
}

void broadcast(string msg){
    int n;
    char tamano[100];
    for (map<int,string>::iterator it=room.begin(); it!=room.end(); ++it){
        string a = "CN";
        sprintf(tamano,"%03d",msg.size());
        tamano[4]='\0';
        string t = tamano;
        string buffer = a + t + msg ;
        n = write(it->first, buffer.c_str(), buffer.size());
        cout << "Protocolo:" << buffer << endl;
    }
}



bool findNickname(map<int,string>::iterator &it , string nick){
    for ( it = room.begin(); it!=room.end(); ++it){
        if(it->second == nick) return 1;
    }
    return 0;
}


void sendMsgByNick(int connfd , string nick , string msg){
    int n;
    char tamano[100];
    map<int,string>::iterator it;

    if(findNickname(it,nick)){
        string a = "CN";
        sprintf(tamano,"%03d",msg.size());
        tamano[4]='\0';
        string t = tamano;
        string buffer = a + t + msg ;
        write(it->first, buffer.c_str(), buffer.size());
        cout << "Node Name: " << buffer << endl;
    }else{
        string errormsg = "\ninvalid node try again\n";
        string buffer = "E" + zeros(errormsg.size()) + errormsg ;
        write(connfd,buffer.c_str(),buffer.size());
        cout << "Error Protocolo: not found " << nick << endl;
    }
}




void sendUserList(int connfd){
    string buffer , action , ClientsSize , nick , nickSize;

    action = "CE";
    ClientsSize = zeros(room.size());

    buffer = action + ClientsSize ;

    for (auto it=room.begin(); it!=room.end(); ++it){
        nick = it->second;
        nickSize = zeros(nick.size(),2);
        buffer += nickSize + nick ;
    }
    int n = write(connfd, buffer.c_str(), buffer.size());
    cout << "Protocolo:" << buffer << endl;
}

void READ(int connfd)
{

    char buff_rx[1010];
    char nickname[1000];
    int n ;
    string message, buffer , action , ClientsSize;

    for(;;)
    {
        bzero(buff_rx,1010); //clean buffer
        read(connfd , buff_rx , 5); // read action and size

        
        if(buff_rx[0] == 'C' && buff_rx[1] =='N'){
            action = "CN";
            ClientsSize = zeros(room.size());
            buffer = action + ClientsSize ;
            cout << "Protocolo:" << buffer << endl;
        }
        /*if(buff_rx[0] == 'C' && buff_rx[1] =='N'){
            int size = atoi(&buff_rx[2]);
            bzero(buff_rx,111); //clean buffer
            read(connfd,buff_rx,size);
            message = "\n[" + room[connfd] + " ] : " + buff_rx + "\n" ;
            broadcast(message);
            cout<<message;
        }*/

        else if(buff_rx[0] == 'C' && buff_rx[1] == 'E'){
            sendUserList(connfd);
        }

    }

    // receptions and transmissions will be disallowed.
    shutdown(connfd , SHUT_RDWR);
    close(connfd);
    cout << "Read_thread termino.\n";
}


void WRITE(int connfd){

    do{
        string buff_tx;

        //send message
        getline(cin,buff_tx);

        if(buff_tx == "Q"  ){
            buff_tx += "000" ;
//            cout << "\nProtocolo:" << buff_tx <<endl;
            write(connfd,buff_tx.c_str(),4);
            break;
        }

        //write message
        buff_tx = "CN" + zeros(buff_tx.size())  + buff_tx ;
        int n = write(connfd, buff_tx.c_str(), buff_tx.size());
//        cout << "\nProtocolo:" << buff_tx <<endl;
        if(n < 0) perror("\nERROR writing to client");

    }while(true);

    // receptions will be disallowed
    shutdown(connfd, SHUT_RDWR);
    close(connfd);

}




int main() /* input arguments are not used */
{
    int sockfd, connfd; /* listening socket and connection socket file descriptors */
    unsigned int len;   /* length of client address */
    struct sockaddr_in servaddr, client;
    int SERV_PORT;
    cout<<"Inster Puerto: ";
    cin>>SERV_PORT;


    /* socket creation */
    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == -1)
    {
        fprintf(stderr, "[SERVER-error]: socket creation failed. %d: %s \n", errno, strerror(errno));
        return -1;
    }
    else
    {
        printf("[SERVER]: Socket successfully created..\n");
    }

    /* clear structure */
    memset(&servaddr, 0, sizeof(servaddr));

    /* assign IP, SERV_PORT, IPV4 */
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(SERV_PORT);

    /* Bind socket */
    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
    {
        fprintf(stderr, "[SERVER-error]: socket bind failed. %d: %s \n", errno, strerror(errno));
        return -1;
    }
    else
    {
        printf("[SERVER]: Socket successfully binded \n");
    }

    /* Listen */
    if ((listen(sockfd, 10)) != 0)
    {
        fprintf(stderr, "[SERVER-error]: socket listen failed. %d: %s \n", errno, strerror(errno));
        return -1;
    }
    else
    {
        printf("[SERVER]: Listening on SERV_PORT %d \n\n", ntohs(servaddr.sin_port));
    }


    /* Accept the data from incoming sockets in a iterative way */
    while (1)
    {
        connfd = accept(sockfd, NULL, NULL);
        if (connfd < 0)
        {
            fprintf(stderr, "[SERVER-error]: connection not accepted. %d: %s \n", errno, strerror(errno));
            return -1;
        }
        else
        {
            thread (READ, connfd).detach();
            thread (WRITE, connfd).detach();

        }

    }
    close(sockfd);

    return 0;
}
