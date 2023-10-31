#include "API_Impl.c"
#include "API_Header.h"
#include <errno.h>
#include <sys/stat.h>
#include <windows.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Get ST construction space in KB
int get_memory_usage_kb(long* vmrss_kb, long* vmsize_kb)
{
    // Get the current process status file from the proc filesystem 
    FILE* procfile = fopen("/proc/self/status", "r");

    long to_read = 8192;
    char buffer[to_read];
    int read = fread(buffer, sizeof(char), to_read, procfile);
    fclose(procfile);

    short found_vmrss = 0;
    short found_vmsize = 0;
    char* search_result;

    // Look through proc status contents line by line 
    char delims[] = "\n";
    char* line = strtok(buffer, delims);

    while (line != NULL && (found_vmrss == 0 || found_vmsize == 0) )
    {
        search_result = strstr(line, "VmRSS:");
        if (search_result != NULL)
        {
            sscanf(line, "%*s %ld", vmrss_kb);
            found_vmrss = 1;
        }

        search_result = strstr(line, "VmSize:");
        if (search_result != NULL)
        {
            sscanf(line, "%*s %ld", vmsize_kb);
            found_vmsize = 1;
        }

        line = strtok(NULL, delims);
    }

    return (found_vmrss == 1 && found_vmsize == 1) ? 0 : 1;
}


// Read input file and alphabet file
int read_files(const char ** argv) {
    char seqName[64];
    char alphaName[64];
    struct stat seq_struct;
    struct stat alpha_struct;
    stat(argv[1], &seq_struct);
    stat(argv[2], &alpha_struct);
    int seq_size = seq_struct.st_size;
    int alpha_size = alpha_struct.st_size;
    
    strcpy(seqName, argv[1]);
    strcpy(alphaName, argv[2]);
    
    FILE *seqFile;
    FILE *alphaFile;
    
    // Checking if Input file is empty
    if (!(seqFile = fopen(seqName,"r"))){
     printf("Error while opening input file.\n");
     return -1;
    }
    
    //Checking if alphabet file is empty
    if (!(alphaFile = fopen(alphaName, "r"))){
     printf("Error while opening alphabet file.\n");
     return -1;
    }
   
    // check if memory is allocated
    sequence = (char*)malloc(seq_size);
    if (sequence == NULL) {
        printf("\nERROR: Cannot allocate memory for sequence.\n");
        return -1;
    }
    
    alphabet = (char*)malloc(alpha_size);
    if (alphabet == NULL) {
        printf("\nERROR: Cannot allocate memory for alphabet.\n");
        return -1;
    }

    int i = 0;

    
    int alpha = alpha_size;
    char seqChar = '\0';
    
    // Read the input file
    do {
        seqChar = fgetc(seqFile);
        if (seqChar == '>') {
            seqChar = fgetc(seqFile);
            while (seqChar != '\n'  && seqChar == '|') {
                if (i < 15) {
                    sequenceName[i] = seqChar;
                    ++i;
                }
                seqChar = fgetc(seqFile);
            }
            sequenceName[i] = '\0';
        }
       
    } while (seqChar !='\n' && !feof(seqFile));
    
    // Read input Sequence
    do {
        seqChar = fgetc(seqFile);
        while (!feof(seqFile)){
                if (seqChar != ' ' && seqChar != '\n') {
                sequence[sequenceLength] = seqChar;
                ++sequenceLength;
            }
            seqChar = fgetc(seqFile);
        }
    } while (!feof(seqFile));
   
    sequence[sequenceLength] = '$';
    sequenceLength++;
    sequence[sequenceLength] = '\0';
    
    // Read  alphabet
    do {
        seqChar = fgetc(alphaFile);
        if (seqChar != ' ' && seqChar != '\n' && !feof(alphaFile)) {
            alphabet[alphabetLength] = seqChar;
            ++alphabetLength;
        }
    } while (alpha > 0 && !feof(alphaFile));
    alphabet[alphabetLength] = '\0';
   
    fclose(seqFile);
    fclose(alphaFile);
    
    return (0);
}


// helper function to convert time into double
double double_time(struct timeval *atime)
{
	return ((atime->tv_sec + (atime->tv_usec/1000000.0)) * 1000.0);
}


// helper function to find difference between two timestamps
double diff_time(struct timeval *tstart, struct timeval *tstop)
{
	return (double_time(tstop) - double_time(tstart));
}


// free up allocated memory
void deallocate_memory(Node *node) {
	free(sequence);
	free(alphabet);
	int i;
	if (node){
		for(i=0; i < node->numberChildren; i++){
			deallocate_memory(node->children[i]);
		}
		free(node->children);
		free(node);
	}
}

// find exact matching repeat sequence
void exact_matching_repeat() {
	int i;
	Node *n = maxDepthNode -> children[0];
	printf("Exact Matching Repeat: ");
    // exact matching repeat is the path label of deepest internal node
	for (i = n -> id - 1; i < maxDepthNode -> depth + n -> id - 1; i++){
		printf("%c", sequence[i]);
	}

    printf("\n");
    printf("Length of longest exact matching repeat: %d",maxDepth);
	printf("\n");
	printf("Exact Matching Repeat positions: ");
    // the node ID's of the leaf's under maxDepth node
	for (i = 0; i < maxDepthNode -> numberChildren; i++){
		printf("%d, ", maxDepthNode -> children[i] -> id);
	}
	printf("\n");
}


// print the statistics asked in question
void print_statistics(Node *tree, char *outputFile) {
    
	struct timeval tstart, tstop;
	long vmrss, vmsize, startMem, endMem;
	char *x = "_BWT.txt";

	get_memory_usage_kb(&vmrss, &vmsize);
    startMem = vmrss;
	gettimeofday(&tstart, NULL);
	tree = buildTree();		// build ST
	gettimeofday(&tstop, NULL);
    get_memory_usage_kb(&vmrss, &vmsize);
    endMem = vmrss;
	printf("\nSUFFIX TREE CONSTRUCTION STATISTICS:\n");
	printf("\nInput size: %ld (bytes)\n", sizeof(char *) * strlen(sequence));
	printf("Suffix Tree Construction Time: %f (ms)\n", diff_time(&tstart, &tstop));
	printf("Suffix Tree Construction Space: %6ld (KB)\n", endMem - startMem);
	printf("Implementation Constant (bytes consumed by code for every input byte): %ld\n", ((endMem - startMem) * 1024) / (sizeof(char *) * strlen(sequence)));
	printf("Number of internal nodes: %d\n", internalNodes + 1);
	printf("Number of leafs: %d\n", leafs - 1);
	printf("Total number of nodes: %d\n", internalNodes + leafs);
	printf("Size of the tree: %ld (bytes)\n", (internalNodes + leafs) * sizeof(tree));
	printf("Average string depth of internal nodes: %d\n", (stringDepth / (internalNodes + 1)));
	printf("String depth of deepest internal node: %d\n", maxDepth);
	exact_matching_repeat();

    FILE *ptr = fopen(strcat(outputFile, x), "w");
	bwt(tree, ptr);
    fclose(ptr);
	
    /*
	printf("Pre-order DFS traversal:\n");
	dfs(tree); 
	printf("\n");
	display_children(tree); // to enumerate children of root from left to right 
	printf("\n"); */
}


int main (int argc, const char *argv[])
{

	Node *tree;
	
	if (argc < 3) {
		printf("\nERROR: Incorrect number of arguments.\n");
		return -1;
	}

	printf("\nThe arguments are:\n\n");
	printf("Input File:\t%s\n", argv[1]);
	printf("Input Alphabet:\t%s\n", argv[2]);
    	
	if (read_files(argv) < 0) 
		return -1;
    
    char *outputFile = malloc(strlen(argv[1]) + 1);
    strcpy(outputFile, argv[1]);
    print_statistics(tree, outputFile);

	deallocate_memory(tree);
	return (0);
}