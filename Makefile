CXX = g++
CC = gcc
CPPOBJS = src/track.o src/tleobj.o SGP4/libsgp4/CoordGeodetic.o SGP4/libsgp4/CoordTopocentric.o SGP4/libsgp4/DateTime.o SGP4/libsgp4/DecayedException.o SGP4/libsgp4/Eci.o SGP4/libsgp4/Globals.o SGP4/libsgp4/Observer.o SGP4/libsgp4/OrbitalElements.o SGP4/libsgp4/SatelliteException.o SGP4/libsgp4/SGP4.o SGP4/libsgp4/SolarPosition.o SGP4/libsgp4/TimeSpan.o SGP4/libsgp4/Tle.o SGP4/libsgp4/TleException.o SGP4/libsgp4/Util.o SGP4/libsgp4/Vector.o
COBJS = 
EDCFLAGS := -std=gnu11 -O2 $(CFLAGS)
EDCXXFLAGS = -I ./ -I ./include/ -I ./SGP4/libsgp4/ -Wall -pthread $(CXXFLAGS)
EDLDFLAGS := -lpthread -lm $(LDFLAGS)
TARGET = track.out

all: $(COBJS) $(CPPOBJS)
	$(CXX) $(EDCXXFLAGS) $(COBJS) $(CPPOBJS) -o $(TARGET) $(EDLDFLAGS)
	sudo ./$(TARGET) objects.tle

%.o: %.cpp
	$(CXX) $(EDCXXFLAGS) -o $@ -c $<

%.o: %.cc
	$(CXX) $(EDCXXFLAGS) -o $@ -c $<

%.o: %.c
	$(CC) $(EDCFLAGS) -o $@ -c $<

.PHONY: clean

clean:
	$(RM) *.out
	$(RM) *.o
	$(RM) src/*.o

spotless: clean
	$(RM) $(COBJS)
	$(RM) $(CPPOBJS)

