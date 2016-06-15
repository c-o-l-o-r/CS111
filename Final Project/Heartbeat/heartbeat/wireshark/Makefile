all: get_tshark

udp_file: get_tshark.c
	gcc -o get_tshark get_tshark.c
	
clean:
	rm -f get_tshark

clobber: 
	make clean 
	rm -f *.txt *~