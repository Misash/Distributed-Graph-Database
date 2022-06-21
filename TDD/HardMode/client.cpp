//  g++ -std=c++11 -o c.exe c.cpp -pthread

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
#include <string>
#include <iostream>
#include <vector>
#include <utility>

#include "biblioteca.h"

rdt myRDT;

using namespace std;

#define MAXLINE 1024

int PORT = 9000;

struct Nodo
{
  string nombre;
  vector<string> atributos;

  void setNombre(string n)
  {
    this->nombre = n;
  }

  void addAtributo(string n, string v)
  {
    atributos.push_back(n);
    atributos.push_back(v);
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

    cout << "_______________" << endl;
    cout << "Nodo: " << nombre << endl;
    for (int i = 0; i < atributos.size(); i++)
    {
      if (i % 2 != 0)
        cout << ": " << atributos[i] << endl;
      else
        cout << " -- " << atributos[i];
    }
    cout << "_______________" << endl;

    // A + CODE + SIZENOMBRE + NOMBRE + #ATRIBUTOS + SIZENOMBREATRIBUTO + NOMBREATRIBUTO + SIZEVALOR + VALOR
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
};

struct Relacion
{
  string nombre1;
  string nombre2;
  string relation;

  void setNodos(string n1, string n2)
  {
    nombre1 = n1;
    nombre2 = n2;
  }

  void setNodos(string n1, string n2, string r)
  {
    nombre1 = n1;
    nombre2 = n2;
    relation = r;
  }

  string completar(string m, int n)
  {
    while (m.size() != n)
      m = "0" + m;
    return m;
  }

  string mk()
  {
    string msj = "CR";
    msj += completar(to_string(nombre1.size()), 2);
    msj += nombre1;

    // msj += completar(to_string(relation.size()), 2);
    // msj += relation;

    msj += completar(to_string(nombre2.size()), 2);
    msj += nombre2;
    return msj;

    // CR + SIZENOMBRE1 + NOMBRE1 +  SIZENOMBRE2 + NOMBRE2
  }

  void setRelaciones(string data)
  {

    int nNodos;
    nNodos = stoi(data.substr(2, 2));
    int contador = 4;
    for (int i = 0; i < nNodos; i++)
    {
      vector<string> atributos;
      string nombre;
      int sizeNombre;
      int nAtrib;
      sizeNombre = stoi(data.substr(contador, 2));
      contador += 2;
      nombre = data.substr(contador, sizeNombre);
      contador += sizeNombre;
      nAtrib = stoi(data.substr(contador, 2));

      contador += 2;

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

      cout << "______________" << endl;
      cout << "Nodo: " << nombre << endl;
      for (int i = 0; i < atributos.size(); i++)
      {
        if (i % 2 != 0)
          cout << ": " << atributos[i] << endl;
        else
          cout << " -- " << atributos[i];
      }
      cout << "______________" << endl;

      atributos.clear();
    }

    // A + CODIGO + #NODOS + (SIZENOMBRENODO + NOMBRENODO + #ATRIB + (SIZENOMBREATRIB + NOMBREATRIB + SIZEVAL + VAL))
  }
};

int main()
{
  int sockfd;
  char buffer[MAXLINE];
  struct hostent *host;
  struct sockaddr_in servaddr;
  host = (struct hostent *)gethostbyname((char *)"159.223.180.58");

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

  int n, len;

  while (1)
  {
    string menu;
    Nodo nNodo;
    Relacion nRelacion;

    do
    {
      cout << "C: Crear\nR: Leer\nU: Actualizar\nD: Borrar\n>> ";
      cin >> menu;
    } while (menu != "C" && menu != "R" && menu != "U" && menu != "D" && menu != "c" && menu != "r" && menu != "u" && menu != "d");

    if (menu == "C")
    {
      do
      {
        cout << "1: Crear Nodo\n2: Crear Relacion\n>> ";
        cin >> menu;
      } while (menu != "1" && menu != "2");

      if (menu == "1")
      {
        string input;
        string attr;
        string attr_value;
        string mensaje;
        cout << "\nGDBU >";
        cin >> input >> attr >> attr_value;

        nNodo.setNombre(input);
        nNodo.addAtributo(attr, attr_value);

        // do{

        //         cout << "1: Añadir Atributo\n2: Terminar\n>>";
        //         cin>>input;

        //         if(input=="1"){
        //                 string atrib;
        //                 cout << "Nombre de Atributo >>";
        //                 cin >> input;
        //                 cout << "Valor de Atributo >>";
        //                 cin>>atrib;
        //                 nNodo.addAtributo(input,atrib);
        //         }

        // }while(input != "2");

        mensaje = nNodo.mk();

        char *buff = &mensaje[0];

        sendto(sockfd, buff, strlen(buff),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));

        cout << mensaje << endl;

        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     (socklen_t *)&len);
        buffer[n] = '\0';

        string mensajeR = buffer;

        if (mensajeR == "A2")
          cout << "\nOperacion exitosa\n\n";
        else
          cout << "\nOcurrio un fallo\n\n";
      }
      else if (menu == "2")
      {
        string n1;
        string n2;
        string relation;
        string mensaje;
        cout << "\nGDBU >";
        cin >> n1 >> relation >> n2;
        // cout << "Ingresa Nombre de Nodo 2 >>";
        // cin >> n2;

        nRelacion.setNodos(n1, n2);

        mensaje = nRelacion.mk();

        char *buff = &mensaje[0];
        sendto(sockfd, buff, strlen(buff),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));

        cout << mensaje << endl;

        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     (socklen_t *)&len);
        buffer[n] = '\0';

        string mensajeR = buffer;

        if (mensajeR == "A2")
          cout << "\nOperacion exitosa\n\n";
        else
          cout << "\nOcurrio un fallo\n\n";
      }
    }
    else if (menu == "R")
    {
      Nodo nodoLeido;

      do
      {
        cout << "1: Leer Atributos\n2: Leer Relaciones\n3: Leer Relaciones con Atrbutos\n>> ";
        cin >> menu;
      } while (menu != "1" && menu != "2" && menu != "3");

      if (menu == "1")
      {
        string input;
        string mensaje;
        cout << "Ingresa Nombre de Nodo >>";
        cin >> input;

        mensaje = "RA";
        mensaje += input.size() < 10 ? "0" + to_string(input.size()) : to_string(input.size());
        mensaje += input;

        char *buff = &mensaje[0];
        sendto(sockfd, buff, strlen(buff),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));

        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     (socklen_t *)&len);
        buffer[n] = '\0';

        string mensajeR = buffer;
        // string mensajeR = "A204PACO0204HOLA021104CHAU0222";

        // TODO: RECIBIR MENSAJE

        nodoLeido.setNodo(mensajeR);
      }
      else if (menu == "2")
      {
        Relacion relaciones;
        string input;
        string mensaje;
        cout << "Ingresa Nombre de Nodo >>";
        cin >> input;

        // TODO: NIVEL, FLAG ATRIB
        mensaje = "RR";
        mensaje += input.size() < 10 ? "0" + to_string(input.size()) : to_string(input.size());
        mensaje += input;
        mensaje += "0";

        char *buff = &mensaje[0];
        sendto(sockfd, buff, strlen(buff),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));

        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     (socklen_t *)&len);
        buffer[n] = '\0';

        string mensajeR = buffer;

        cout << "RR :::: " << mensajeR << endl;
        // string mensajeR = "A20204PACO0105PEDRO021104LUIS0204CHAU020002JE02JE";
        // TODO: RECIBIR MENSAJE

        cout << "Relaciones de " << input << ": " << endl;
        relaciones.setRelaciones(mensajeR);
      }
      else if (menu == "3")
      {
        Relacion relaciones;
        string input;
        string mensaje;
        cout << "Ingresa Nombre de Nodo >>";
        cin >> input;

        // TODO: NIVEL, FLAG ATRIB
        mensaje = "RR";
        mensaje += input.size() < 10 ? "0" + to_string(input.size()) : to_string(input.size());
        mensaje += input;
        mensaje += "1";

        char *buff = &mensaje[0];
        sendto(sockfd, buff, strlen(buff),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));

        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     (socklen_t *)&len);
        buffer[n] = '\0';

        string mensajeR = buffer;
        cout << mensajeR << endl;
        // string mensajeR = "A20204PACO0105PEDRO021104LUIS0204CHAU020002JE02JE";
        // TODO: RECIBIR MENSAJE

        cout << "Relaciones de " << input << ": " << endl;
        relaciones.setRelaciones(mensajeR);
      }
    }
    else if (menu == "U")
    {
      do
      {
        cout << "1: Actualizar Nombre\n2: Actualizar Relaciones\n>> ";
        cin >> menu;
      } while (menu != "1" && menu != "2");

      if (menu == "1")
      {
        string input;
        string mensaje = "UN";
        cout << "Ingresa nombre de Nodo >>";
        cin >> input;
        mensaje += input.size() < 10 ? "0" + to_string(input.size()) : to_string(input.size());
        mensaje += input;
        cout << "Ingresa nuevo nombre de Nodo >>";
        cin >> input;
        mensaje += input.size() < 10 ? "0" + to_string(input.size()) : to_string(input.size());
        mensaje += input;

        char *buff = &mensaje[0];
        sendto(sockfd, buff, strlen(buff),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));

        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     (socklen_t *)&len);
        buffer[n] = '\0';

        string mensajeR = buffer;

        // UN + SIZENOMBRE + NOMBRE + SIZENEWNOMBRE + NEWNOMBRE

        cout << mensajeR << endl;

        if (mensajeR == "A2")
          cout << "\nOperacion exitosa\n\n";
        else
          cout << "\nOcurrio un fallo\n\n";
      }
      else if (menu == "2")
      {
        string input;
        string mensaje = "UR";
        cout << "Ingresa nombre de Nodo Origen >>";
        cin >> input;
        mensaje += input.size() < 10 ? "0" + to_string(input.size()) : to_string(input.size());
        mensaje += input;
        cout << "Ingresa nombre de Nodo Destino >>";
        cin >> input;
        mensaje += input.size() < 10 ? "0" + to_string(input.size()) : to_string(input.size());
        mensaje += input;
        cout << "Ingresa nombre de nuevo Nodo Destino >>";
        cin >> input;
        mensaje += input.size() < 10 ? "0" + to_string(input.size()) : to_string(input.size());
        mensaje += input;

        char *buff = &mensaje[0];
        sendto(sockfd, buff, strlen(buff),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));

        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     (socklen_t *)&len);
        buffer[n] = '\0';

        string mensajeR = buffer;

        // UR + SIZENOrigen + NombreOrigen + SizeNDest + NombreDest + SizeNewNombre + NewNombre

        cout << mensaje << endl;

        if (mensajeR == "A2")
          cout << "\nOperacion exitosa\n\n";
        else
          cout << "\nOcurrio un fallo\n\n";
      }
    }
    else if (menu == "D")
    {
      string menu;
      do
      {
        cout << "1: Borrar Nodo\n2: Borrar Relacion\n";
        cin >> menu;
      } while (menu != "1" && menu != "2");

      if (menu == "1")
      {
        string input;
        string mensaje = "DN";
        cout << "Ingrese Nombre de Nodo >>";
        cin >> input;
        mensaje += input.size() < 10 ? "0" + to_string(input.size()) : to_string(input.size());
        mensaje += input;

        char *buff = &mensaje[0];
        sendto(sockfd, buff, strlen(buff),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));

        // DN + SIZENOMBRE + NOMBRE

        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     (socklen_t *)&len);
        buffer[n] = '\0';

        string mensajeR = buffer;

        cout << mensaje << endl;

        if (mensajeR == "A2")
          cout << "\nOperacion exitosa\n\n";
        else
          cout << "\nOcurrio un fallo\n\n";
      }
      else if (menu == "2")
      {
        string input;
        string mensaje = "DR";
        cout << "Ingrese Nombre de Nodo Origen >>";
        cin >> input;
        mensaje += input.size() < 10 ? "0" + to_string(input.size()) : to_string(input.size());
        mensaje += input;
        cout << "Ingrese Nombre de Nodo Destino >>";
        cin >> input;
        mensaje += input.size() < 10 ? "0" + to_string(input.size()) : to_string(input.size());
        mensaje += input;

        char *buff = &mensaje[0];

        sendto(sockfd, buff, strlen(buff),
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));

        // DR + SIZENOMBRE1 + NOMBRE1 + SIZENOMBRE2 + NOMBRE2

        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     (socklen_t *)&len);
        buffer[n] = '\0';

        string mensajeR = buffer;

        cout << mensaje << endl;

        if (mensajeR == "A2")
          cout << "\nOperacion exitosa\n\n";
        else
          cout << "\nOcurrio un fallo\n\n";
      }
    }

    // sendto(sockfd, buffer , strlen(hello),
    //         MSG_CONFIRM, (const struct sockaddr *) &servaddr,
    //                 sizeof(servaddr));

    // n = recvfrom(sockfd, (char *)buffer, MAXLINE,
    //                         MSG_WAITALL, (struct sockaddr *) &servaddr,
    //                         (socklen_t*)&len);
    // buffer[n] = '\0';
  }
  close(sockfd);
  return 0;
}
