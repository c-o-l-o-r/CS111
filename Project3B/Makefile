all: data.o lab3b.o 
	gcc -g data.o lab3b.o -o lab3b

lab3b.o: lab3b.c data.h 
	gcc -g -c lab3b.c

data.o: data.c data.h
	gcc -g -c data.c

run:
	./lab3b
	
check:
	./lab3b
	sort lab3b_check.txt > lab3b_check_sort.txt
	sort lab3b_check1.txt > lab3b_check1_sort.txt
	diff -u lab3b_check_sort.txt lab3b_check1_sort.txt
	rm lab3b_check_sort.txt lab3b_check1_sort.txt
	
dist:
	tar -zcvf lab3b-504669405.tar.gz lab3b.c data.h data.c Makefile README 
	
clean:
	rm data.o lab3b.o lab3b
