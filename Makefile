all: indexfs

bin:
	mkdir -p bin

indexfs: bin ./bin/main.o ./bin/interface.o ./bin/suffixAutomaton.o ./bin/index.o
	g++ ./bin/main.o ./bin/interface.o ./bin/suffixAutomaton.o ./bin/index.o -o indexfs

bin/main.o: ./src/main.cpp
	g++ -std=c++11 -c ./src/main.cpp -o ./bin/main.o -Iinclude 

bin/interface.o: ./src/interface.cpp
	g++ -std=c++11 -c ./src/interface.cpp -o ./bin/interface.o -Iinclude

bin/suffixAutomaton.o: ./src/suffixAutomaton.cpp
	g++ -std=c++11 -c ./src/suffixAutomaton.cpp -o ./bin/suffixAutomaton.o -Iinclude

bin/index.o: ./src/index.cpp
	g++ -std=c++11 -c ./src/index.cpp -o ./bin/index.o -Iinclude

clean:
	rm -rf bin indexfs
