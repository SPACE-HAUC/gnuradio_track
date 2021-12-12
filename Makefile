CXX = g++
CC = gcc
COBJS = 
CPPOBJS = track.o
EDCFLAGS := -std=gnu11 -O2 $(CFLAGS)
EDCXXFLAGS = -I sgp4-lib/include -Wall -pthread $(CXXFLAGS)
EDLDFLAGS := -lpthread -lm $(LDFLAGS)
TARGET = track.out
LIBSGP4 = sgp4-lib/libsgp4.a

all: $(COBJS) $(CPPOBJS) $(LIBSGP4)
	$(CXX) $(EDCXXFLAGS) $(COBJS) $(CPPOBJS) $(LIBSGP4) -o $(TARGET) $(EDLDFLAGS)
	sudo ./$(TARGET) objects.tle

%.o: %.cpp
	$(CXX) $(EDCXXFLAGS) -o $@ -c $<

%.o: %.cc
	$(CXX) $(EDCXXFLAGS) -o $@ -c $<

%.o: %.c
	$(CC) $(EDCFLAGS) -o $@ -c $<

$(LIBSGP4):
	cd sgp4-lib && make && cd ..

.PHONY: clean

clean:
	$(RM) *.out
	$(RM) *.o
	$(RM) src/*.o

spotless: clean
	$(RM) $(COBJS)
	$(RM) $(CPPOBJS)
	cd sgp4-lib && make spotless && cd ..

