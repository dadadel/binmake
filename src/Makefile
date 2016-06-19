BIN_PATH=..
SOURCES = BinMaker.cpp \
          binmake.cpp \
          utils.cpp
TARGET = $(BIN_PATH)/binmake
OBJECTS = $(SOURCES:.cpp=.o)

CXX=g++
CFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS =

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(INC) $(CFLAGS)

clean:
	rm -rf *.o $(TARGET)
