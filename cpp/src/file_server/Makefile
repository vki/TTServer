CC = g++

INC = ../base
OPT = -Wall -Wno-deprecated -g

OBJS = FileConn.o file_server.o ../base/libbase.a 

SERVER = file_server

all: $(SERVER)

$(SERVER): $(OBJS)
	$(CC)  $(OPT) -o $@ $(OBJS) -lpthread 

FileConn.o: FileConn.cpp
	$(CC) -I $(INC) $(OPT) -c -o $@ $<
	
file_server.o: file_server.cpp
	$(CC) -I $(INC) $(OPT) -c -o $@ $<

clean:
	rm -f $(SERVER) *.o

