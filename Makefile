OBJS = Parser.o CUDPFeed.o CBroadCastHandler.o 

all: feed

feed: main.cc libparser.a
	g++ -W -O2 -std=c++17 -o $@ $^

%.o : %.cc
	g++ -W -O2 -c -std=c++17 -o $@ $<

libparser.a: $(OBJS)
	ar rcs libparser.a $^

clean:
	rm -f *.o *.a feed
