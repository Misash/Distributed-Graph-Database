#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include <utility>
#include <iostream>
#include <string>

#define TAM_BUFFER 1024

using namespace std;
struct paquete
{
        string n_sec, n_flujo, sec_flujo, last, size, data, checksum;
        string res;

       int n_sec_, n_flujo_, sec_flujo_, last_, size_, checksum_;
       string acc;

        paquete(int _n_sec, int _n_flujo, int _sec_flujo, int _last,string _acc, int _size, string _data, int _checksum)
        {       
                this->n_sec_ = _n_sec;
                this->n_flujo_ = _n_flujo;
                this->sec_flujo_ = _sec_flujo;
                this->last_ = _last;
                this->acc=_acc;
                this->size_ = _size;
                this->checksum_ = _checksum;

                this->n_sec = complete(_n_sec, 5);
                this->n_flujo = complete(_n_flujo, 3);
                this->sec_flujo = complete(_sec_flujo, 2);
                this->last = to_string(_last);
                this->size = complete(_size, 3);
                this->data = _data;
                this->checksum = complete(_checksum, 2);

                this->res = n_sec + n_flujo + sec_flujo + last + acc + size + data + checksum;
        }

         string complete(int n, int c)
        {
                string _res;
                _res = to_string(n);
                while (_res.size() != c)
                        _res = "0" + _res;
                return _res;
        }
};

class rdt{
    public:
    int n_sec = 1; //cont global paquetes
    int n_flujo = 1; //cont global bloques
    int sec_flujos; //iterador en el flujo

    vector<int> convert(string cont){
        vector<int> aux;
        for(int i=0;i<cont.size();i++){
            const char* ptr= &cont[i];
            aux.push_back(atoi(ptr));
        }
        return aux;
    }

    int checksum(string contenido){
        int total = 0;
        for (int i = 0; i < contenido.size(); i++){
            if(contenido[i]!='\0'){
                total = total + (int)contenido[i];
            }
        }
        return total % 24;
    }
    pair<vector<string>,vector<int>> leer(int sockfd, sockaddr_in cliaddr){
        bool flag = 1;
        vector<string> deposito;
        vector<int> ack;
        while(flag){
            char buffer[TAM_BUFFER];
            int len, n;
            len = sizeof(cliaddr);
            
            n = recvfrom(sockfd, (char *)buffer, TAM_BUFFER,
                                                    MSG_WAITALL, ( struct sockaddr *) &cliaddr,
                                                    (socklen_t*)&len);
            buffer[n] = '\0';
            string parte = buffer;
            deposito.push_back( parte.substr(15,n-15-2));
            int check = checksum(buffer);
            int acks = 0;
            if(check==stoi(parte.substr(15 + (n-15-2),2))){
                acks = stoi(parte.substr(0,5));
            }
            ack.push_back(acks);
            int termino = stoi(parte.substr(10,1));
            if(termino) flag = 0;
        }
        return make_pair(deposito,ack);
    }
    void mandar(int sockfd,sockaddr_in cliaddr, string mensaje){
        char *buff = &mensaje[0];
        sendto(sockfd,buff,strlen(buff),MSG_CONFIRM,(const struct sockaddr *) &cliaddr,sizeof(cliaddr));
    }
    vector<paquete> hacerPaquete(string contenido){

        // DIVIDIRLO en PAQUETES de 512
        // LE COLOCAS RDT A CADA PAQUETE
        // LOS COLOCAS  A UN VECTOR
        int n_pack = contenido.size()/494;
        n_pack = contenido.size() % 494 == 0 ? n_pack : n_pack + 1;
        vector<paquete> envio;
        for(int i = 0; i < n_pack;i++){
            string parte = contenido.substr(494*i,494);
            envio.push_back(paquete(n_sec,n_flujo,i,i==n_pack-1?1:0,"A",parte.size(),parte,checksum(parte)));
            n_sec++;
        }
        n_flujo++;

        // 0000100102holaA12sfasdas
        return envio;
    }

    string hacerAck(vector<int> a){
        string dumb="";
        for(int i=0; i < a.size();i++){
            int ana=0;
            if(i+1==a[i]){
                ana = a[i];
            }
            dumb+= to_string(ana);
        }
        return dumb;
    }
    
    bool valido(vector<int> ack){
        for(int i=0; i < ack.size();i++){
            if(ack[i]==0)
                return false;
        }
        return true;
    }

    vector<string> corregir(vector<string> msg,vector<string> msg_unico,vector<int> indices){
        for(int i = 0; i < indices.size();i++){
            vector<string>::iterator a= msg_unico.begin()+indices[i]-1;
            msg.insert(a,msg_unico[i]);
        }
        return msg;
    }

    vector<int> actualizarAck(vector<int> ack, vector<int> actualizar){
        for(int i=0; i <actualizar.size();i++){
            vector<int>::iterator a = actualizar.begin()+actualizar[i]-1;
            ack.insert(a,actualizar[i]);
        }
        return ack;
    }
    string desempaquetar(int sockfd, sockaddr_in cliaddr){

        //BUCLE ETERNO:
            //LEE PAQUETES
            //HACE ACKS
            //MANDA ACK
            //VERIFICA QUE TODO EL MENSAJE ESTE COMPLETO
            //SI ESTA COMPLETO:
                //SALE
            //SINO:
                //LEE PAQUETE
                //CORRIJE EL MENSAJE
                //ACTUALIZA ACK
            //VERIFICA QUE TODO EL MENSAJE ESTE COMPLETO
            //SI ESTA COMPLETO:
                //SALE

        string mensaje="";
        pair<vector<string>,vector<int>> m_pair;
        while(true){
            m_pair = leer(sockfd,cliaddr);
            string ack = hacerAck(m_pair.second);
            vector<int> aux = convert(ack);
            mandar(sockfd,cliaddr,ack);
            if(valido(m_pair.second))
                break;
            else{
                pair<vector<string>,vector<int>> m_pair_aux;
                m_pair_aux = leer(sockfd,cliaddr);
                m_pair.first = corregir(m_pair.first,m_pair_aux.first,m_pair_aux.second);
                aux = actualizarAck(aux,m_pair_aux.second);
                ack=hacerAck(aux);
                mandar(sockfd,cliaddr,ack);
            }
            if(valido(aux)) break;
        }
        for(int i =0; i < m_pair.first.size();i++){
            mensaje+=m_pair.first[i];
        }
        return mensaje;        
    }

};

