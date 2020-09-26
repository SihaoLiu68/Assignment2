test: parse
	./parse < test.txt

parse: parse.o scan.o
	g++ -o parse parse.o scan.o

clean:
	rm *.o parse

parse.o: scan.h
scan.o: scan.h

