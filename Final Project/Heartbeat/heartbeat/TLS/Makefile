LIBS = -L/usr/lib -lssl -lcrypto
OBJS = tls_header.o

all: tls_client start_attack set_rate

tls_client: tls_client.c tls_header.o
	gcc -w -o tls_client tls_client.c $(OBJS) $(LIBS)

set_rate: set_rate.c tls_header.o
	gcc -o set_rate set_rate.c $(OBJS) $(LIBS)
	
tls_header.o: tls_header.c
	gcc -c -o tls_header.o tls_header.c $(LIBS)

start_attack: start_attack.c
	gcc -o start_attack start_attack.c $(OBJS) $(LIBS)

clean:
	rm -f set_rate start_rate tls_client tls_header.o

clobber: 
	make clean
	rm -f *.txt *~
