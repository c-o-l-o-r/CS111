all: SortedList.o lab2c.o 
	gcc -std=gnu99 -pthread SortedList.o lab2c.o -o lab2c

lab2c.o: lab2c.c SortedList.h 
	gcc -std=gnu99 -pthread -c lab2c.c

SortedList.o: SortedList.c SortedList.h
	gcc -std=gnu99 -pthread -c SortedList.c

dist:
	tar -zcvf lab2c-504669405.tar.gz lab2c.c SortedList.h SortedList.c Graph1.png Makefile README

clean:
	rm lab2c lab2c.o SortedList.o

test:
	echo > "result.txt"
	./lab2c --debugtest --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --threads=1 --iterations=2000 >> result.txt
	./lab2c --debugtest --threads=1 --iterations=3000 >> result.txt
	./lab2c --debugtest --threads=1 --iterations=4000 >> result.txt
	./lab2c --debugtest --threads=1 --iterations=5000 >> result.txt
	./lab2c --debugtest --threads=1 --iterations=6000 >> result.txt
	./lab2c --debugtest --threads=1 --iterations=7000 >> result.txt
	./lab2c --debugtest --threads=1 --iterations=8000 >> result.txt
	./lab2c --debugtest --threads=1 --iterations=9000 >> result.txt
	./lab2c --debugtest --threads=1 --iterations=10000 >> result.txt
	./lab2c --debugtest --threads=1 --iterations=11000 >> result.txt
	./lab2c --debugtest --threads=1 --iterations=12000 >> result.txt
	
test2:
	./lab2c --threads=10 --iterations=1000 || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=1000 --yield=i || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=1000 --yield=d || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=1000 --yield=is || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=1000 --yield=ds || ([ $$? -ne 0 ] && echo "Error!")

test3:
	./lab2c --threads=10 --iterations=10 --sync=m || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=10 --sync=m --yield=i || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=10 --sync=m --yield=d || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=10 --sync=m --yield=is || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=10 --sync=m --yield=ds || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=10 --sync=m --yield=ids || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=10 --sync=s || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=10 --sync=s --yield=i || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=10 --sync=s --yield=d || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=10 --sync=s --yield=is || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=10 --sync=s --yield=ds || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=10 --sync=s --yield=ids || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=1000 --sync=m || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=1000 --sync=m --yield=i || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=1000 --sync=m --yield=d || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=1000 --sync=m --yield=is || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=1000 --sync=m --yield=ds || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=1000 --sync=m --yield=ids || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=1000 --sync=s || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=1000 --sync=s --yield=i || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=1000 --sync=s --yield=d || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=1000 --sync=s --yield=is || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=1000 --sync=s --yield=ds || ([ $$? -ne 0 ] && echo "Error!")
	./lab2c --threads=10 --iterations=1000 --sync=s --yield=ids || ([ $$? -ne 0 ] && echo "Error!")
	
test4:
	echo > "result.txt"
	./lab2c --debugtest --list=1 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=2 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=3 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=4 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=5 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=6 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=7 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=8 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=9 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=10 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=1 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=2 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=3 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=4 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=5 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=6 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=7 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=8 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=9 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=10 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=1 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=2 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=3 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=4 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=5 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=6 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=7 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=8 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=9 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=10 --sync=s --threads=10 --iterations=1000 >> result.txt
	
test5:
	echo > "result.txt"
	./lab2c --debugtest --list=1 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=2 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=3 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=4 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=5 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=6 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=7 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=8 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=9 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=10 --threads=1 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=1 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=2 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=3 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=4 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=5 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=6 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=7 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=8 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=9 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=10 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=20 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=30 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=40 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=50 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=60 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=70 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=80 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=90 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=100 --sync=m --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=1 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=2 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=3 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=4 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=5 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=6 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=7 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=8 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=9 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=10 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=20 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=30 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=40 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=50 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=60 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=70 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=80 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=90 --sync=s --threads=10 --iterations=1000 >> result.txt
	./lab2c --debugtest --list=100 --sync=s --threads=10 --iterations=1000 >> result.txt

test6:
	echo > "analysis.txt"
	./lab2c --list=1 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=2 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=3 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=4 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=5 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=6 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=7 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=8 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=9 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=10 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=20 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=30 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=40 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=50 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=60 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=70 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=80 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=90 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=100 --sync=m --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt

test7:
	echo > "analysis.txt"
	./lab2c --list=1 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=2 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=3 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=4 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=5 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=6 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=7 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=8 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=9 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=10 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=20 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=30 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=40 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=50 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=60 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=70 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=80 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=90 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt
	./lab2c --list=100 --sync=s --threads=10 --iterations=3000 >> analysis.txt
	gprof lab2c gmon.out >> analysis.txt