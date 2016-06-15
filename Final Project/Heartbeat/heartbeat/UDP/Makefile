all: udp_client start_attack set_rate

udp_file: udp_client.c
	gcc -o udp_client udp_client.c
	
start_attack: start_attack.c
	gcc -o start_attack start_attack.c
	
set_rate: set_rate.c
	gcc -o set_rate set_rate.c
	
clean:
	rm -f set_rate start_rate udp_client

clobber: 
	make clean 
	rm -f *.txt *~