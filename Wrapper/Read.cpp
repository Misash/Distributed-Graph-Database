#include <iostream> 
#include <stdio.h>


//lib especiales
#include <dlfcn.h>              // se encarga de los comandos RTLD
#include <unistd.h>             // se encarga de el uso del Wrapper
#include <sys/types.h>          // ayuda con el uso de las variables volatiles 
#include <syscall.h>            // llama funciones que interfiere con c++
#include <errno.h>              // Se encraga del errno

#define _GNU_SOURCE 1           // para smartpointers


size_t real_read(int fd, void *buf, size_t count)
{
    long ret;

    __asm__ volatile ("int $0x80"   //AssemblerTemplate     // Aqui se trabaja con la arquitura de la maquina juega con el comando 
        : "=a" (ret),               //OutputOperands
        "+b" (fd),                  //InputOperands         /* pass arg1 in ebx ("b") */
        "+c" (buf),                 //InputOperands         /* pass arg2 in ecx ("c") */
        "+d" (count)                //InputOperands         /* pass arg2 in ecx ("d") */
        : "0" (2));                 //Clobbers 

    if (ret >= 0)
    {
        return (int) ret;
    }
    errno = -ret;                   //preprocessor macro used for error indication.
    return -1;                      
}



//                                      Tester                                  //


int main(int argc, char *argv[])
{
    int temporal=0;
    int filedes = 1;
    char buf[] = "write() called\n";
    size_t nbyte = sizeof buf / sizeof buf[0];

    temporal=real_read(filedes, buf, nbyte);
    std::cout<< temporal; 
    return 0;
}
