CPP = g++
CXXFLAGS = -std=c++11 -O2 -g
CXXFLAGS += -Wall -Wextra
OBJS = kvstore2pcsystem.o participant_server.o coordinator_server.o Command_handle.o
LIBS = -pthread

kvstore2pcsystem : $(OBJS)
	$(CPP) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)
	rm *.o *.hpp.gch

kvstore2pcsystem.o : kvstore2pcsystem.cc
	$(CPP) $(CXXFLAGS) -c $^ $(LIBS)

coordinator_server.o : coordinator_server.hpp coordinator_server.cc
	$(CPP) $(CXXFLAGS) -c $^ $(LIBS)

participant_server.o : participant_server.hpp participant_server.cc
	$(CPP) $(CXXFLAGS) -c $^ $(LIBS)

Command_handle.o : Command_handle.hpp Command_handle.cc
	$(CPP) $(CXXFLAGS) -c $^ $(LIBS)

clean:
	rm kvstore2pcsystem
	rm *.conf