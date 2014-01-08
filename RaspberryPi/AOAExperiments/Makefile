PREFIX=/usr/local
CFLAGS=-o -I$(PREFIX)/include -I$(PREFIX)/include/libusb-1.0 -DDEBUG
LDFLAGS=-L$(PREFIX)/lib
ADK : ADK.o AndrOA.o
	g++ $(LDFLAGS) -o ADK ADK.o AndrOA.o  -lusb-1.0 -lbcm2835

AndrOA.o : AndrOA.cpp
	g++ -c $(CFLAGS) -o AndrOA.o AndrOA.cpp

ADK.o : ADK.cpp
	g++ -c $(CFLAGS) -o ADK.o ADK.cpp

clean:
	rm -f *.o ADK
