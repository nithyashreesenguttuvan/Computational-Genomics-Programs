#ifndef header
#define header

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Node Structure 
typedef struct Node{		
	int id;
	int depth;
	int numberChildren;
	int suffixHead;
	int suffixTail;
	struct Node *parent;
	struct Node *suffixLink;
	struct Node **children;
}Node;

// Global varaiables declaration
int alphabetLength = 0;					// size of alphabet 
int sequenceLength = 0;					// length of sequence 
int internalNodes = 0;					// # internal nodes
int leafs = 1;							// # leaves
Node *maxDepthNode = NULL;				// deepest internal node
int maxDepth = 0;						// depth of the deepest internal node
int stringDepth = 0;					// running sum of depth of each internal node
char *alphabet;							// alphabet
char *sequence;							// input sequence
char sequenceName[16];					// input sequence name

// Global Function prototypes
Node *buildTree();						// Construction of Suffix Tree
int dfs( Node *node );					// pre-order dfs
int display_children( Node *node );		// enumerate children from left to right 
int bwt( Node *node , FILE *ptr);		// Burrows Wheeler Transform

#endif	