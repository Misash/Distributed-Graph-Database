
client:
	@ cd Code/ &&  g++ -o client client.cpp -pthread -l sqlite3 && ./client

server:
	@ cd Code/ &&  g++ -o server server.cpp -pthread -l sqlite3 && ./server

server:


.PHONY: clean
clean:
	@ cd Code/ && rm -f *.o client server 

