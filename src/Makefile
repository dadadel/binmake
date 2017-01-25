BIN_PATH=../bin
LIB_PATH=../lib
SOURCES = BinStream.cpp \
          binmake.cpp \
          bin_tools.cpp \
          utils.cpp
SOURCES_LIB = BinStream.cpp \
              bin_tools.cpp \
              utils.cpp
INC_PATH = ../include
INC = -I. -I$(INC_PATH)
TARGET_BIN = $(BIN_PATH)/binmake
TARGET_LIB_D = $(LIB_PATH)/libbinstream.so
TARGET_LIB_A = $(LIB_PATH)/libbinstream.a
OBJECTS = $(SOURCES:.cpp=.o)
OBJECTS_LIB = $(SOURCES_LIB:.cpp=.o)

$(shell mkdir -p $(BIN_PATH))
$(shell mkdir -p $(LIB_PATH))

AR=ar
AR_FLAGS=rvs
CXX=g++
CFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS =
CFLAGS_D = -std=c++11 -Wall -Wextra -fPIC
LDFLAGS_D = -shared

all: $(SOURCES) $(TARGET_BIN) $(TARGET_LIB_A) $(TARGET_LIB_D)

$(TARGET_BIN): $(OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(INC) $(CFLAGS)

$(TARGET_LIB_D): $(SOURCES_LIB)
	$(CXX) -c $^ $(INC) $(CFLAGS_D)
	$(CXX) -o $@ $(OBJECTS_LIB) $(LDFLAGS_D)

$(TARGET_LIB_A): $(OBJECTS_LIB)
	$(AR) $(AR_FLAGS) $@ $^

clean:
	rm -rf *.o $(TARGET_BIN) $(TARGET_LIB_A) $(TARGET_LIB_D)
