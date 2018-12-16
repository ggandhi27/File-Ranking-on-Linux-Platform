#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>


// define character size
// currently Trie supports lowercase English characters (a - z)
#define CHAR_SIZE 26


struct List{
	char fileName[4096];
	struct List *next;
	int count;
};

// A Trie node
struct Trie
{
    int isLeaf;    // 1 when node is a leaf node
    struct Trie* character[CHAR_SIZE];
    struct List *fileList;
};

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
int search(struct Trie* head, char* str)
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
			printf("The string is present in : %s , %d times.\n",temp->fileName,temp->count);
			temp = temp->next;
		}
	}
    return curr->isLeaf;
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
				printf("File Name is : %s\n",fileName);
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
						printf("%s\n",word);
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
    scanf("%s",query);
    printf("%d ", search(head,query));        // print 0

return 0;
}