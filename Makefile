CPP=g++
 
CPPFLAGS=-g -std=c++0x -Wall
  
LIBCDS=libcds
LIB=$(LIBCDS)/lib/libcds.a
INCLUDES=-I$(LIBCDS)/includes/
SRC=src/
all: 
	    $(CPP) $(CPPFLAGS) $(INCLUDES) -o test $(SRC)main.cpp $(LIB)		 
clean:
	    rm 
