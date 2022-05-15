#include <string>

#include "fst.h"

std::string keys[] = {"mop", "moth"};
int values[] = {100, 91};


int main() {
    FST *fst = (FST *)calloc(1, sizeof(FST));
    fst_add(fst, keys[0], values[0]);
    fst_add(fst, keys[1], values[1]);
    fst_print_frontier(fst);
    return 0;
}
