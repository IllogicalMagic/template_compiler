CC=$(CXX)
CXXFLAGS=-std=c++17 -O2

ThreeAddress: ThreeAddress.cpp

clean:
	rm -rf *~ *.o *.s a.out ThreeAddress
