#include <string>

#include "fst.h"

std::string keys[] = {"mop", "moth", "pop", "star", "stop", "top"};
int values[] = {100, 91, 72, 83, 54, 55};


int main() {
    FST *fst = new FST();
    fst_add(fst, keys[0], values[0]);
    fst_add(fst, keys[1], values[1]);
    fst_add(fst, keys[2], values[2]);
    fst_add(fst, keys[3], values[3]);
    fst_add(fst, keys[4], values[4]);
    fst_add(fst, keys[5], values[5]);
    // fst_add(fst, "", 0);
    fst_print_frontier(fst);
    fst_print_current(fst);
    return 0;
}
