#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <math.h>

// define character size
// currently Trie supports lowercase English characters (a - z)
#define CHAR_SIZE 26


//Structure to create a list of words in the query
struct Word{
	char word[256];
	struct Word *next;
};


//Structure to create a list of files in the present working directory.
struct File{
	char fileName[4096];
	struct File *next;
	double weight;
};

//Structure to create a list of files at the leaf nodes of a trie.
struct List{
	char fileName[4096];
	struct List *next;
	int count;
};

//Structure to create a trie node.
struct Trie
{
    int isLeaf;    // 1 when node is a leaf node
    struct Trie* character[CHAR_SIZE];
    struct List *fileList;
};


/* function prototypes */
struct File* SortedMerge(struct File* a, struct File* b); 
void FrontBackSplit(struct File* source, 
        struct File** frontRef, struct File** backRef); 
  
/* sorts the linked list by changing next pointers (not data) */
void MergeSort(struct File** headRef) 
{ 
struct File* head = *headRef; 
struct File* a; 
struct File* b; 
  
/* Base case -- length 0 or 1 */
if ((head == NULL) || (head->next == NULL)) 
{ 
    return; 
} 
  
/* Split head into 'a' and 'b' sublists */
FrontBackSplit(head, &a, &b);  
  
/* Recursively sort the sublists */
MergeSort(&a); 
MergeSort(&b); 
  
/* answer = merge the two sorted lists together */
*headRef = SortedMerge(a, b); 
} 
  
struct File* SortedMerge(struct File* a, struct File* b) 
{ 
struct File* result = NULL; 
  
/* Base cases */
if (a == NULL) 
    return(b); 
else if (b==NULL) 
    return(a); 
  
/* Pick either a or b, and recur */
if (a->weight >= b->weight) 
{ 
    result = a; 
    result->next = SortedMerge(a->next, b); 
} 
else
{ 
    result = b; 
    result->next = SortedMerge(a, b->next); 
} 
return(result); 
} 
  
/* UTILITY FUNCTIONS */
/* Split the nodes of the given list into front and back halves, 
    and return the two lists using the reference parameters. 
    If the length is odd, the extra node should go in the front list. 
    Uses the fast/slow pointer strategy. */
void FrontBackSplit(struct File* source, 
        struct File** frontRef, struct File** backRef) 
{ 
        struct File* fast; 
        struct File* slow; 
    slow = source; 
    fast = source->next; 
  
    /* Advance 'fast' two nodes, and advance 'slow' one node */
    while (fast != NULL) 
    { 
    fast = fast->next; 
    if (fast != NULL) 
    { 
        slow = slow->next; 
        fast = fast->next; 
    } 
    } 
  
    /* 'slow' is before the midpoint in the list, so split it in two 
    at that point. */
    *frontRef = source; 
    *backRef = slow->next; 
    slow->next = NULL; 
} 
// Function that returns a new Trie node
struct Trie* getNewTrieNode()
{
    struct Trie* node = (struct Trie*)malloc(sizeof(struct Trie));
    node->isLeaf = 0;

    for (int i = 0; i < CHAR_SIZE; i++)
        node->character[i] = NULL;

    node->fileList = NULL;
    return node;
}

// Iterative function to insert a string in Trie.
void insert(struct Trie* *head, char* str,char* fileName)
{
    // start from root node
    struct Trie* curr = *head;
    while (*str)
    {
        // create a new node if path doesn't exists
        if (curr->character[*str - 'a'] == NULL)
            curr->character[*str - 'a'] = getNewTrieNode();

        // go to next node
        curr = curr->character[*str - 'a'];

        // move to next character
        str++;
    }

    // mark current node as leaf
    curr->isLeaf = 1;
    

    /**********************************************************************************
     * Initate a function to create linked list here to insert a node in linked list. *
     * That node will contain the name of the file in which that word is present.     *
     **********************************************************************************
     */

	if(curr->fileList == NULL){
		curr->fileList = (struct List *)malloc(sizeof(struct List));
		curr->fileList->next = NULL;
		strcpy(curr->fileList->fileName,fileName);
		curr->fileList->count = 1;
	}
	else{
		struct List *temp,*newNode;
		temp = curr->fileList;
		while(temp->next != NULL){
			if(strcmp(temp->fileName,fileName)==0){
				temp->count++;
				break;
			}
			temp = temp->next;
		}
		if(temp->next == NULL){
			if(strcmp(temp->fileName,fileName)==0){
				temp->count++;
			}
			else{
				newNode = (struct List *)malloc(sizeof(struct List));
				newNode->next = NULL;
				newNode->count = 1;
				strcpy(newNode->fileName,fileName);
				temp->next = newNode;
			}
		}
	}
}

// Iterative function to search a string in Trie. It returns 1
// if the string is found in the Trie, else it returns 0
int search(struct Trie* head, char* str,char *filename)
{
    // return 0 if Trie is empty
    if (head == NULL)
        return 0;

    struct Trie* curr = head;
    while (*str)
    {
	char c = *str;

	if((c>='A')&&(c<='Z')){
		c = c + 32;
	}
        // go to next node
        curr = curr->character[c - 'a'];

        // if string is invalid (reached end of path in Trie)
        if (curr == NULL)
            return 0;

        // move to next character
        str++;
    }

    // if current node is a leaf and we have reached the
    // end of the string, return 1

	if(curr->fileList != NULL){
		struct List *temp;
		temp = curr->fileList;
		while(temp!=NULL){
			//printf("The string is present in : %s , %d times.\n",temp->fileName,temp->count);
			if(strcmp(filename,temp->fileName)==0){
				return temp->count;
			}
			temp = temp->next;
		}
	}
    return 0;
}

// returns 1 if given node has any children
int haveChildren(struct Trie* curr)
{
    for (int i = 0; i < CHAR_SIZE; i++)
        if (curr->character[i])
            return 1;    // child found

    return 0;
}

// Recursive function to delete a string in Trie.
int deletion(struct Trie* *curr, char* str)
{
    // return if Trie is empty
    if (*curr == NULL)
        return 0;

    // if we have not reached the end of the string
    if (*str)
    {
        // recurse for the node corresponding to next character in
        // the string and if it returns 1, delete current node
        // (if it is non-leaf)
        if (*curr != NULL && (*curr)->character[*str - 'a'] != NULL &&
            deletion(&((*curr)->character[*str - 'a']), str + 1) &&
            (*curr)->isLeaf == 0)
        {
            if (!haveChildren(*curr))
            {
                free(*curr);
                (*curr) = NULL;
                return 1;
            }
            else {
                return 0;
            }
        }
    }

    // if we have reached the end of the string
    if (*str == '\0' && (*curr)->isLeaf)
    {
        // if current node is a leaf node and don't have any children
        if (!haveChildren(*curr))
        {
            free(*curr); // delete current node
            (*curr) = NULL;
            return 1; // delete non-leaf parent nodes
        }

        // if current node is a leaf node and have children
        else
        {
            // mark current node as non-leaf node (DON'T DELETE IT)
            (*curr)->isLeaf = 0;
            return 0;       // don't delete its parent nodes
        }
    }

    return 0;
}

void initiateTrie(struct Trie** head)
{
	FILE *fp;
	DIR *d;
	struct dirent *dir;
	char fileName[4096];
	int len;
	/********************************************************
	 * Here the file name is supposed to be given by the	*
	 * function which reads the present working directory	*
	 * and returns us the name of the file one by one.	*
	 * ******************************************************
	 */
	char cwd[4096];
	if((getcwd(cwd,sizeof(cwd))==NULL))
	{
		//If it is not able to fetch the path of the current working directory.
		perror("getcwd() error!");        
		exit(-1);
	}

	len = strlen(cwd);
	if(cwd[len-1]!='/')
        {
                strcat(cwd,"/");
        }

        d=opendir(cwd);

	if(d)
	{
		while((dir=readdir(d))!=NULL)
		{
			if(dir->d_type!=DT_DIR)
			{
				strcpy(fileName,dir->d_name);
				//printf("File Name is : %s\n",fileName);
				fp = fopen(fileName,"r");
				if (!fp)
				{
					printf("File not open");
				}
				char word[4096];
				int i;
				char c;
				i = 0;
				while((!feof(fp))&&(i<=4096))
				{
					c = fgetc(fp);
					if(!(((c>='A')&&(c<='Z'))||((c>='a')&&(c<='z'))))
					{
				//		printf("%s\n",word);
						insert(head,word,fileName);
						memset(word,0,strlen(word));
						i = 0;
					}
					else
					{
						if((c>='A')&&(c<='Z')){
							c = c + 32;
						}
						word[i]=c;
						i++;
					}
				}
			}
		}

	}
}

void performTfIdf(struct Trie *head,char *query){

	//Create a Linked List of words present in the query.
	
	struct Word *queryList;
	char *token = strtok(query," ");
	int wordCount;
	wordCount = 0;
	queryList = NULL;
	while (token != NULL){
		
		if (queryList == NULL){
			queryList = (struct Word *)malloc(sizeof(struct Word));
			queryList->next = NULL;
			strcpy(queryList->word, token);
		}
		else{
			struct Word *temp,*newNode;
			temp = queryList;
			while(temp->next != NULL){
				temp = temp->next;
			}
			newNode = (struct Word *)malloc(sizeof(struct Word));
			newNode->next = NULL;
			strcpy(newNode->word,token);
			temp->next = newNode;
		}
		wordCount++;
		token = strtok(NULL," ");
	}
	//Create a Linked of files present in the current working directory.
	
	struct File *fileList;
	fileList = NULL;
	FILE *fp;
        DIR *d;
        struct dirent *dir;
        char fileName[4096];
        int len;
        /********************************************************
         * Here the file name is supposed to be given by the    *
         * function which reads the present working directory   *
         * and returns us the name of the file one by one.      *
         * ******************************************************
         */
        char cwd[4096];
	int fileCount;
	fileCount = 0;
        if((getcwd(cwd,sizeof(cwd))==NULL))
        {
                //If it is not able to fetch the path of the current working directory.
                perror("getcwd() error!");
                exit(-1);
        }

        len = strlen(cwd);
        if(cwd[len-1]!='/')
        {
                strcat(cwd,"/");
        }

        d=opendir(cwd);

        if(d)
        {
                while((dir=readdir(d))!=NULL)
                {
                        if(dir->d_type!=DT_DIR)
                        {
				fileCount++;
				if(fileList == NULL){
					fileList = (struct File *)malloc(sizeof(struct File));
					fileList->next = NULL;
                                	strcpy(fileList->fileName,dir->d_name);
				}
				else{
					struct File *temp,*newNode;
					temp = fileList;
					while(temp->next != NULL){
						temp = temp->next;
					}
					newNode = (struct File *)malloc(sizeof(struct File));
					newNode->next = NULL;
					strcpy(newNode->fileName,dir->d_name);
					temp->next = newNode;
				}
			}
		}
	}
	
//	printf("Word Count %d File Count %d \n",wordCount,fileCount);
	//Implementing tf-idf
	int wordVsFile[wordCount][fileCount];
	float tfArray[wordCount][fileCount];
	double idfArray[wordCount];
	double tfidf[fileCount];
	struct Word *wtemp;
	struct File *ftemp;
	int total[fileCount];
	int i,j;
	
	for(i=0;i<fileCount;i++){
		total[i] = 0;
	}
	
	wtemp = queryList;
	ftemp = fileList;

	i=0;
	j=0;
	
	while(wtemp!=NULL){
		j=0;
		ftemp = fileList;
		while(ftemp!=NULL){
			wordVsFile [i][j] = search(head,wtemp->word,ftemp->fileName);
			total[j] = total[j] + wordVsFile[i][j];
			ftemp = ftemp->next;
			j++;
		}
		wtemp = wtemp->next;
		i++;
	}
	int s;
	for(i=0;i<wordCount;i++)
	{	
		s = 0;
		for(j=0;j<fileCount;j++){
			if(total[j] == 0){
				total[j] = 500;
			}
			if(wordVsFile[i][j]!=0){
				s++;
			}
			tfArray[i][j] = (float)wordVsFile[i][j]/(float)total[j];
		}
		idfArray[i] = log((double)fileCount/(double)s);
	}

	for(j=0;j<fileCount;j++){
		double x = 0.0;
		for(i=0;i<wordCount;i++){
			x = x+tfArray[i][j]*idfArray[i];
		}
		tfidf[j] = x;
//		printf("Weight[%d]  = %lf\n",j,tfidf[j]);
	}
	
	ftemp = fileList;
	i = 0;
	while(ftemp!=NULL){
		ftemp->weight = tfidf[i++];
		ftemp = ftemp->next;
	}
	
/*
	printf("Before sorting \n");
	
	for(ftemp=fileList;ftemp!=NULL;ftemp = ftemp->next){
		printf("%s : %lf \n",ftemp->fileName,ftemp->weight);
	}
*/
	MergeSort(&fileList);
	//Sort the file list

	printf("\nFile Arranged the in decreasing order of their relevancy are : \n\n");

	printf("FileName\t:\tRelevancy\n\n");	
	for(ftemp=fileList;ftemp!=NULL;ftemp = ftemp->next){
		printf("%s\t:\t%lf\n",ftemp->fileName,ftemp->weight);
	}

	
	/*
	i = 0;
	j = 0;
	wtemp = queryList;
	ftemp = fileList;
	printf("\t");
	while(ftemp!=NULL){
		printf("%s\t",ftemp->fileName);
		ftemp = ftemp->next;
	}
	printf("\n");
	while(wtemp!=NULL){
		j = 0;
		ftemp = fileList;
		printf("%s\t",wtemp->word);
		while(ftemp!=NULL){
			printf("%d\t",wordVsFile[i][j]);
			j++;
			ftemp = ftemp->next;
		}
		wtemp = wtemp->next;
		i++;
		printf("\n");
	}*/
}

// Trie Implementation in C - Insertion, Searching and Deletion
int main()
{
//	printf("Hello");
    struct Trie* head = getNewTrieNode();
    initiateTrie(&head);

    /*
    deletion(&head, "hello");
    printf("%d ", search(head, "hello"));       // print 0 (hello deleted)
    printf("%d ", search(head, "helloworld"));  // print 1
    printf("%d \n", search(head, "hell"));      // print 1 + newline
    deletion(&head, "h");
    printf("%d ", search(head, "h"));           // print 0 (h deleted)
    printf("%d ", search(head, "hell"));        // print 1
    printf("%d\n", search(head, "helloworld")); // print 1 + newline
    deletion(&head, "helloworld");
    printf("%d ", search(head, "helloworld"));  // print 0
    printf("%d ", search(head, "hell"));        // print 1
    deletion(&head, "hell");
    printf("%d\n", search(head, "hell"));       // print 0 + newline
	*/

    if (head == NULL)
        printf("Trie empty!!\n");               // Trie is empty now

    char query[4096];
    printf("Enter a string : ");
    scanf("%[^\n]%*c",query);
    performTfIdf(head,query);

return 0;
}
