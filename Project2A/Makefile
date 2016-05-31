all: 
	gcc -pthread lab2a.c -o lab2a

dist:
	tar -zcvf lab2a-504669405.tar.gz lab2a.c LockComparison.png SingleThread.png Makefile README

clean:
	rm lab2a

test:
	echo > "result.txt"
	./lab2a --debugtest --threads=1 --iterations=1 >> result.txt
	./lab2a --debugtest --threads=1 --iterations=10 >> result.txt
	./lab2a --debugtest --threads=1 --iterations=100 >> result.txt
	./lab2a --debugtest --threads=1 --iterations=1000 >> result.txt
	./lab2a --debugtest --threads=1 --iterations=10000 >> result.txt
	./lab2a --debugtest --threads=1 --iterations=100000 >> result.txt
	./lab2a --debugtest --threads=1 --iterations=1000000 >> result.txt
	./lab2a --debugtest --threads=1 --iterations=10000000 >> result.txt
	./lab2a --debugtest --threads=1 --iterations=100000000 >> result.txt
	
test2:
	echo > "result.txt"
	./lab2a --debugtest --yield  --threads=1 --iterations=1 >> result.txt
	./lab2a --debugtest --yield  --threads=1 --iterations=10 >> result.txt
	./lab2a --debugtest --yield  --threads=1 --iterations=100 >> result.txt
	./lab2a --debugtest --yield  --threads=1 --iterations=1000 >> result.txt
	./lab2a --debugtest --yield  --threads=1 --iterations=10000 >> result.txt
	./lab2a --debugtest --yield  --threads=1 --iterations=100000 >> result.txt
	./lab2a --debugtest --yield  --threads=10 --iterations=1 >> result.txt
	./lab2a --debugtest --yield  --threads=10 --iterations=10 >> result.txt
	./lab2a --debugtest --yield  --threads=10 --iterations=100 >> result.txt
	./lab2a --debugtest --yield  --threads=10 --iterations=1000 >> result.txt
	./lab2a --debugtest --yield  --threads=10 --iterations=10000 >> result.txt
	./lab2a --debugtest --yield  --threads=100 --iterations=1 >> result.txt
	./lab2a --debugtest --yield  --threads=100 --iterations=10 >> result.txt
	./lab2a --debugtest --yield  --threads=100 --iterations=100 >> result.txt
	
test3:
	echo > "result.txt"
	./lab2a --sync=m --threads=50 --iterations=1000 >> result.txt
	./lab2a --yield --sync=m --threads=50 --iterations=1000 >> result.txt
	./lab2a --sync=s --threads=50 --iterations=1000 >> result.txt
	./lab2a --yield --sync=s --threads=50 --iterations=1000 >> result.txt
	./lab2a --sync=c --threads=50 --iterations=1000 >> result.txt
	./lab2a --yield --sync=c --threads=50 --iterations=1000 >> result.txt
	./lab2a --yield  --threads=50 --iterations=1000 >> result.txt
	./lab2a --threads=50 --iterations=1000 >> result.txt

test4:
	echo > "result.txt"
	echo "Unprotect:\n" >> "result.txt"
	./lab2a --debugtest --threads=1 --iterations=1000000 >> result.txt
	./lab2a --debugtest --threads=5 --iterations=1000000 >> result.txt
	./lab2a --debugtest --threads=10 --iterations=1000000 >> result.txt
	./lab2a --debugtest --threads=15 --iterations=1000000 >> result.txt
	./lab2a --debugtest --threads=20 --iterations=1000000 >> result.txt
	./lab2a --debugtest --threads=25 --iterations=1000000 >> result.txt
	./lab2a --debugtest --threads=30 --iterations=1000000 >> result.txt
	./lab2a --debugtest --threads=35 --iterations=1000000 >> result.txt
	./lab2a --debugtest --threads=40 --iterations=1000000 >> result.txt
	./lab2a --debugtest --threads=45 --iterations=1000000 >> result.txt
	./lab2a --debugtest --threads=50 --iterations=1000000 >> result.txt
	echo "Mutex:\n" >> "result.txt"
	./lab2a --debugtest --sync=m --threads=1 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=m --threads=5 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=m --threads=10 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=m --threads=15 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=m --threads=20 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=m --threads=25 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=m --threads=30 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=m --threads=35 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=m --threads=40 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=m --threads=45 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=m --threads=50 --iterations=1000000 >> result.txt
	echo "Spin:\n" >> "result.txt"
	./lab2a --debugtest --sync=s --threads=1 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=s --threads=5 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=s --threads=10 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=s --threads=15 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=s --threads=20 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=s --threads=25 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=s --threads=30 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=s --threads=35 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=s --threads=40 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=s --threads=45 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=s --threads=50 --iterations=1000000 >> result.txt
	echo "CAS:\n" >> "result.txt"
	./lab2a --debugtest --sync=c --threads=1 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=c --threads=5 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=c --threads=10 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=c --threads=15 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=c --threads=20 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=c --threads=25 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=c --threads=30 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=c --threads=35 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=c --threads=40 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=c --threads=45 --iterations=1000000 >> result.txt
	./lab2a --debugtest --sync=c --threads=50 --iterations=1000000 >> result.txt	
	