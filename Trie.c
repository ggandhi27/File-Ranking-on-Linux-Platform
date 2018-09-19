/* There is a problem in this code, just try to understand the readFromFile() and append() right next to it. What here we’re doing is, inserting strings from files to trie, which unfortunately is not happening, share your views on this. However there’s no compilation error. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//**********************************************************
#define CSIZE 26

int i;

FILE *fp;

char strRead[30];

char ch;

struct Trie

{

    int isLeaf;   

    struct Trie* character[CSIZE];

};

 

struct Trie* getNewTrieNode()

{

    struct Trie* node=(struct Trie*)malloc(sizeof(struct Trie));

    node->isLeaf = 0;

    for(i=0;i<CSIZE;i++)

        node->character[i]=NULL;

 

    return node;

}

 

//**********************************************************

 

void append(char chRead);
void insertIntoTrie(struct Trie* *head, char* str);
int readFromFile(struct Trie* *head)

{

              char chRead;

              fp=fopen("Test2.txt","r");

              chRead=fgetc(fp);

              while(chRead!=EOF)

              {

                           if(chRead!=' ')

                                         append(chRead);

                           else if(chRead==' ')

                           {

                                         insertIntoTrie(head, strRead);

                                         char s1[1];

                                         s1[0]='\0';

                                         strcpy(strRead,s1);

                           }

              }

              return 0;

}

 

void append(char chRead)

{

        int len = strlen(strRead);

        strRead[len] = chRead;

        strRead[len+1] = '\0';

}

 

//**********************************************************

 

int haveChildren(struct Trie* curr)

{

    for(i=0;i<CSIZE;i++)

        if (curr->character[i])

            return 1;

 

    return 0;

}

 

int search(struct Trie** head, char* str)

{

    if (head==NULL)

        return 0;

 

    struct Trie* curr=*head;

    while (*str)

    {

        curr=curr->character[*str-'a'];

        if (curr==NULL)

            return 0;

        str++;

    }

    return curr->isLeaf;

}

 

void insertIntoTrie(struct Trie* *head, char* str)

{

    struct Trie* curr= *head;

    while (*str)

    {

              int num=*str-'a';

                           if(curr->character[num]==NULL)

                                         curr->character[num]=getNewTrieNode();

        curr=curr->character[num];

        str++;

    }

    curr->isLeaf=1;

}

 


int deletion(struct Trie* *curr, char* str)

{

    if (*curr==NULL)

        return 0;

              if (*str)

    {

        if (*curr!=NULL && (*curr)->character[*str-'a']!= NULL && deletion( &((*curr)->character[*str - 'a']), str + 1) &&(*curr)->isLeaf == 0)

        {

            if (!haveChildren(*curr))

            {

                free(*curr);

                (*curr) = NULL;

                return 1;

            }

            else

                return 0;

        }

    }

              if (*str=='\0' && (*curr)->isLeaf)

    {

        if(!haveChildren(*curr))

        {

              free(*curr);

                                         (*curr) = NULL;

                                         return 1;

                           }

                           else

                           {

                                         (*curr)->isLeaf = 0;//

                                         return 0;

                           }

              }

              return 0;

}

 

//**********************************************************

 

int main()

{

    struct Trie* head = getNewTrieNode();

    if (head == NULL)

                           printf("Trie not created.\n");

              int outCome=readFromFile(&head);

              if(outCome==0)

                           printf("File inserted into Trie.\n");

              char strInput[20];

              int i=0;

              for(;i<10;i++)

              {

                           scanf("%[^\n]%*c", strInput);

                           printf("%d\nSearch:", search(&head,strInput));

              }

    return 0;

}
