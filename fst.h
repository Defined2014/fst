#include <string>
#include <vector>
#include <map>

typedef struct Node Node;
typedef struct Arc Arc;
typedef struct FST FST;

struct FST {
    int lastFrozenNode;
    std::vector <char> current;
    std::vector <Node *> frontier;

    std::map <std::string, int> nodeHash;
};

FST *fst_create();

void fst_add(FST *fst, std::string key, int value);
int fst_get(FST *fst, std::string key);

void fst_print_current(FST *fst);
void fst_print_frontier(FST *fst);
