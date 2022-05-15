#include <string>

#include "fst.h"

std::string keys[] = {"mop"};
int values[] = {100};


int main() {
    FST *fst = (FST *)calloc(1, sizeof(FST));
    fst_add(fst, keys[0], values[0]);
    fst_print(fst);
    return 0;
}
