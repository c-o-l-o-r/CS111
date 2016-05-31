all: server client

server:
	gcc -pthread -lmcrypt server.c -o server
	
client:
	gcc -pthread -lmcrypt client.c -o client

dist:
	tar -zcvf lab1b-504669405.tar.gz client.c server.c my.key Makefile README

clean:
	rm server
	rm client

