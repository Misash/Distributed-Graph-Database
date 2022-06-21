//  g++ -std=c++11 -o slave.exe slave.cpp -pthread

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <utility>

#include "biblioteca.h"
#include "orm.hpp"

using namespace std;

#define TAM_BUFFER 1024
#define N_SLAVES 2

int myPORT;

rdt myRDT;

string client(int PORT, string mensaje)
{
  cout << "cliente 1" << endl;
  int sockfd;
  char buffer[TAM_BUFFER];
  struct hostent *host;
  struct sockaddr_in servaddr;

  // host = (struct hostent *)gethostbyname((char *)"159.223.180.58");
  host = (struct hostent *)gethostbyname((char *)"127.0.0.1");

  // Creating socket file descriptor
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));

  // Filling server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr = *((struct in_addr *)host->h_addr);

  printf("[%s] [%s : %hd]\n", "Servidor: ", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));

  // char myIP[16];
  // socklen_t len2 = sizeof(servaddr);
  // getsockname(sockfd, (struct sockaddr *)&servaddr, &len2);
  // inet_ntop(AF_INET, &servaddr.sin_addr, myIP, sizeof(myIP));

  // string ip = myIP;
  // int port = ntohs(servaddr.sin_port);


  int n, len;

  char *buff = &mensaje[0];

  cout << "cliente 2" << endl;

  sendto(sockfd, buff, strlen(buff),
         MSG_CONFIRM, (const struct sockaddr *)&servaddr,
         sizeof(servaddr));

  cout << "cliente 3" << endl;

  n = recvfrom(sockfd, (char *)buffer, TAM_BUFFER,
               MSG_WAITALL, (struct sockaddr *)&servaddr,
               (socklen_t *)&len);
  buffer[n] = '\0';

  string respuesta = buffer;

  close(sockfd);

  cout << "cliente 4" << endl;
  return respuesta;
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

struct Nodo
{
  bool vacio;
  string nombre;
  vector<string> atributos;
  vector<string> relaciones;

  void cambiarNombreRelacion(string s, string n)
  {
    for (int i = 0; i < relaciones.size(); i++)
    {
      if (relaciones[i] == s)
      {
        relaciones[i] = n;
        break;
      }
    }
  }
  void setNombre(string n)
  {
    this->nombre = n;
  }
  void addAtributo(string n, string v)
  {
    atributos.push_back(n);
    atributos.push_back(v);
  }

  void addRelacion(string r)
  {
    for (int i = 0; i < relaciones.size(); i++)
    {
      if (relaciones[i] == r)
        return;
    }

    relaciones.push_back(r);
    cout << "relacion anandida " << nombre << " - " << r << endl;
  }

  void delRelacion(string r)
  {
    for (int i = 0; i < relaciones.size(); i++)
    {
      if (relaciones[i] == r)
      {
        relaciones.erase(relaciones.begin() + i);
      }
    }
  }

  string getRelaciones(bool atrib)
  {
    // A + CODIGO + #NODOS + (SIZENOMBRENODO + NOMBRENODO + #ATRIB + (SIZENOMBREATRIB + NOMBREATRIB + SIZEVAL + VAL))

    string mensaje = relaciones.size() < 10 ? "0" + to_string(relaciones.size()) : to_string(relaciones.size());

    for (int i = 0; i < relaciones.size(); i++)
    {
      mensaje += relaciones[i].size() < 10 ? "0" + to_string(relaciones[i].size()) : to_string(relaciones[i].size());
      mensaje += relaciones[i];
      mensaje += "00";
    }
    return mensaje;
  }

  string getNodo()
  {
    string msj = "";
    msj += completar(to_string(nombre.size()), 2);
    msj += nombre;
    msj += completar(to_string(atributos.size() / 2), 2);

    for (int i = 0; i < atributos.size(); i++)
    {
      msj += completar(to_string(atributos[i].size()), 2);
      msj += atributos[i];
    }
    return msj;
  }

  void setNodo(string data)
  {
    int sizeNombre;
    int nAtrib;
    sizeNombre = stoi(data.substr(2, 2));
    nombre = data.substr(4, sizeNombre);
    nAtrib = stoi(data.substr(4 + sizeNombre, 2));

    int contador = 4 + sizeNombre + 2;
    for (int i = 0; i < nAtrib; i++)
    {
      int sizeN = stoi(data.substr(contador, 2));
      contador += 2;
      atributos.push_back(data.substr(contador, sizeN));
      contador += sizeN;
      int sizeV = stoi(data.substr(contador, 2));
      contador += 2;
      atributos.push_back(data.substr(contador, sizeV));
      contador += sizeV;
    }

    cout << "Nodo: " << nombre << endl;
    for (int i = 0; i < atributos.size(); i++)
    {
      if (i % 2 != 0)
        cout << ": " << atributos[i] << endl;
      else
        cout << " -- " << atributos[i];
    }

    // A + CODE + SIZENOMBRE + #ATRIBUTOS + SIZENOMBREATRIBUTO + NOMBREATRIBUTO + SIZEVALOR + VALOR
  }

  string completar(string m, int n)
  {
    while (m.size() != n)
      m = "0" + m;
    return m;
  }

  string mk()
  {
    string msj = "CN";
    msj += completar(to_string(nombre.size()), 2);
    msj += nombre;
    msj += completar(to_string(atributos.size() / 2), 2);

    for (int i = 0; i < atributos.size(); i++)
    {
      msj += completar(to_string(atributos[i].size()), 2);
      msj += atributos[i];
    }
    return msj;

    // CN + SIZENOMBRE + NOMBRE + #ATRIBUTOS + SIZENOMBREATRIBUTO + NOMBRE ATRIBUTO + SIZEVALORATRIBUTO + VALORATRIBUTO
  }

  void print()
  {
    cout << "_______________\n";
    cout << nombre << endl;
    for (int i = 0; i < atributos.size(); i++)
    {
      if (i % 2 != 0)
        cout << ": " << atributos[i] << endl;
      else
        cout << " -- " << atributos[i];
    }
    for (int i = 0; i < relaciones.size(); i++)
      cout << "  R: " << relaciones[i] << endl;
    cout << "_______________\n";
  }
};

struct Grafo
{
  vector<Nodo> nodos;

  void addNodo(Nodo n)
  {
    nodos.push_back(n);
  }

  int buscarNodo(string n)
  {
    for (int i = 0; i < nodos.size(); i++)
    {
      if (nodos[i].nombre == n)
      {
        return i;
      }
    }
    return -1;
  };

  void cambiarRelacion(string origen, string destino, string nuevodestino)
  {
    int index = buscarNodo(origen);
    if (index == -1)
      return;

    nodos[index].delRelacion(destino);
    nodos[index].addRelacion(nuevodestino);
  }

  void delNodo(string nombre)
  {
    int index = buscarNodo(nombre);
    if (index == -1)
      return;

    for (int i = 0; i < nodos[index].relaciones.size(); i++)
    {
      if (stringToASCII(nodos[index].relaciones[i]) % N_SLAVES + 8000 == myPORT)
      {
        int index2 = buscarNodo(nodos[index].relaciones[i]);
        if (index2 == -1)
          continue;

        nodos[index2].delRelacion(nombre);
      }
      else
      {
        string input = "DR";
        // input += nombre.size() < 10 ? "0" + to_string(nombre.size()) : to_string(nombre.size());
        // input += nombre;
        // input += nodos[index].relaciones[i].size() < 10 ? "0" + to_string(nodos[index].relaciones[i].size()) : to_string(nodos[index].relaciones[i].size());
        // input += nodos[index].relaciones[i];

        input += nodos[index].relaciones[i].size() < 10 ? "0" + to_string(nodos[index].relaciones[i].size()) : to_string(nodos[index].relaciones[i].size());
        input += nodos[index].relaciones[i];
        input += nombre.size() < 10 ? "0" + to_string(nombre.size()) : to_string(nombre.size());
        input += nombre;

        cout << "inpuuuut: " << input << endl;
        string rcv = client(stringToASCII(nodos[index].relaciones[i]) % N_SLAVES + 8000, input);
      }
    }

    for (int i = 0; i < nodos.size(); i++)
    {
      if (nodos[i].nombre == nombre)
      {
        nodos.erase(nodos.begin() + i);
      }
    }
  }

  void print()
  {
    cout << "NODOS: " << endl;
    for (int i = 0; i < nodos.size(); i++)
      nodos[i].print();
  }
};

/////////////////////////////////////
Grafo miGrafo;
////////////////////////////////////

void ser(int PORT)
{
  // cout << "entre a server" << endl;

  int sockfd;
  char buffer[TAM_BUFFER];
  struct sockaddr_in servaddr, cliaddr;

  // Creating socket file descriptor
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  // Filling server information
  servaddr.sin_family = AF_INET; // IPv4
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(PORT);

  // Bind the socket with the server address
  if (bind(sockfd, (const struct sockaddr *)&servaddr,
           sizeof(servaddr)) < 0)
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  int len, n;

  len = sizeof(cliaddr); // len is value/resuslt
  // cout << "apunto de while\n";

  while (1)
  {
    cout << " READY \n";
    // AQUI RECIBE 1. CONSULTA DEL CLIENTE 2. CONSULTA DE OTRO SLAVE COMO CLIENTE
    n = recvfrom(sockfd, (char *)buffer, TAM_BUFFER,
                 MSG_WAITALL, (struct sockaddr *)&cliaddr,
                 (socklen_t *)&len);
    buffer[n] = '\0';

    string mensaje = buffer;

    cout << mensaje << endl;

    if (mensaje[0] == 'C')
    {
      if (mensaje[1] == 'N')
      {

        cout << "Entre a CN" << endl;
        cout << "Mensaje Completo: " << mensaje << endl;
        int sizeNombre = stoi(mensaje.substr(2, 2));
        string nombre = mensaje.substr(4, sizeNombre);

        int sizeAttr = stoi(mensaje.substr(6 + sizeNombre, 2));
        string attr_name = mensaje.substr(8 + sizeNombre, sizeAttr);

        int sizeValue = stoi(mensaje.substr(8 + sizeNombre + sizeAttr, 2));
        string value = mensaje.substr(10 + sizeNombre + sizeAttr, sizeValue);

        int nSlave = stringToASCII(nombre);
        nSlave = nSlave % N_SLAVES;

        ORM orm("db_NA_" + to_string(nSlave) + ".db");

        cout << "nodo creado: " << nombre << endl;
        cout << "almacenado en: " << nSlave << endl;

        if (8000 + nSlave == myPORT)
        {
          Nodo nuevoNodo;
          nuevoNodo.setNodo(mensaje);
          miGrafo.addNodo(nuevoNodo);

          orm.insert_node(nombre, attr_name, value);

          miGrafo.print();

          string mensaje = "A2";

          char *buff = &(mensaje[0]);
          sendto(sockfd, buff, strlen(buff),
                 MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                 len);
        }
        else
        {
          string respuesta = client(8000 + nSlave, mensaje);
          // TODO: RDT
          char *buff = &(respuesta[0]);
          sendto(sockfd, buff, strlen(buff),
                 MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                 len);
        }
      }
      else if (mensaje[1] == 'R')
      {
        cout << "Entre a CR" << endl;
        cout << "mensaje completo: " << mensaje << endl;

        int sizeNombre = stoi(mensaje.substr(2, 2));
        string nombre = mensaje.substr(4, sizeNombre);

        int sizeNombre2 = stoi(mensaje.substr(4 + sizeNombre, 2));
        string nombre2 = mensaje.substr(4 + sizeNombre + 2, sizeNombre2);

        // int sizeNombre2 = stoi(mensaje.substr(6 + sizeNombre + sizeRelation, 2));
        // string nombre2 = mensaje.substr(8 + sizeNombre + sizeRelation, sizeNombre2);

        int nSlave1 = stringToASCII(nombre) % N_SLAVES;
        int nSlave2 = stringToASCII(nombre2) % N_SLAVES;

        ORM orm1("db_NA_" + to_string(nSlave1) + ".db");
        ORM orm2("db_NA_" + to_string(nSlave2) + ".db");

        cout << nombre << endl;
        // cout << relation << endl;
        cout << nombre2 << endl;
        cout << nSlave1 << endl;
        cout << nSlave2 << endl;

        if (nSlave1 == nSlave2 && 000 + nSlave1 == myPORT)
        {
          int index = miGrafo.buscarNodo(nombre);
          if (index != -1)
          {
            miGrafo.nodos[index].addRelacion(nombre2);
            // orm1.insert_edge(nombre, "relacionado", nombre2);
          }

          index = miGrafo.buscarNodo(nombre2);
          if (index != -1)
          {
            miGrafo.nodos[index].addRelacion(nombre);
            // orm2.insert_edge(nombre, "relacionado", nombre2);
          }

          miGrafo.print();

          string mensaje = "A2";

          char *buff = &(mensaje[0]);
          sendto(sockfd, buff, strlen(buff),
                 MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                 len);
        }
        else if (8000 + nSlave1 == myPORT)
        {
          int index = miGrafo.buscarNodo(nombre);

          if (index != -1)
          {
            miGrafo.nodos[index].addRelacion(nombre2);
            orm1.insert_edge(nombre, "relacionado", nombre2);
          }

          miGrafo.print();

          string mensaje = "A2";

          char *buff = &(mensaje[0]);
          sendto(sockfd, buff, strlen(buff),
                 MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                 len);
        }
        else if (8000 + nSlave2 == myPORT)
        {
          int index = miGrafo.buscarNodo(nombre2);
          if (index != -1)
          {
            miGrafo.nodos[index].addRelacion(nombre);
            orm2.insert_edge(nombre, "relacionado", nombre2);
          }

          miGrafo.print();

          string mensaje = "A2";

          char *buff = &(mensaje[0]);
          sendto(sockfd, buff, strlen(buff),
                 MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                 len);
        }
        else
        {
          string respuesta;
          if (nSlave1 == nSlave2)
          {
            respuesta = client(8000 + nSlave1, mensaje);
            mensaje = "CR";
            mensaje += nombre2.size() < 10 ? "0" + to_string(nombre2.size()) : to_string(nombre2.size());
            mensaje += nombre2;
            mensaje += nombre.size() < 10 ? "0" + to_string(nombre.size()) : to_string(nombre.size());
            mensaje += nombre;
            respuesta = client(8000 + nSlave1, mensaje);
          }

          else
          {
            respuesta = client(8000 + nSlave1, mensaje);
            respuesta = client(8000 + nSlave2, mensaje);
            mensaje = "CR";
            mensaje += nombre2.size() < 10 ? "0" + to_string(nombre2.size()) : to_string(nombre2.size());
            mensaje += nombre2;
            mensaje += nombre.size() < 10 ? "0" + to_string(nombre.size()) : to_string(nombre.size());
            mensaje += nombre;
            respuesta = client(8000 + nSlave1, mensaje);
            respuesta = client(8000 + nSlave2, mensaje);
          }

          char *buff = &(respuesta[0]);
          sendto(sockfd, buff, strlen(buff),
                 MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                 len);
        }
      }
    }
    else if (mensaje[0] == 'R')
    {
      if (mensaje[1] == 'A')
      {
        cout << "Entre a RA" << endl;

        int sizeNombre = stoi(mensaje.substr(2, 2));
        string nombre = mensaje.substr(4, sizeNombre);
        int nSlave = stringToASCII(nombre) % N_SLAVES;

        cout << nombre << endl;
        cout << nSlave << endl;

        if (8000 + nSlave == myPORT)
        {
          string respuesta;
          int index = miGrafo.buscarNodo(nombre);
          if (index != -1)
          {
            respuesta = miGrafo.nodos[index].mk();
            cout << respuesta << endl;
            // RDT
            cout << "RA - nombre: " << nombre << endl;
            char *buff = &(respuesta[0]);
            sendto(sockfd, buff, strlen(buff),
                   MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                   len);
          }
        }
        else
        {
          string respuesta = client(8000 + nSlave, mensaje);
          char *buff = &(respuesta[0]);
          sendto(sockfd, buff, strlen(buff),
                 MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                 len);
        }
      }
      else if (mensaje[1] == 'R')
      {
        cout << "Entre a RR" << endl;
        int sizeNombre = stoi(mensaje.substr(2, 2));
        string nombre = mensaje.substr(4, sizeNombre);
        int nSlave = stringToASCII(nombre) % N_SLAVES;

        if (8000 + nSlave == myPORT)
        {
          string respuesta;
          int index = miGrafo.buscarNodo(nombre);
          cout << "INDEX: " << index << endl;

          if (index != -1)
          {
            respuesta = "A2";
            if (mensaje[mensaje.size() - 1] == '0')
              respuesta += miGrafo.nodos[index].getRelaciones(0);
            else if (mensaje[mensaje.size() - 1] == '1')
            {

              int index2 = miGrafo.buscarNodo(nombre);
              vector<string> relaciones = miGrafo.nodos[index2].relaciones;
              respuesta += relaciones.size() < 10 ? "0" + to_string(relaciones.size()) : to_string(relaciones.size());

              for (int i = 0; i < relaciones.size(); i++)
              {
                int relSlave = stringToASCII(relaciones[i]) % N_SLAVES;

                if (8000 + relSlave == myPORT)
                {
                  int index3 = miGrafo.buscarNodo(relaciones[i]);
                  if (index3 != -1)
                  {
                    respuesta += miGrafo.nodos[index3].getNodo();
                  }
                }
                else
                {
                  string input2 = "RA";
                  input2 += relaciones[i].size() < 10 ? "0" + to_string(relaciones[i].size()) : to_string(relaciones[i].size());
                  input2 += relaciones[i];
                  string rcv = client(8000 + relSlave, input2);
                  cout << "rcv: " << rcv << endl;
                  respuesta += rcv.substr(2, rcv.size() - 1);
                }
              }
            }

            cout << respuesta << endl;

            char *buff = &(respuesta[0]);
            sendto(sockfd, buff, strlen(buff),
                   MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                   len);

            // RDT
          }
        }
        else
        {
          string respuesta = client(8000 + nSlave, mensaje);
          char *buff = &(respuesta[0]);
          sendto(sockfd, buff, strlen(buff),
                 MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                 len);
        }
      }
    }
    else if (mensaje[0] == 'U')
    {
      cout << "Funcionalidad no implementada AUN" << endl;
      // if (mensaje[1] == 'N')
      // {
      //   cout << "Entre a UN\n";
      //   int sizeNombre = stoi(mensaje.substr(2, 2));
      //   string nombre = mensaje.substr(4, sizeNombre);
      //   int nSlave = stringToASCII(nombre) % N_SLAVES;

      //   cout << nombre << endl;
      //   cout << nSlave << endl;

      //   if (8000 + nSlave == myPORT)
      //   {
      //     int sizeNombre2 = stoi(mensaje.substr(4 + sizeNombre, 2));
      //     string nombre2 = mensaje.substr(4 + sizeNombre + 2, sizeNombre2);
      //     int index = miGrafo.buscarNodo(nombre);

      //     // TODO: LAS RELACIONES NO SE ACTUALIZAN, LAS RELACIONES DE LOS OTROS DESAPARECEN CAMBIAR
      //     if (index != -1)
      //     {
      //       vector<string> atr = miGrafo.nodos[index].atributos;
      //       vector<string> rels = miGrafo.nodos[index].relaciones;

      //       Nodo nNodo;
      //       nNodo.setNombre(nombre2);
      //       nNodo.atributos = atr;
      //       nNodo.relaciones = rels;

      //       int nuevoSlave = stringToASCII(nombre2) % N_SLAVES;

      //       string respuesta;

      //       miGrafo.delNodo(nombre);

      //       if (nuevoSlave + 8000 == myPORT)
      //       {

      //         miGrafo.addNodo(nNodo);

      //         for (int i = 0; i < rels.size(); i++)
      //         {
      //           int relSlave = stringToASCII(rels[i]) % N_SLAVES;

      //           if (relSlave + 8000 == myPORT)
      //           {
      //             int index3 = miGrafo.buscarNodo(rels[i]);
      //             miGrafo.nodos[index3].addRelacion(nombre2);
      //           }
      //           else
      //           {
      //             string input = "CR";
      //             input += rels[i].size() < 10 ? "0" + to_string(rels[i].size()) : to_string(rels[i].size());
      //             input += rels[i];
      //             input += nombre2.size() < 10 ? "0" + to_string(nombre2.size()) : to_string(nombre2.size());
      //             input += nombre2;
      //             respuesta = client(8000 + nuevoSlave, input);
      //           }
      //         }
      //       }
      //       else
      //       {
      //         respuesta = client(nuevoSlave + 8000, nNodo.mk());

      //         for (int i = 0; i < rels.size(); i++)
      //         {
      //           int relSlave = stringToASCII(rels[i]) % N_SLAVES;

      //           if (relSlave + 8000 == myPORT)
      //           {
      //             int index3 = miGrafo.buscarNodo(rels[i]);
      //             miGrafo.nodos[index3].addRelacion(nombre2);
      //           }
      //           else
      //           {
      //             string input = "CR";
      //             input += rels[i].size() < 10 ? "0" + to_string(rels[i].size()) : to_string(rels[i].size());
      //             input += rels[i];
      //             input += nombre2.size() < 10 ? "0" + to_string(nombre2.size()) : to_string(nombre2.size());
      //             input += nombre2;
      //             respuesta = client(8000 + nuevoSlave, input);

      //             input = "CR";
      //             input += nombre2.size() < 10 ? "0" + to_string(nombre2.size()) : to_string(nombre2.size());
      //             input += nombre2;
      //             input += rels[i].size() < 10 ? "0" + to_string(rels[i].size()) : to_string(rels[i].size());
      //             input += rels[i];
      //             respuesta = client(8000 + nuevoSlave, input);
      //           }
      //         }
      //       }
      //       // if(nuevoSlave + 8000 == myPORT){
      //       //         miGrafo.nodos[index].setNombre(nombre2);

      //       //         for (int i = 0; i < rels.size(); i++)
      //       //         {
      //       //                 if(stringToASCII(rels[i])%N_SLAVES + 8000 == myPORT ){
      //       //                         int index2 = miGrafo.buscarNodo(rels[i]);
      //       //                         if(index2 != -1){
      //       //                                 miGrafo.nodos[index2].cambiarNombreRelacion(nombre,nombre2);
      //       //                         }
      //       //                 }
      //       //                 else{
      //       //                         // UR + SIZENOrigen + NombreOrigen + SizeNDest + NombreDest + SizeNewNombre + NewNombre
      //       //                         string input = "UR";
      //       //                         input += rels[i].size() < 10 ? "0" + to_string(rels[i].size()) : to_string(rels[i].size());
      //       //                         input += rels[i];
      //       //                         input += nombre.size() < 10 ? "0" + to_string(nombre.size()) : to_string(nombre.size());
      //       //                         input += nombre;
      //       //                         input += nombre2.size() < 10 ? "0" + to_string(nombre2.size()) : to_string(nombre2.size());
      //       //                         input += nombre2;

      //       //                         string rpta = client(stringToASCII(rels[i])%N_SLAVES + 8000, input);
      //       //                 }
      //       //         }

      //       //         respuesta = "A2";
      //       // }
      //       // else{

      //       //         miGrafo.delNodo(nombre);
      //       //         respuesta = client(8000+nuevoSlave, nNodo.mk());
      //       //         for(int i=0; i<rels.size(); i++){
      //       //                 string input="CR";
      //       //                 input += nombre2.size() < 10 ? "0" + to_string(nombre2.size()) : to_string(nombre2.size());
      //       //                 input += nombre2;
      //       //                 input += rels[i].size() < 10 ? "0" + to_string(rels[i].size()) : to_string(rels[i].size());
      //       //                 input += rels[i];

      //       //                 respuesta = client(8000+nuevoSlave, input);
      //       //         }

      //       // }

      //       // miGrafo.print();

      //       // RDT
      //       char *buff = &(respuesta[0]);
      //       sendto(sockfd, buff, strlen(buff),
      //              MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
      //              len);
      //     }
      //   }
      //   else
      //   {
      //     string respuesta = client(8000 + nSlave, mensaje);

      //     // TODO: RDT}

      //     char *buff = &(respuesta[0]);
      //     sendto(sockfd, buff, strlen(buff),
      //            MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
      //            len);
      //   }
      // }
      // else if (mensaje[1] == 'R')
      {
        //   cout << "Entre a UR\n";
        //   int sizeNombre = stoi(mensaje.substr(2, 2));
        //   string nombre = mensaje.substr(4, sizeNombre);
        //   int sizeNombre2 = stoi(mensaje.substr(4 + sizeNombre, 2));
        //   string nombre2 = mensaje.substr(4 + sizeNombre + 2, sizeNombre2);
        //   int sizeNombre3 = stoi(mensaje.substr(4 + sizeNombre + 2 + sizeNombre2, 2));
        //   string nombre3 = mensaje.substr(4 + sizeNombre + 2 + sizeNombre2 + 2, sizeNombre3);

        //   int nSlave1 = stringToASCII(nombre) % N_SLAVES;
        //   int nSlave2 = stringToASCII(nombre2) % N_SLAVES;
        //   int nSlave3 = stringToASCII(nombre3) % N_SLAVES;

        //   if (nSlave1 == nSlave2 && nSlave2 == nSlave3 && 8000 + nSlave1 == myPORT)
        //   {
        //     int index = miGrafo.buscarNodo(nombre);
        //     if (index != -1)
        //     {
        //       miGrafo.cambiarRelacion(nombre, nombre2, nombre3);
        //     }
        //     index = miGrafo.buscarNodo(nombre2);
        //     if (index != -1)
        //     {
        //       miGrafo.nodos[index].delRelacion(nombre);
        //     }
        //     index = miGrafo.buscarNodo(nombre3);
        //     if (index != -1)
        //     {
        //       miGrafo.nodos[index].addRelacion(nombre);
        //     }
        //     string respuesta = "A2";
        //     char *buff = &(respuesta[0]);
        //     sendto(sockfd, buff, strlen(buff),
        //            MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
        //            len);
        //   }
        //   else if (nSlave1 == nSlave2 && 8000 + nSlave1 == myPORT)
        //   {
        //     int index = miGrafo.buscarNodo(nombre);
        //     if (index != -1)
        //     {
        //       miGrafo.cambiarRelacion(nombre, nombre2, nombre3);
        //     }
        //     index = miGrafo.buscarNodo(nombre2);
        //     if (index != -1)
        //     {
        //       miGrafo.nodos[index].delRelacion(nombre);
        //     }
        //     string respuesta = "A2";
        //     char *buff = &(respuesta[0]);
        //     sendto(sockfd, buff, strlen(buff),
        //            MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
        //            len);
        //   }
        //   else if (nSlave1 == nSlave3 && 8000 + nSlave1 == myPORT)
        //   {
        //     int index = miGrafo.buscarNodo(nombre);
        //     if (index != -1)
        //     {
        //       miGrafo.cambiarRelacion(nombre, nombre2, nombre3);
        //     }
        //     index = miGrafo.buscarNodo(nombre3);
        //     if (index != -1)
        //     {
        //       miGrafo.nodos[index].addRelacion(nombre);
        //     }
        //     string respuesta = "A2";
        //     char *buff = &(respuesta[0]);
        //     sendto(sockfd, buff, strlen(buff),
        //            MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
        //            len);
        //   }
        //   else if (nSlave2 == nSlave3 && 8000 + nSlave2 == myPORT)
        //   {
        //     int index = miGrafo.buscarNodo(nombre2);
        //     if (index != -1)
        //     {
        //       miGrafo.nodos[index].delRelacion(nombre);
        //     }
        //     index = miGrafo.buscarNodo(nombre3);
        //     if (index != -1)
        //     {
        //       miGrafo.nodos[index].addRelacion(nombre);
        //     }
        //     string respuesta = "A2";
        //     char *buff = &(respuesta[0]);
        //     sendto(sockfd, buff, strlen(buff),
        //            MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
        //            len);
        //   }
        //   else if (8000 + nSlave1 == myPORT)
        //   {
        //     int index = miGrafo.buscarNodo(nombre);
        //     if (index != -1)
        //     {
        //       miGrafo.cambiarRelacion(nombre, nombre2, nombre3);
        //     }

        //     string respuesta = "A2";
        //     char *buff = &(respuesta[0]);
        //     sendto(sockfd, buff, strlen(buff),
        //            MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
        //            len);
        //   }
        //   else if (8000 + nSlave2 == myPORT)
        //   {
        //     int index = miGrafo.buscarNodo(nombre2);
        //     if (index != -1)
        //     {
        //       miGrafo.nodos[index].delRelacion(nombre);
        //     }

        //     string respuesta = "A2";
        //     char *buff = &(respuesta[0]);
        //     sendto(sockfd, buff, strlen(buff),
        //            MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
        //            len);
        //   }
        //   else if (8000 + nSlave3 == myPORT)
        //   {
        //     int index = miGrafo.buscarNodo(nombre3);
        //     if (index != -1)
        //     {
        //       miGrafo.nodos[index].addRelacion(nombre);
        //     }

        //     string respuesta = "A2";
        //     char *buff = &(respuesta[0]);
        //     sendto(sockfd, buff, strlen(buff),
        //            MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
        //            len);
        //   }
        //   else
        //   {
        //     string respuesta;
        //     if (nSlave1 == nSlave2 && nSlave2 == nSlave3)
        //       respuesta = client(8000 + nSlave1, mensaje);
        //     else if (nSlave1 == nSlave2)
        //     {
        //       respuesta = client(8000 + nSlave1, mensaje);
        //       respuesta = client(8000 + nSlave3, mensaje);
        //     }
        //     else if (nSlave1 == nSlave3)
        //     {
        //       respuesta = client(8000 + nSlave1, mensaje);
        //       respuesta = client(8000 + nSlave2, mensaje);
        //     }
        //     else if (nSlave2 == nSlave3)
        //     {
        //       respuesta = client(8000 + nSlave1, mensaje);
        //       respuesta = client(8000 + nSlave2, mensaje);
        //     }
        //     else
        //     {
        //       respuesta = client(8000 + nSlave1, mensaje);
        //       respuesta = client(8000 + nSlave2, mensaje);
        //       respuesta = client(8000 + nSlave3, mensaje);
        //     }

        //     char *buff = &(respuesta[0]);
        //     sendto(sockfd, buff, strlen(buff),
        //            MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
        //            len);
        //   }
      }
    }
    else if (mensaje[0] == 'D')
    {
      if (mensaje[1] == 'N')
      {
        cout << "Entre a DN\n";
        int sizeNombre = stoi(mensaje.substr(2, 2));
        string nombre = mensaje.substr(4, sizeNombre);
        int nSlave = stringToASCII(nombre) % N_SLAVES;
        if (8000 + nSlave == myPORT)
        {
          string respuesta;
          int index = miGrafo.buscarNodo(nombre);
          if (index != -1)
          {
            miGrafo.delNodo(nombre);
          }

          respuesta = "A2";
          char *buff = &(respuesta[0]);
          sendto(sockfd, buff, strlen(buff),
                 MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                 len);
        }
        else
        {
          string respuesta = client(8000 + nSlave, mensaje);
          char *buff = &(respuesta[0]);
          sendto(sockfd, buff, strlen(buff),
                 MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                 len);
        }
      }
      else if (mensaje[1] == 'R')
      {
        cout << "Entre a DR\n";
        int sizeNombre = stoi(mensaje.substr(2, 2));
        string nombre = mensaje.substr(4, sizeNombre);
        int sizeNombre2 = stoi(mensaje.substr(4 + sizeNombre, 2));
        string nombre2 = mensaje.substr(4 + sizeNombre + 2, sizeNombre2);

        int nSlave1 = stringToASCII(nombre) % N_SLAVES;
        int nSlave2 = stringToASCII(nombre2) % N_SLAVES;

        if (nSlave1 == nSlave2 && 8000 + nSlave1 == myPORT)
        {
          int index = miGrafo.buscarNodo(nombre);
          if (index != -1)
          {
            miGrafo.nodos[index].delRelacion(nombre2);
          }

          index = miGrafo.buscarNodo(nombre2);
          if (index != -1)
          {
            miGrafo.nodos[index].delRelacion(nombre);
          }

          string respuesta = "A2";
          char *buff = &(respuesta[0]);
          sendto(sockfd, buff, strlen(buff),
                 MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                 len);
        }
        else if (8000 + nSlave1 == myPORT)
        {
          int index = miGrafo.buscarNodo(nombre);
          if (index != -1)
          {
            miGrafo.nodos[index].delRelacion(nombre2);
          }

          string respuesta = "A2";
          char *buff = &(respuesta[0]);
          sendto(sockfd, buff, strlen(buff),
                 MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                 len);
        }
        else if (8000 + nSlave2 == myPORT)
        {
          int index = miGrafo.buscarNodo(nombre2);
          if (index != -1)
          {
            miGrafo.nodos[index].delRelacion(nombre);
          }

          string respuesta = "A2";
          char *buff = &(respuesta[0]);
          sendto(sockfd, buff, strlen(buff),
                 MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                 len);
        }
        else
        {
          string respuesta;
          if (nSlave1 == nSlave2)
          {
            respuesta = client(8000 + nSlave1, mensaje);
          }
          else
          {
            respuesta = client(8000 + nSlave1, mensaje);
            respuesta = client(8000 + nSlave2, mensaje);
          }

          char *buff = &(respuesta[0]);
          sendto(sockfd, buff, strlen(buff),
                 MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                 len);
        }
      }
    }
  }
};

int main()
{

  int PORT;
  cout << "ID 0 PARA SER EL NODO PRINCIPAL >";
  cin >> PORT;

  if (PORT == 0)
    PORT = 9000;
  else
    PORT = PORT + 8000 - 1;

  myPORT = PORT;

  if (PORT == 9000)
    cout << " ** Eres el NODO PRINCIPAL : " << myPORT << endl;
  else
    cout << " ** ERES un NODO ALMACENAMIENTO : " << PORT << endl;

  // miGrafo.iniciales();

  ser(PORT);

  // cout << "ERES EL SLAVE : " << PORT << endl;

  while (1)
    ;
  return 0;
}
