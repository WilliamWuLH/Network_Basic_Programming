CPP = g++
CXXFLAGS = -std=c++11 -O2 -g
CXXFLAGS += -Wall -Wextra
OBJS = main.o Thread_Pool.o http_handle.o
LIBS = -pthread

httpserver : $(OBJS)
	$(CPP) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)
	rm *.o *.hpp.gch

main.o : main.cc
	$(CPP) $(CXXFLAGS) -c $^ $(LIBS)

Thread_Pool.o : Thread_Pool.hpp Thread_Pool.cc
	$(CPP) $(CXXFLAGS) -c $^ $(LIBS)

http_handle.o : http_handle.hpp http_handle.cc
	$(CPP) $(CXXFLAGS) -c $^ $(LIBS)

clean:
	rm httpserver