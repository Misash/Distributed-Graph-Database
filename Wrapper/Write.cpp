#include <iostream> 
#include <stdio.h>


//lib especiales
#include <dlfcn.h>              // se encarga de los comandos RTLD
#include <unistd.h>             // se encarga de el uso del Wrapper
#include <sys/types.h>          // ayuda con el uso de las variables volatiles 
#include <syscall.h>            // llama funciones que interfiere con c++
#include <errno.h>              // Se encraga del errno

#define _GNU_SOURCE 1           // para smartpointers


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


int main(int argc, char *argv[])
{
    int filedes = 1;
    char buf[] = "write() called\n";
    size_t nbyte = sizeof buf / sizeof buf[0];

    real_write(filedes, buf, nbyte);

    return 0;
}
