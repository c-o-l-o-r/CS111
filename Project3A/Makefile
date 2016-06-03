all:
	gcc lab3a.c -o lab3a

dist:
	tar -zcvf lab3a-504669405.tar.gz lab3a.c Makefile README 

check:
	./lab3a disk-image
	diff -u super.csv super1.csv
	diff -u group.csv group1.csv
	diff -u bitmap.csv bitmap1.csv
	diff -u inode.csv inode1.csv
	diff -u directory.csv directory1.csv
	diff -u indirect.csv indirect1.csv
	
clean:
	rm lab3a
