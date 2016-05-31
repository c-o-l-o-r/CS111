all: SortedList.o lab2b.o 
	gcc -g -std=gnu99 -pthread SortedList.o lab2b.o -o lab2b

lab2b.o: lab2b.c SortedList.h 
	gcc -g -std=gnu99 -pthread -c lab2b.c

SortedList.o: SortedList.c SortedList.h
	gcc -g3 -std=gnu99 -pthread -c SortedList.c

dist:
	tar -zcvf lab2b-504669405.tar.gz lab2b.c SortedList.h SortedList.c Graph1.png Graph2.png Makefile README

clean:
	rm lab2b lab2b.o SortedList.o

test:
	echo > "result.txt"
	./lab2b --debugtest --threads=1 --iterations=1000 >> result.txt
	./lab2b --debugtest --threads=1 --iterations=2000 >> result.txt
	./lab2b --debugtest --threads=1 --iterations=3000 >> result.txt
	./lab2b --debugtest --threads=1 --iterations=4000 >> result.txt
	./lab2b --debugtest --threads=1 --iterations=5000 >> result.txt
	./lab2b --debugtest --threads=1 --iterations=6000 >> result.txt
	./lab2b --debugtest --threads=1 --iterations=7000 >> result.txt
	./lab2b --debugtest --threads=1 --iterations=8000 >> result.txt
	./lab2b --debugtest --threads=1 --iterations=9000 >> result.txt
	./lab2b --debugtest --threads=1 --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=1 --iterations=11000 >> result.txt
	./lab2b --debugtest --threads=1 --iterations=12000 >> result.txt
	
test2:
	./lab2b --threads=10 --iterations=1000 || ([ $$? -ne 0 ] && echo "Error!")
	./lab2b --threads=10 --iterations=1000 --yield=i || ([ $$? -ne 0 ] && echo "Error!")
	./lab2b --threads=10 --iterations=1000 --yield=d || ([ $$? -ne 0 ] && echo "Error!")
	./lab2b --threads=10 --iterations=1000 --yield=is || ([ $$? -ne 0 ] && echo "Error!")
	./lab2b --threads=10 --iterations=1000 --yield=ds || ([ $$? -ne 0 ] && echo "Error!")

test3:
	./lab2b --threads=10 --iterations=1000 --sync=m || ([ $$? -ne 0 ] && echo "Error!")
	./lab2b --threads=10 --iterations=1000 --sync=m --yield=i || ([ $$? -ne 0 ] && echo "Error!")
	./lab2b --threads=10 --iterations=1000 --sync=m --yield=d || ([ $$? -ne 0 ] && echo "Error!")
	./lab2b --threads=10 --iterations=1000 --sync=m --yield=is || ([ $$? -ne 0 ] && echo "Error!")
	./lab2b --threads=10 --iterations=1000 --sync=m --yield=ds || ([ $$? -ne 0 ] && echo "Error!")
	./lab2b --threads=10 --iterations=1000 --sync=s || ([ $$? -ne 0 ] && echo "Error!")
	./lab2b --threads=10 --iterations=1000 --sync=s --yield=i || ([ $$? -ne 0 ] && echo "Error!")
	./lab2b --threads=10 --iterations=1000 --sync=s --yield=d || ([ $$? -ne 0 ] && echo "Error!")
	./lab2b --threads=10 --iterations=1000 --sync=s --yield=is || ([ $$? -ne 0 ] && echo "Error!")
	./lab2b --threads=10 --iterations=1000 --sync=s --yield=ds || ([ $$? -ne 0 ] && echo "Error!")

test4:
	echo > "result.txt"
	./lab2b --debugtest --threads=1 --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=1 --sync=m --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=2 --sync=m --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=3 --sync=m --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=4 --sync=m --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=5 --sync=m --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=6 --sync=m --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=7 --sync=m --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=8 --sync=m --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=9 --sync=m --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=10 --sync=m --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=1 --sync=s --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=2 --sync=s --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=3 --sync=s --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=4 --sync=s --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=5 --sync=s --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=6 --sync=s --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=7 --sync=s --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=8 --sync=s --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=9 --sync=s --iterations=10000 >> result.txt
	./lab2b --debugtest --threads=10 --sync=s --iterations=10000 >> result.txt