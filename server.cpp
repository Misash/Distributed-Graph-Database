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
#include <math.h>


using namespace std;

mutex m;

map<int , string> room;


// RSA //

int modulo(int a,int n){
    int r( a - n*(a/n) );
    if(r<0)
        r=a-n*((a/n)-1);
    return r;
}

int euclides(int a ,int b){
   int r ;
   r = modulo(a,b);
   while(r > 0){
       if (r > (b/2) )
           r = b - r;
       a = b;
       b = r;
       r = modulo(a,b);
   }
   return b ;
}

int euclidesExtendido(int a ,int b){
   int res;
   int r1{a} , r2 {b} , s1{1} , s2{0} , t1{0} ,t2{1} ;
   int q , r , s , t;
 
   while(r2 > 0 ){
       q = r1/r2;
 
       r = r1 - q*r2 ;
       r1 = r2 ; r2 = r ;
 
       s = s1 - q*s2;
       s1 = s2 ; s2 = s ;
 
       t = t1 - q*t2 ;
       t1 = t2 ; t2 = t;
   }
   res = s1 ;
   return res;
}

int inversa(int x , int y ){
    int num ;
    num = euclides(x,y);
    if( num == 1){
        int aux;
        aux = euclidesExtendido(x,y);
        if (aux  < 0 )
            aux = modulo(aux , y );
        return  aux;
    }
    else{
        num = 0; 
        cout << "NO TIENE INVERSA" << endl;
        return num;
    }
}

int modPow(int a ,int n,int m){

    int exp (1);
    int x = modulo(a,m);

    while(n>0){
        if(modulo(n, int(2)) == 1)
            exp = modulo(exp*x,m);

        x = modulo(x*x,m);
        n/=2;
    }

    return exp;
}

int gcd(int a, int b) {
   int t;
   while(1) {
      t= a%b;
      if(t==0)
      return b;
      a = b;
      b= t;
   }
}

class RSA{
    private:
    int d;
    public:
    int n ;
    int e;
    RSA();
    RSA(int , int);
    void generarClaves();
    int cifrado(int);
    int descifrado(int);
};

RSA::RSA(){
    generarClaves();
}

RSA::RSA(int n,int e){
    this -> n = n ;
    this -> e = e ;
}

void RSA::generarClaves(){
    int p ;
    int q;
    p = 13;
    q = 11;
    n = p*q;
    int fi_n = (p-1)*(q-1);
    e = 7 ;
    d = inversa(e,fi_n);
}

int RSA::cifrado(int mensaje){
    int tem;
    tem = modPow(mensaje, e, n);
    return tem;
}

int RSA::descifrado(int mensaje){
    int tem;
    tem = modPow(mensaje , d, n);
    return tem;
}

//   RSA  //


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
    cout<<"\n\n read \n\n";
    char buff_rx[1010];
    char nickname[1000];
    int n ;
    string message, buffer , action , ClientsSize;

    RSA receptor;
    RSA emisor(receptor.n,receptor.e);

    for(;;)
    {
        bzero(buff_rx,1010); //clean buffer
        read(connfd , buff_rx , 5); // read action and size
        //new guest node
        if(buff_rx[0] == 'N' && buff_rx[1] =='N')
        {

            int size = atoi(&buff_rx[2]);
            bzero(nickname,1000); //clean buffer
            read(connfd,nickname,size);
            room[connfd] = nickname;    //add client
            message = "\n " + room[connfd] + " node created \n";
            cout<<message;
//            broadcast(message);
        }
        else if(buff_rx[0] == 'C' && buff_rx[1] =='N')
        {

            int size = atoi(&buff_rx[2]);
            bzero(buff_rx,1010);
            read(connfd,buff_rx,size);

            int nodeSelected = 0;
            room[nodeSelected] = buff_rx;

            message = "\n " + room[nodeSelected] + " storage node created \n";
//            broadcast(message);
            cout<<message;
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

