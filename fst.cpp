#include <malloc.h>
#include <stdlib.h>

#include <stack>
#include <vector>

#include "fst.h"

#define BIT_FINAL_ARC       (1 << 0)
#define BIT_LAST_ARC        (1 << 1)
#define BIT_TARGET_NEXT     (1 << 2)
#define BIT_STOP_NODE       (1 << 3)
#define BIT_ARC_HAS_OUTPUT  (1 << 4)

struct Arc {
    char label;
    bool isFinal;
    Node *targetNode;
    int targetIndex;
    int output;
    int nextFinalOutput;
};

struct Node {
    int numArcs;
    std::vector<Arc *> arcs;
};

Arc* fst_create_arc(std::string key, int index) {
    Arc *arc = (Arc *)calloc(1, sizeof(Arc));
    arc->label = key[index];
    if (index == key.size() - 1) {
        arc->isFinal = true;
    }

    return arc;
}

Node* fst_create_node() {
    Node *n = (Node *)calloc(1, sizeof(Node));
    return n;
}

std::string fst_cacl_hash(Node *node, std::vector <int> &t) {
    std::string hash;
    char tmp[30];
    for (int i = t.size() - 1; i >= 0; i--) {
        sprintf(tmp, "%d", t[i]); 
        hash += tmp;
        sprintf(tmp, "%d", node->arcs[i]->targetIndex);
        hash += tmp;
    }
    return hash;
}

int fst_hash_find(FST *fst, Node *node, std::vector <int> t) {
    std::string hashKey = fst_cacl_hash(node, t);
    std::map<std::string, int>::iterator iter = fst->nodeHash.find(hashKey);
    if (iter != fst->nodeHash.end()) {
        return iter->second;
    } else {
        return -1;
    }
}


void fst_hash_insert(FST *fst, Node *node, std::vector <int> t, int index) {
    fst->nodeHash[fst_cacl_hash(node, t)] = index;
}

void fst_merge_to_current(FST *fst, std::vector <int> &t) {
    for (int i = t.size() - 1; i >= 0; i--) {
        int tmp = t[i];
        if ((tmp & 0xff000000) != 0) {
            fst->current.push_back((tmp & 0xff000000) >> 24);
        }
        if ((tmp & 0x00ff0000) != 0) {
            fst->current.push_back((tmp & 0x00ff0000) >> 16);
        }
        if ((tmp & 0x0000ff00) != 0) {
            fst->current.push_back((tmp & 0x0000ff00) >> 8);
        }
        if ((tmp & 0x000000ff) != 0) {
            fst->current.push_back((tmp & 0x000000ff));
        }
    }
}

void fst_frozen_node(FST *fst, Node *node, Node *prevNode) {
    if (node->numArcs == 0) {
        fst->lastFrozenNode = -1;
        prevNode->arcs[prevNode->numArcs - 1]->targetIndex = fst->lastFrozenNode;
    } else {
        std::vector <int> t;
        for (int i = 0; i < node->numArcs; i++) {
            int flag = 0, tmp = 0;
            Arc *arc = node->arcs[i];
            if (arc->isFinal) {
                flag |= BIT_FINAL_ARC;
            }
            if (i == node->numArcs - 1) {
                flag |= BIT_LAST_ARC;
            }
            if (arc->targetIndex == fst->lastFrozenNode) {
                flag |= BIT_TARGET_NEXT;
            } else if (arc->targetIndex != -1) {
                tmp |= ((arc->targetIndex & 0xff) << 24);
            }
            if (arc->targetIndex == -1) {
                flag |= BIT_STOP_NODE;
            }
            if (arc->output != 0) {
                flag |= BIT_ARC_HAS_OUTPUT;
                tmp |= ((arc->output & 0xff) << 16);
            }
            tmp |= ((int)(arc->label) << 8);
            tmp |= (flag & 0xff);
            t.push_back(tmp);
        }
        int index = fst_hash_find(fst, node, t);
        if (index != -1) {
            fst->lastFrozenNode = -2;
            prevNode->arcs[prevNode->numArcs - 1]->targetIndex = index;
        } else {
            fst_merge_to_current(fst, t);
            fst->lastFrozenNode = fst->current.size() - 1;
            fst_hash_insert(fst, node, t, fst->current.size() - 1);
            prevNode->arcs[prevNode->numArcs - 1]->targetIndex = fst->lastFrozenNode;
        }
    }
}

void fst_adjust_values(FST *fst, int preFixLen, int value) {
    Node *node = fst->frontier[0];
    Arc *arc = node->arcs[node->numArcs - 1];

    int remainValue = value - arc->output;

    if (remainValue == 0) {
        return;
    } else if (remainValue < 0) {
        arc->output = value;
        for (int i = 1; i <= preFixLen; i++) {
            node = fst->frontier[i];
            for (int j = 0; j < node->numArcs - 1; j++) {
                arc = node->arcs[j];
                arc->output -= remainValue;
            }
        }
    } else {
        node = fst->frontier[preFixLen];
        node->arcs[node->numArcs - 1]->output = remainValue;
    }
}

void fst_add(FST *fst, std::string key, int value) {
    int i, preFixLen, preFixValue = 0;

    // find prefix len
    for (i = 0; i < key.size(); i++) {
        if (i >= fst->frontier.size()) {
            break;
        }
        Node *node = fst->frontier[i];
        Arc *arc = node->arcs[node->numArcs - 1];
        if (key[i] != arc->label) {
            break;
        }
        preFixValue += arc->output;
    }
    preFixLen = i;

    // frozen nodes
    for (i = fst->frontier.size() - 1; i > preFixLen; i--) {
        fst_frozen_node(fst, fst->frontier[i], fst->frontier[i - 1]);
        free(fst->frontier[i]);
        fst->frontier.pop_back();
    }

    if (fst->frontier.size() == 0) {
        fst->frontier.push_back(fst_create_node());
    }

    for (i = preFixLen; i < key.size(); i++) {
        Arc *arc = fst_create_arc(key, i);
        arc->targetNode = fst_create_node();
        fst->frontier.push_back(arc->targetNode);

        fst->frontier[i]->numArcs++;
        fst->frontier[i]->arcs.push_back(arc);
    }

    fst_adjust_values(fst, preFixLen, value);
}

int fst_get(FST *fst, std::string key) {
    return -1;
}

void fst_print_frontier(FST *fst) {
   for (int i = 0; i < fst->frontier.size(); i++) {
       printf("Node: %p, num_arcs: %ld, arcs: \n", fst->frontier[i], fst->frontier[i]->arcs.size());
       for (int j = 0; j < fst->frontier[i]->arcs.size(); j++) {
           Arc *arc = fst->frontier[i]->arcs[j];
           if (j == fst->frontier[i]->arcs.size() - 1) {
               printf("\tArc, output: %d, label: %c, isFinal: %d, targetNode: %p\n", arc->output, arc->label, arc->isFinal, arc->targetNode);
           } else {
               printf("\tArc, output: %d, label: %c, isFinal: %d, targetIndex: %d\n", arc->output, arc->label, arc->isFinal, arc->targetIndex);
           }
       }
   }
}

void fst_print_current(FST *fst) {
    printf("current: ");
    for (int i = 0; i < fst->current.size(); i++) {
        printf("%u ", fst->current[i]);
    }
    printf("\n");
}
