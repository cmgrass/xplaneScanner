scanner.o: ./scanner/scanner.c
	sh clean.sh
	gcc -g -Wall -c ./scanner/scanner.c -o ./scanner/scanner.o
	gcc -g -Wall -c ./util/util.c -o ./util/util.o

ScanProg: ./scanner/scanner.o ./util/util.o
	gcc ./scanner/scanner.o ./util/util.o -o ScanProg
