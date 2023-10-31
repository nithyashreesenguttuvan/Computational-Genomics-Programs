#include "Header.h"

// helper function to return max of 3 integers
int max(int a, int b, int c){
	//printf("Max\n");
	return (a > b ? (a > c ? a : c) : (b > c ? b : c));
}

// helper function to reverse a string
char* stringReverse(char *str){
	//printf("String reverse\n");
	char *start = str;
	char *end = start+strlen(str)-1;

	while(start<end)
	{
		char temp = *end;
		*end = *start;
		*start=temp;
		start++;
		end--;
	}
	return str;
}

int chooseDirection(int i, int j){
	//printf("ChooseD\n");
	return max(Matrix[i][j].s, Matrix[i][j].d, Matrix[i][j].i);
}

// calculate the substitution score
int substitution(char *s1, char *s2, int i, int j){
	//printf("Substituition\n");
	//printf("%d\t%c\t%d\t%c\n",i - 1, s1[i - 1], j - 1, s2[j - 1]);
	if (s1[i - 1] != s2[j - 1]) 
		return chooseDirection(i-1,j-1) + mi;
	else
		return chooseDirection(i-1,j-1) + ma;	
}

// dynamically allocate the memory for DP_cell matrix
int allocate(int m, int n){
	//printf("Allocate\n");
	int i;	
	DP_cell **mat = (DP_cell **)malloc(m * sizeof(DP_cell *));
	if (mat == 0){
		printf("Error allocating memory");
		return -1;
	}
	for (i = 0; i < m; i++){
		mat[i] = (DP_cell *)malloc(n * sizeof(DP_cell));
		if(mat[i] == NULL){
			printf("Error allocating memory");
			return -1;
		}
	}
	Matrix = mat;
	return 0;
}

// initialise row 0 and col 0 of matrix
void initialiseMatrix(int m, int n){
	//printf("Initalize\n");
	int i, j;

	Matrix[0][0].s = 0;
	Matrix[0][0].d = 0;
	Matrix[0][0].i = 0;
	
	for (i = 1; i <= m; i++){
			Matrix[i][0].s = INT_MIN - (h + (2 * ((m > n) ? m : n) * g));
			Matrix[i][0].d = h + (g * (i));
			Matrix[i][0].i = INT_MIN - (h + (2 * ((m > n) ? m : n) * g));
	}
	for (j = 1; j <= n; j++){
			Matrix[0][j].s = INT_MIN - (h + (2 * ((m > n) ? m : n) * g));
			Matrix[0][j].d = INT_MIN - (h + (2 * ((m > n) ? m : n) * g));
			Matrix[0][j].i = h + (g * (j));
	}
	return;
}

int traceBack(char *seq1, char *seq2, int maxRowIndex, int maxColIndex) {
	//printf("Tracing back\n");
    int i = maxRowIndex;
    int j = maxColIndex;
    int numMatches = 0;

    while (i > 0 && j > 0) {
        if (Matrix[i][j].s == Matrix[i - 1][j - 1].s + substitution(seq1, seq2, i, j)) {
            // match or substitution
            if (seq1[i - 1] == seq2[j - 1]) {
                numMatches++;
            }
            i--;
            j--;
        } else if (Matrix[i][j].s == Matrix[i][j - 1].i + h + g) {
            // insertion
            j--;
        } else {
            // deletion
            i--;
        }
    }

    return numMatches;
}


// alignment algo for the 2 sequences
int align(char *seq1, char *seq2){
	//printf("Aligning\n");
	int m = strlen(seq1);
	int n = strlen(seq2);
	int i, j;
	int maxRowIndex = 0 , maxColIndex = 0;
	int matches = 0;

	// allocate the matrix
	if (allocate(m+1, n+1) < 0)
		return -1;		

	// Initialise Row 1 and Column 1
	initialiseMatrix(m, n);

	for (i = 1; i <= m; i++){
		for (j = 1; j <= n; j++){
				// substitution
				Matrix[i][j].s = substitution(seq1, seq2, i, j);				
				// deletion
				Matrix[i][j].d = max(Matrix[i-1][j].s + h + g, Matrix[i-1][j].i + h + g, Matrix[i-1][j].d + g);
				// insertion
				Matrix[i][j].i = max(Matrix[i][j-1].s + h + g, Matrix[i][j-1].i + g, Matrix[i][j-1].d + h + g);
		}
	}

	
	// Contribution of Waterman to this field is the ZERO!!
	for (i = 1; i <= m; i++){
		for (j = 1; j <= n; j++){
			// Storing the index where the max score was encountered
			if (chooseDirection(i,j) > chooseDirection(maxRowIndex, maxColIndex)){
				maxRowIndex = i;
				maxColIndex = j;
			}
		}
	}
	int maxscore = max(Matrix[maxRowIndex][maxColIndex].s, Matrix[maxRowIndex][maxColIndex].d, Matrix[maxRowIndex][maxColIndex].i);
	matches = traceBack(seq1, seq2, maxRowIndex, maxColIndex);

    return matches;
}
