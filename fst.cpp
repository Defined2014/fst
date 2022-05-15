#include <malloc.h>
#include <vector>

#include "fst.h"

struct Arc {
    char label;
    bool isFinal;
    Node *targetNode;
    int output;
    int nextFinalOutput;
};

struct Node {
    int numArcs;
    std::vector<Arc *> arcs;
};

Arc* createArc(std::string key, int index, int value) {
    Arc *arc = (Arc *)calloc(1, sizeof(Arc));
    arc->label = key[index];
    if (index == key.size() - 1) {
        arc->isFinal = true;
    }
    arc->output = value;

    return arc;
}

Node* createNode() {
    Node *n = (Node *)calloc(1, sizeof(Node));
    return n;
}

void fst_add(FST *fst, std::string key, int value) {
    int preFixLen = -1;
    int remainedValue = value;

    if (fst->frontier.size() == 0) {
        fst->frontier.push_back(createNode());
    }

    for (int i = preFixLen + 1; i < key.size(); i++) {
        Arc *arc = createArc(key, i, remainedValue);
        remainedValue = 0;
        arc->targetNode = createNode();
        fst->frontier.push_back(arc->targetNode);

        fst->frontier[i]->numArcs++;
        fst->frontier[i]->arcs.push_back(arc);
    }
}

int fst_get(FST *fst, std::string key) {
    return -1;
}

void fst_print(FST *fst) {
   for (int i = 0; i < fst->frontier.size(); i++) {
       printf("Node: %p, num_arcs: %ld, arcs: \n", fst->frontier[i], fst->frontier[i]->arcs.size());
       for (int j = 0; j < fst->frontier[i]->arcs.size(); j++) {
           Arc *arc = fst->frontier[i]->arcs[j];
           printf("\tArc, output: %d, label: %c, isFinal: %d, targetNode: %p\n", arc->output, arc->label, arc->isFinal, arc->targetNode);
       }
   }
}
