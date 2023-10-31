#include "API_Header.h"

int pretty_dfs = 0;

// make a new node
Node *make_node(int id, Node *parent,int suffixHead,int suffixTail,int stringDepth ){
	
	Node *newnode = (Node *)malloc(sizeof(Node));
	if (newnode == NULL) {
		printf("\nERROR: Could not allocate memory for new node\n");
		exit (1);
	}
	if (suffixHead > suffixTail){
		printf("Error: Could not make Node with ID: %d", id);
		exit (-1);
	}

	newnode->id = id;
	newnode->suffixLink = NULL;
	newnode->parent = parent == NULL ? newnode : parent;
	newnode->suffixHead = suffixHead;
	newnode->suffixTail = suffixTail;
	newnode->numberChildren = 0;
	newnode->children = (Node **)malloc(alphabetLength * sizeof(Node *));
	newnode->depth = stringDepth;
	return (newnode);
}


// add the child to the parent in lexicographic order
void add_child( Node *parent, Node *child ){
	parent -> children[parent -> numberChildren] = child;
	parent -> numberChildren++;
	child -> parent = parent;
	// Bubble sort the children
	Node *temp;
	for(int i=0; i < parent -> numberChildren - 1; ++i ){
		for(int j=0; j < parent -> numberChildren - 1 - i; ++j ){
			if( sequence[parent -> children[j] -> suffixHead] > sequence[parent -> children[j + 1] -> suffixHead]) {
				temp = parent -> children[j + 1];
				parent -> children[j + 1] = parent -> children[j];
				parent -> children[j] = temp;
			}
		}
	}
	return;
}


// find the child that matches the first character of the suffix
Node *match_child( Node *n, int suffix, int *i ){
	Node *currentNode = NULL;
	//at node n check all children's first char
	for (*i = 0; *i < n -> numberChildren && n -> numberChildren > 0; *i+=1){
		currentNode = n -> children[*i];
		if (sequence[currentNode -> suffixHead] == sequence[suffix]){
			return (currentNode);
		}
	}
	// No child exists with the first character of suffix
	return (NULL);
}


// split the current nodes parent edge with the suffix return the leaf
Node *split_edge( Node *current, int head, int tail ){
	int i = 0, ch = 0;
	match_child(current -> parent, head, &ch);
	for( i = current -> suffixHead; i <= tail; ++i ) {  //(int)strlen(suffix); ++i ){
		if( sequence[i] != sequence[head + (i - current -> suffixHead)] ){

			Node *newInternalNode = make_node( sequenceLength + internalNodes + 1, current -> parent, current -> suffixHead, i - 1, current -> parent -> depth + i - current -> suffixHead);
			internalNodes ++;
			stringDepth += newInternalNode -> depth;
			if (newInternalNode -> depth > maxDepth){
				maxDepth = newInternalNode -> depth;
				maxDepthNode = newInternalNode;
			}
			// need to set the current children to the new internalNodes children
			add_child(newInternalNode, current);
			newInternalNode -> parent -> children[ch] = newInternalNode;
			current -> suffixHead = newInternalNode -> suffixTail + 1;
			Node *newLeaf = make_node( leafs, newInternalNode, head + newInternalNode -> suffixTail - newInternalNode -> suffixHead + 1, tail, tail - (head + newInternalNode -> suffixTail - newInternalNode -> suffixHead) + newInternalNode -> depth);
			leafs ++;
			add_child(newInternalNode, newLeaf);
			return (newLeaf);
		}
	}
	perror("ERROR: Couldn't split edge\n");
	exit(1);
	return (current);
}


// smaller hops inside find_path 
Node *hop( Node *n,int head, int tail){ 
	int numChild = 0, i = 0, min;
	Node *a = match_child(n, head, &numChild);
	// if there isnt a child that matches return that node
	if( a == NULL){
		//if ( strlen(beta) == 1 )
		return (n);
	}
	//x = (int)strlen(beta);
	//y = (int)strlen(a->parentEdgeLabel);
	// find minimum of x and y
	min = ((tail - head + 1) < (a -> suffixTail - a -> suffixHead + 1)) ? (tail - head + 1) : (a -> suffixTail - a -> suffixHead + 1);
	for(i = 0; i < min; i++){
		if( sequence[head + i] != sequence[a -> suffixHead + i] ){
			return (n);
		}
	}
	// not an ending leaf and the for loop has gone through the string
	return (hop( a, head + i, tail));
}


// given a suffix, insert it at node v
Node *find_path( Node *v, int head ){
	int numChild, tail = sequenceLength - 1;
	Node *hopped = hop(v, head, sequenceLength - 1);
	int numhops = hopped -> depth - v -> depth;
	Node *child = match_child(hopped, head + numhops, &numChild);
	if ( child == NULL){
		child = make_node(leafs, hopped,	head + numhops, tail, hopped -> depth + (tail - head) + 1);
		add_child(hopped, child);
		leafs ++;
	} else { // a child exists
		child = split_edge(child, head + numhops, tail);
	}
	return child;
}


// hop to v from vPrime
Node *node_hops(Node *vPrime, Node *u, int betaHead, int betaEnd, int suffix){
	int r = 0, childNum = 0, betaLen = (betaEnd - betaHead + 1);
	Node *currNode = vPrime;
	Node *e = NULL, *i = NULL, *v = NULL;

	while(r <= (betaLen)){ 
		e = match_child(currNode, betaHead + r, &childNum);
		if(e){ 
			int edgeLen = (e -> suffixTail - e -> suffixHead + 1);
			if( edgeLen + r > betaLen ){ // beta will get exhausted and hence split edge required in the middle of the edge
				i = split_edge(e, suffix + currNode -> depth, sequenceLength - 1);
				v = i -> parent;
				u -> suffixLink = v;
				return i;
			} else if( edgeLen + r == betaLen ){ // end of beta and end of edge coincides
				v = e;
				u->suffixLink = v;
				int k = u -> depth;
				i = find_path(v, suffix + k -1);
				return i;
			} else { // edge will exhausted and beta will overflow to the next edge 
				r += edgeLen;
				currNode = e;
				continue;
			}
		} else { // this case shouldn't come up as leaf formation is taken care of by split_edge
			printf("ERROR: Don't know why this error occurs");
			exit(-1);
		}
	}
	return i;
}


// insert suffix[i]
Node *insert( int i, Node *root, Node *leaf ){
	Node *u = leaf->parent;
	int Case = -1;

	if( u->suffixLink != NULL )
		if ( u != root )			// case IA
			Case = 0;
		else 						// case IB
			Case = 1;
	else if ( u->parent != root )	// case IIA
			Case = 2;
	else							// case IIB
		Case = 3;

	switch(Case){
		// IA: suffix link for u is known and u is not the root
		case 0:
			{
				int cAlphaLen = u -> depth;
				Node *v = u -> suffixLink;
				return find_path(v, i + cAlphaLen - 1); 
				//}
				break;
			}
		// IB: suffix link for u is known and u is the root
		case 1:
			{
				return find_path(u, i);
				break;
			}
		// IIA: suffix link for u is unknown and u' is not the root
		case 2:
			{
				Node *uPrime = u -> parent;
				int betaHead = u -> suffixHead, betaTail = u -> suffixTail; //betaHead to betaTail contains Beta
				Node *vPrime = uPrime -> suffixLink;
				return node_hops(vPrime, u, betaHead, betaTail, i);
				break;
			}
		// IIB: suffix link for u is unknown and u' is the root
		case 3:
			{
				Node *uPrime = u -> parent;
				int betaHead = u -> suffixHead, betaTail = u -> suffixTail; //betaHead to betaTail contains Beta
				int betaPrimeHead = betaHead + 1;                           //betaPrimeHead to betaTail contains BetaPrime
				if (betaTail == betaHead){ // the lenght of u is 1
					u -> suffixLink = uPrime;
					return find_path(uPrime, i);
				} else {
					return node_hops(uPrime, u, betaPrimeHead, betaTail, i);
				}
				break;
			}
		default:
			printf("ERROR: Couldn't insert\n");
			exit(1);
	}
	return 0;
}


// Construct Suffix Tree
Node *buildTree(){
	Node *root = make_node(0, NULL, 0, 0, 0);
	root -> suffixLink = root;
	Node *leaf = root;
	for(int i=0; i < sequenceLength; i++ ){
		leaf = insert( i, root, leaf);
		if (leaf == NULL)
			return NULL;
	}
	return root;
}

// depth first search - preorder
int dfs( Node *node ){
	unsigned int i;
	
	printf("Depth: %d\t", node->depth);
	printf("NID: %d\t", node->id);
	if ( node != NULL ) {
		printf("Parent: %d\t", node->parent->id);
		//printf("EdgeLabel: %s\t", node->parentEdgeLabel);
		printf("EdgeLabel: ");
		for(i = node->suffixHead; i <= node->suffixTail; i++){
			printf("%c", sequence[i]);
		}
		printf("\t");
	}
	if( node->suffixLink )
		printf("SL->ID: %d\n", node->suffixLink->id); //, node->suffixLink);
	else
		printf("SL->ID: NULL\n");
	for ( i = 0; i < node->numberChildren; ++i)
	{
		dfs(node->children[i]);
	}
	return (0);
}



// enumerate children of node from left to right
int display_children(Node *node){
	printf("Enumerating children of Node ID: %d from left to right:\n", node -> id);
    for(int i = 0; i < node -> numberChildren; i++)
    {
        printf("Child Number: %d, ID: %d ; ", i, node -> children[i] -> id);
    }
	printf("\n");
    return 0;
}


// Burrows Wheeler Transform
int bwt(Node *node, FILE *ptr){
	int i;
	if(node -> numberChildren == 0){
		int value = node -> id - 1;
		fprintf(ptr,"%c\r\n", (value > 0 ? sequence[value - 1] : sequence[sequenceLength - 1]));
	} else {
		for (i = 0; i < node -> numberChildren; i++){
			bwt(node -> children[i], ptr);
		}
	}
}