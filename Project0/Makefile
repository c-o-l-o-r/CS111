all:
	gcc lab0.c -o lab0

check: check1 check2 check3 check4 check5 check6

check1:
	./lab0 < check1.txt > output1.txt
	([ $$? -eq 0 ] && echo "Success") || (echo "Fail";)
	diff -u check1.txt output1.txt || (echo "Incorrect output";)
	rm output1.txt

check2:
	./lab0 --input=check2.txt > output2.txt
	([ $$? -eq 0 ] && echo "Success") || (echo "Fail";)
	diff -u check2.txt output2.txt || (echo "Incorrect output";)
	rm output2.txt

check3:
	./lab0 --output=output3.txt < check3.txt
	([ $$? -eq 0 ] && echo "Success") || (echo "Fail";)
	diff -u check3.txt output3.txt || (echo "Incorrect output";)
	rm output3.txt

check4:
	./lab0 --input=check4.txt --output=output4.txt
	([ $$? -eq 0 ] && echo "Success") || (echo "Fail";)
	diff -u check4.txt output4.txt || (echo "Incorrect output";)
	rm output4.txt

check5:
	./lab0 --segfault || ([ $$? -eq 139 ] && echo "Return code for segfault OK")

check6:
	./lab0 --segfault --catch || ([ $$? -eq 3 ] && echo "Return code for catch OK")

dist:
	tar -zcvf lab0-504669405.tar.gz lab0.c Makefile README segfault.png inspection.png check1.txt check2.txt check3.txt check4.txt

clean:
	rm lab0
