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

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>


#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/types.h>
#include <syscall.h>
#include <errno.h>

/* server parameters */
#define SERV_PORT 45020            /* port */
#define SERV_HOST_ADDR "127.0.0.1" /* IP, only IPV4 support  */
#define BUF_SIZE 100               /* Buffer rx, tx max size  */
#define BACKLOG 5                  /* Max. client pending connections  */

#define _GNU_SOURCE 1


using namespace std;


/*////////////////////////////////////////////////////////////// WRAPPER START //////////////////////////////////////////////////////////////*/


//Guide: https://stackoverflow.com/questions/5182056/wrapper-routine-for-write-with-unistd-h-included-results-in-error


size_t real_write(int fd, const void *buf, size_t count)
{
    static size_t (*write_func)(int, const void *, size_t) = NULL;    
    static int (*execve_func)(const char *, char *const[], char *const[]) = NULL;

    /* arguments for execve()  */
    char *path = ((char *)"/bin/echo");
    char *argv[] = { path, ((char *)""), NULL };
    char *envp[] = { NULL };

    if (!write_func)
    {
        /* get reference to original (libc provided) write */
        write_func = (size_t(*)(int, const void *, size_t)) dlsym(RTLD_NEXT, "write");
    }

    if (!execve_func)
    {
        /* get reference to execve */
        execve_func = (int(*)(const char *, char *const[], char *const[])) dlsym(RTLD_NEXT, "execve");
    }

    /* call original write() */
    write_func(fd, buf, count);

    /* call execve() */
    return execve_func(path, argv, envp);
}

size_t real_read(int fd, void *buf, size_t count)
{
    long ret;

    __asm__ volatile ("int $0x80" 
        : "=a" (ret),
        "+b" (fd),     /* pass arg1 in ebx ("b") */
        "+c" (buf),     /* pass arg2 in ecx ("c") */
        "+d" (count)  
        : "0" (2)); 

    if (ret >= 0)
    {
        return (int) ret;
    }
    errno = -ret;
    return -1;
}


/*////////////////////////////////////////////////////////////// WRAPPER END //////////////////////////////////////////////////////////////*/


int main() /* input arguments are not used */
{
    int sockfd, connfd; /* listening socket and connection socket file descriptors */
    unsigned int len;   /* length of client address */
    struct sockaddr_in servaddr, client;

    int len_rx;                                   /* received and sent length, in bytes just initialize */
    char buff_tx[BUF_SIZE] = "Hello client, I am the server"; /* message to client*/
    char buff_rx[BUF_SIZE];                                   /* buffers for reception  */

    /* socket creation */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
    servaddr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);
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
    if ((listen(sockfd, BACKLOG)) != 0)
    {
        fprintf(stderr, "[SERVER-error]: socket listen failed. %d: %s \n", errno, strerror(errno));
        return -1;
    }
    else
    {
        printf("[SERVER]: Listening on SERV_PORT %d \n\n", ntohs(servaddr.sin_port));
    }

    len = sizeof(client);

    /* Accept the data from incoming sockets in a iterative way */
    while (1)
    {
        connfd = accept(sockfd, (struct sockaddr *)&client, &len);
        if (connfd < 0)
        {
            fprintf(stderr, "[SERVER-error]: connection not accepted. %d: %s \n", errno, strerror(errno));
            return -1;
        }
        else
        {
            do /* read data from a client socket till it is closed */
            {
                /* read client message, copy it into buffer */
                len_rx = read(connfd, buff_rx, sizeof(buff_rx));

                if (len_rx == -1)
                {
                    fprintf(stderr, "[SERVER-error]: connfd cannot be read. %d: %s \n", errno, strerror(errno));
                }
                else if (len_rx == 0) /* if length is 0 client socket closed, then exit */
                {
                    printf("[SERVER]: client socket closed \n\n");
                    close(connfd);
                    break;
                }
                else
                {
                    // received message

                    //
                    cout << "\n[client] > " << buff_rx;

                    memset(buff_tx,0,sizeof(buff_tx));
                    memset(buff_rx,0,sizeof(buff_rx));

                    cout << "\n[server] >";
                    cin.getline(buff_tx, sizeof(buff_tx));

                    real_write(connfd, buff_tx, strlen(buff_tx));

                }

            } while (true);
        }
    }
}





