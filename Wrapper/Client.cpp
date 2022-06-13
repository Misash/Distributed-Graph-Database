/*************************************************************************************/
/* @file    client_1.c                                                               */
/* @brief   This clients connects,                                                   */
/*          sends a text, reads what server and disconnects                          */
/*************************************************************************************/

// CPP
#include <iostream>
#include <bits/stdc++.h> //stl
using namespace std;

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_ADDRESS "127.0.0.1" /* server IP */
#define PORT 45020

/* Test sequences */
char buf_tx[] = "Hello server. I am a client";
char buf_rx[100]; /* receive buffer */

/* This clients connects, sends a text and disconnects */
int main()
{
    int sockfd;
    struct sockaddr_in servaddr;

    /* Socket creation */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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

    /* assign IP, PORT */
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    servaddr.sin_port = htons(PORT);

    /* try to connect the client socket to server socket */
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        return -1;
    }

    printf("connected to the server..\n");


    do{
        memset(buf_tx,0,sizeof(buf_tx));
        memset(buf_rx,0,sizeof(buf_rx));
        
        cout << "\n[client] >";
        cin.getline(buf_tx,sizeof(buf_tx));

        write(sockfd, buf_tx, sizeof(buf_tx)); 

        read(sockfd, buf_rx, sizeof(buf_rx));
        cout<<"[server] > "<<buf_rx;

    }while(buf_rx != "bye" && buf_tx != "bye");

    /* close the socket */
    close(sockfd);
}

