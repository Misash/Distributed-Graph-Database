# To Compile Server (Wrapper) # 

# Lib Needed #
  * #include <stdio.h>
  * #include <dlfcn.h>
  * #include <unistd.h>
  * #include <sys/types.h>
  * #include <syscall.h>
  * #include <errno.h>

# Includes following wrappers # 
  * read()   -   real_read()
  * write()  -   real_write()

# Commands to Compile #
  * g++ -Wall -Werror -Wl,--no-as-needed -ldl Server.cpp -o test
  * ./test
