CC=$(CXX)
CXXFLAGS?=-std=c++17 -O2 -I.

ThreeAddress: ThreeAddress.cpp

Calc: Calc.cpp

clean:
	rm -rf *~ *.o *.s a.out ThreeAddress Calc
