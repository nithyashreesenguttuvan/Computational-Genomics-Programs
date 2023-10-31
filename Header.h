#ifndef header
#define header

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

typedef struct Node{		
	int id;
	int depth;
	int numberChildren;
	int suffixHead;
	int suffixTail;
	int color;
	struct Node *parent;
	struct Node *suffixLink;
	struct Node **children;
}Node;

typedef struct DP_cell{
	int s; int i; int d;
} DP_cell;

// Global varaiables declaration
DP_cell **Matrix;                       // stores the dynamically computed matrix
int SW = 1;                             // Smith-Waterman or Needleman-Wunsch
int ma=1, mi= -2, h = -5, g = -1;		// scoring parameters
int maxRowIndex = 0 , maxColIndex = 0;  // stores index of max value to back track from
char *s1;                               // stores sequence 1
char *s2;                               // stores sequence 2

int alphabetLength = 0;					// size of alphabet 
int sequenceLength = 0;					// length of sequence 
int internalNodes = 0;					// # internal nodes
int leafs = 1;							// # leaves
Node *maxDepthNode = NULL;				// deepest internal node
int maxDepth = 0;						// depth of the deepest internal node
int stringDepth = 0;					// running sum of depth of each internal node
char *alphabet;							// alphabet
char *sequence;							// input sequence
char* sequences;
char sequenceName[16];
int num_sequences;
int numberofcolors;						// number of colors for the coloring scheme
int MAX_SEQ_LENGTH=1000000;


Node *tree;								// global suffix tree for reference genome rooted at tree
int *A = NULL;							// global A[] for PrepareST()
Node** alphas;
// Global funtion prototypes
Node *buildTree(char *sequences[], int numSequences);		// SuffixTree Construction 
void printFingerprints(Node* root, char* sequences[], int numStrings);  // Generating DNA Fingerprints
void compute_similarity_matrix(char* sequences[], int num_sequences);  //Generate Similarity Matrix
void dfs(Node* node, int num_strings);						// DFS function to color
int align(char *seq1, char *seq2);			//Alignment

#endif