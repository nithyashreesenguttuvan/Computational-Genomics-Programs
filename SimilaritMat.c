#include "Header.h"

void longestcommonsubstring(Node* root, int* start1, int* start2, int* end1, int* end2, int* len) {
    int i = 0;
    int color = num_sequences + 1;
    if (root->id > sequenceLength) { // This node is an internal Node
        if (root->color == color) {
            if (*len < root->depth) {
                *len = root->depth;
                while (i < alphabetLength) {
                    if (root->children[i] != NULL && root->children[i]->color == 0) {
                        *start1 = root->children[i]->suffixHead - *len;
                        *end1 = root->suffixTail - *len + 1;
                    }
                    if (root->children[i] != NULL && root->children[i]->color == 1) {
                        *start2 = root->children[i]->suffixHead - *len;
                        *end2 = root->suffixTail - *len + 1;
                    }
                    i++;
                }
            }
        }
    }
    for (int j = 0; j < alphabetLength; j++) {
        if (root->children[j] != NULL) {
            longestcommonsubstring(root->children[i], start1, start2, end1, end2, len);
        }
    }
}

void dfs_h(Node* node, int num_strings) {
    //printf("Coloring\n");
    if (node->numberChildren == 0) {
        // node is a leaf
        int color = node->suffixHead % num_strings;
        node->color = color;
    } else {
        // node is an internal node
        int color = node->color;
        bool same_color = true;
        for (int i = 0; i < node->numberChildren; i++) {
            dfs_h(node->children[i], num_strings);
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

// Extracting and Reversing the prefixes
char* reverse_string(char* str, int len) {
    //printf("\nReverse String entry %s\n",str);
    //char* rev_str = (char*) malloc((len+1) * sizeof(char)); // allocate memory for reversed string
    int act_len = strlen(str);
    char *rev_str = malloc(act_len * sizeof(char*));

    if (rev_str == NULL) {
        printf("Memory allocation failed.\n");
        return NULL; // Return with an error code
    }

    int j = 0;
    // Copy characters in reverse order
    for (int i = act_len - 1; i >= 0 && i< act_len; i--) {
        if(str[i]!= '$'){  
            rev_str[j++] = str[i];
        }
    }

    // Null-terminate the reverse string
    rev_str[j] = '\0';

    //printf("\nReverse string exit %s\n", rev_str);
    return rev_str;
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

// Extracting the suffixes
char* extract_suffix(char* s, int y1) {
    //printf("Extract Suffixx\n");
    int n = strlen(s);
    char* suffix = malloc(sizeof(char) * (n - y1 + 1));
    if (!suffix) {
        fprintf(stderr, "Error: unable to allocate memory for suffix\n");
        exit(1);
    }
    int i;
    for (i = y1; i < n; i++) {
        suffix[i - y1] = s[i];
    }
    suffix[n - y1] = '\0';
    return suffix;
}

// Function to compute similarity matrix
void compute_similarity_matrix(char* sequences[], int num_sequences) {
    //printf("COmpute Similairty Matrix\n");
    Node *tree;
    char* curr_sequences[2];
    int similarity_matrix[num_sequences][num_sequences];
    struct timeval tstart, tstop;

    // Loop over all pairs of sequences
    for (int i = 0; i < num_sequences; i++) {
        //printf("\n compute i = %d\t",i);
        for (int j = i+1; j < num_sequences; j++) {
            //printf("compute j= %d\n",j);
            // Storing the sequences
            strcpy(curr_sequences[0],sequences[i]);
            strcpy(curr_sequences[1],sequences[j]);

            // Build GST for {s_i, s_j}
            gettimeofday(&tstart, NULL);
            tree = buildTree(curr_sequences, 2);
            gettimeofday(&tstop, NULL);
            //printf("Suffix Tree Time %d and %d : %f (ms)\n", i, j, diff_time(&tstart, &tstop));
            //printf("built tree\n");

            dfs_h(tree, 2);
            //printf("Colored\n");

            // Find longest common substring and its coordinates
            int x1, x2, y1, y2, b;
            longestcommonsubstring(tree, &x1, &x2, &y1, &y2, &b);
            //printf("The length of longest common substring for sequences %d and %d are %d\n", i, j, b);

            // Extract prefixes and reverse them
            char* s_i_rev = reverse_string(sequences[i], x1);
            char* s_j_rev = reverse_string(sequences[j], x2);

            // Compute global alignment between reversed prefixes
            gettimeofday(&tstart, NULL);
            int a = align(s_i_rev, s_j_rev);
            gettimeofday(&tstop, NULL);

            int tempt = diff_time(&tstart, &tstop);

            // Free memory for reversed prefixes
            free(s_i_rev);
            free(s_j_rev);

            // Extract suffixes
            char* s_i_fwd = extract_suffix(sequences[i], y1);
            char* s_j_fwd = extract_suffix(sequences[j], y2);

            // Compute global alignment between suffixes
            gettimeofday(&tstart, NULL);
            int c = align(s_i_fwd, s_j_fwd);
            gettimeofday(&tstop, NULL);

            //printf("Alignment Time: %d",tempt + diff_time(&tstart, &tstop));
            // Free memory for suffixes
            free(s_i_fwd);
            free(s_j_fwd);

            // Compute similarity value and set in similarity matrix
            int similarity_value = a + b + c;
            similarity_matrix[i][j] = similarity_value;
            similarity_matrix[j][i] = similarity_value;
            //printf("The similarity of %d and %d is %d ", i, j, similarity_matrix[i][j]);
            
        }
    }
}