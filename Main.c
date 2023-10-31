#include "Header.h"
#include "SuffixTree.c"
#include "Dnafingerprint.c"
#include "SimilaritMat.c"
#include "Alignment.c"

#include <errno.h>
#include <sys/stat.h>
#include <windows.h>
#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#include <time.h>

// Read alphabet file
int read_file_alpha(char alphaName[]){
    
    struct stat alpha_struct;
    stat(alphaName, &alpha_struct);
    int alpha_size = alpha_struct.st_size;

    FILE *alphaFile;

    //Checking if alphabet file is empty
    if (!(alphaFile = fopen(alphaName, "r"))){
     printf("Error while opening alphabet file.\n");
     return -1;
    }

    // check if memory is allocated
    alphabet = (char*)malloc(alpha_size);
    if (alphabet == NULL) {
        printf("\nERROR: Cannot allocate memory for alphabet.\n");
        return -1;
    }

    int alpha = alpha_size;
    char seqChar = '\0';

     // Read  alphabet
    do {
        seqChar = fgetc(alphaFile);
        if (seqChar != ' ' && seqChar != '\n' && !feof(alphaFile)) {
            alphabet[alphabetLength] = seqChar;
            ++alphabetLength;
        }
    } while (alpha > 0 && !feof(alphaFile));
    alphabet[alphabetLength] = '\0';

    fclose(alphaFile);
    
    return (0);

}


// Read input file 
char *read_files(char seqName[]) {
    struct stat seq_struct;
    stat(seqName, &seq_struct);
    int seq_size = seq_struct.st_size;
    
    FILE *seqFile;
    
    // Checking if Input file is empty
    if (!(seqFile = fopen(seqName,"r"))){
     printf("Error while opening input file.\n");
     return NULL;
    }
    
    // check if memory is allocated
    sequence = (char*)malloc(seq_size);
    if (sequence == NULL) {
        printf("\nERROR: Cannot allocate memory for sequence.\n");
        return NULL;
    }
    
    int i = 0;
    char seqChar = '\0';
    char sequenceName[16];
    int sequenceLength = 0;

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
   
    fclose(seqFile);
    return sequence;
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

int main(){
    
    Node *tree;
    char path[100];
    struct timeval tstart, tstop;

    //Getting input fasta files
    printf("Creating Generalized Suffix Tree............\n");
    printf("Enter the alphabet file name:\n");
    scanf("%s",path);
    read_file_alpha(path);

    int num_sequences; 
    printf("Enter the number of the FASTA files:\n");
    scanf("%d",&num_sequences);

    char **sequences = malloc(num_sequences * sizeof(char*));
    char filenames[num_sequences][100];

    printf("Enter the names of the FASTA files:\n");
    for (int i = 0; i < num_sequences; i++) {
        scanf("%s", filenames[i]);
    }
    
    // read sequences from files
    for(int i=0; i<num_sequences; i++){
    // read sequence from file
        char *sequence = read_files(filenames[i]);
        if(sequence == NULL){
            printf("Error: could not read sequence from file %s\n", filenames[i]);
            return 1;
        }
        // allocate memory for sequence
        sequences[i] = malloc(strlen(sequence) + 1);
        strcpy(sequences[i], sequence);
        //printf("%s\n",sequences[i]);
    }
    
    printf("Building Tree.....\n");
    gettimeofday(&tstart, NULL);
    tree = buildTree(sequences, num_sequences);
    gettimeofday(&tstop, NULL);
    //printf("Suffix Tree Construction Time: %f (ms)\n", diff_time(&tstart, &tstop));

    printf("Coloring Tree.....\n");
    dfs(tree, num_sequences);

    printf("Finding the DNA Fingerprint.........\n");
    Node** alphas = malloc((num_sequences + 1) * sizeof(Node*));  
    for (int i = 0; i <= num_sequences; i++) {
        alphas[i] = NULL;
    }
    alphaLocator(tree, num_sequences);
    
    gettimeofday(&tstart, NULL);
    for (int i = 0; i < num_sequences; i++) {
        char i_s[10];
        sprintf(i_s, "%d", i+1);
        char outputFile[100] = "Input";  
        strcat(outputFile,i_s);
        char *x = "_fingerprint.txt";
        FILE *ptr = fopen(strcat(outputFile, x), "w");
	    
        fingerprint(i, alphas, sequence);

        fclose(ptr);
    }
    gettimeofday(&tstop, NULL);
    //printf("Fingerprint Identification Time: %f (ms)\n", diff_time(&tstart, &tstop));

    free(tree);
    free(alphas);

    // Compute similarity matrix
    printf("Similarity Matrix..........\n");
    gettimeofday(&tstart, NULL);
    compute_similarity_matrix(sequences, num_sequences);
    gettimeofday(&tstop, NULL);
    //printf("Similarity Matrix Execution Time: %f (ms)\n", diff_time(&tstart, &tstop));

    // Free memory
    for (int i = 0; i < num_sequences; i++) {
        free(sequences[i]);
    }
    
    deallocate_memory(tree);
    return (0);

}
