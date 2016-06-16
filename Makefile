all: binmake

binmake:
	g++ -std=c++11 src/BinBuilder.cpp src/binmake.cpp src/utils.cpp -o binmake -Wall

clean:
	rm -rf *.o binmake
