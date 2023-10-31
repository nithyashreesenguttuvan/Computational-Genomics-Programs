#include "Header.h"

Node** alphas;  // dynamically allocate memory for alphas

// Adding Color values
void dfs(Node* node, int num_strings) {
    if (node->numberChildren == 0) {
        // node is a leaf
        int color = node->suffixHead % num_strings;
        node->color = color;
    } else {
        // node is an internal node
        int color = node->color;
        bool same_color = true;
        for (int i = 0; i < node->numberChildren; i++) {
            dfs(node->children[i], num_strings);
            int child_color = node->children[i]->color;
            if (color == -1) {
                color = child_color;
            } else if (color != child_color) {
                same_color = false;
            }
        }
        if (same_color) {
            node->color = color;
        } else {
            node->color = num_strings + 1;
        }
    }
    //printf("%d %d\n",node->id, node->color);
}

void alphaLocator(Node* startNode, int num_strings) {
    int common_color = num_strings+1;
    if (startNode->color == common_color) {
        for (int i = 1; i < alphabetLength; i++) {
            if (startNode->children[i] != NULL && startNode->children[i]->color != common_color) {
                if (alphas[startNode->children[i]->color]->depth > startNode->depth) {
                    alphas[startNode->children[i]->color] = startNode;
                }
            }
        }
    }
    for (int i = 1; i < alphabetLength; i++) {
        if (startNode->children[i] != NULL) {
            alphaLocator(startNode->children[i], num_strings);
        }
    }
}

void fingerprint(int color, Node **alphas, char *sequence) {
    int c = 0;
    if (alphas[color]->id == -1) {
        printf("Colored string %d's fingerprint:\n", color);
        printf("NONE\n");
        return;
    }
    for (int i = 1; i < alphabetLength; i++) {
        if (alphas[color]->children[i] != NULL && alphas[color]->children[i]->color==color) {
            c = alphas[color]->children[i]->parent->depth + 1;
            break;
        }
    }
    char *alpha = (char*)malloc((alphas[color]->depth - c + 2) * sizeof(char));
    strncpy(alpha, &sequence[c + alphas[color]->suffixHead], alphas[color]->depth - c + 1);
    alpha[alphas[color]->depth - c + 1] = '\0';
    printf("Colored string %d's fingerprint:\n", color);
    printf("%s\n", alpha);
    printf("Fingerprint size: %d\n", (int)strlen(alpha));
    free(alpha);
}
