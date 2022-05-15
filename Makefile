fst:
	g++ -o fst.out -O2 -g fst.h fst.cpp main.cpp
clean:
	rm -rf *out *gch
