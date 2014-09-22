CC = g++

INC = ../base
OPT = -Wall -Wno-deprecated -g

OBJS = ClientConn.o ../base/libbase.a 
CLIENT_OBJS = test_client.o $(OBJS) 
PRESSURE_OBJS = pressure_test.o $(OBJS) 

CLIENT = test_client
PRESSURE_TEST = pressure_test

all: $(CLIENT) $(PRESSURE_TEST) $(TEST_DB)

$(CLIENT): $(CLIENT_OBJS)
	$(CC)  $(OPT) -o $@ $(CLIENT_OBJS) -lpthread 

$(PRESSURE_TEST) : $(PRESSURE_OBJS)
	$(CC) $(OPT) -o $@ $(PRESSURE_OBJS) -lpthread 

ClientConn.o: ClientConn.cpp
	$(CC) -I $(INC) $(OPT) -c -o $@ $<

test_client.o: test_client.cpp
	$(CC) -I $(INC) $(OPT) -c -o $@ $<

pressure_test.o: pressure_test.cpp
	$(CC) -I $(INC) $(OPT) -c -o $@ $<

clean:
	rm -f $(CLIENT) $(PRESSURE_TEST) $(TEST_DB) *.o

