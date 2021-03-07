#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <wchar.h>
#include <locale.h>

// Keeps data about read documents
struct textFile {
   char  filename[50];	// keeps filename
   int   score;	// keeps number of matched scores
   char content[20000];	// keeps content of the text file
};

// Heap structure
struct heap {
	int size;	// keeps heap size
	int count;	 
	int *heap_pointer;
};


int *heap, size, count;
int initial_size = 0;
static usedID[42];

struct textFile txt[42]; 

// Initializes the heap
void heap_initialize(struct heap *h)
{
	h->count = 0;
	h->size = 0;
	h->heap_pointer = (int *) malloc(sizeof(int) * 4);
}

void convert_to_max_heap(int *data, int loc, int count) {
	int left, right, largest, temp;
	left = 2*(loc) + 1;
	right = left + 1;
	largest = loc;
	

	if (left <= count && data[left] > data[largest]) {
		largest = left;
	} 
	if (right <= count && data[right] > data[largest]) {
		largest = right;
	} 
	
	if(largest != loc) {
		temp = data[loc];
		data[loc] = data[largest];
		data[largest] = temp;
		convert_to_max_heap(data, largest, count);
	}

}

void heap_push(struct heap *h, int value)
{
	int index, parent;
 
	// Resize the heap if it is too small to hold all the data
	if (h->count == h->size)
	{
		h->size += 1;
		h->heap_pointer = realloc(h->heap_pointer, sizeof(int) * h->size);
		if (!h->heap_pointer) exit(-1); // Exit if the memory allocation fails
	}
 	
 	index = h->count++; // First insert at last of array

 	// Find out where to put the element and put it
	for(;index; index = parent)
	{
		parent = (index - 1) / 2;
		if (h->heap_pointer[parent] >= value) break;
		h->heap_pointer[index] = h->heap_pointer[parent];
	}
	h->heap_pointer[index] = value;
}

void heap_display(struct heap *h) {
	int i;
	for(i=0; i<h->count; ++i) {
		//printf("|%d|", h->heap_pointer[i]);	// print heap structure
	}
	printf("\n");
}

int heap_delete(struct heap *h)
{
	int removed;
	int temp = h->heap_pointer[--h->count];
 	
	
	if ((h->count <= (h->size + 2)) && (h->size > initial_size))
	{
		h->size -= 1;
		h->heap_pointer = realloc(h->heap_pointer, sizeof(int) * h->size);
	}
 	removed = h->heap_pointer[0];
 	h->heap_pointer[0] = temp;
 	convert_to_max_heap(h->heap_pointer, 0, h->count);
 	return removed;
}


int print_PrQ(struct heap *pq, int numRelDocs) {
	int i;
	int numOfDocs = 5;
	int temp;
	int score;
	char docName[50];
	char docContent[numRelDocs][20000];
	char docFilename[numRelDocs][50];
	int docScores[numRelDocs];
	
	if(numRelDocs < numOfDocs)
		numOfDocs = numRelDocs;
		
	printf("The relevance order is: ");
	for(i=0;i<numOfDocs;i++) {
		score = heap_delete(pq);
		temp = searchDocID(score);
		strcpy(docName,txt[temp].filename);
		strcpy(docContent[i],txt[temp].content);
		strcpy(docFilename[i],txt[temp].filename);
		docScores[i] = txt[temp].score;
		
		printf("%s",docName);
		if(i==numOfDocs-1) 
			printf("(%d).",score);
		else
			printf("(%d), ",score);	
	}
	printf("\n\n");
	i=0;
	
	for(i=0;i<numOfDocs;i++) {
		printf("%s",docFilename[i]);
		printf("(%d): ",docScores[i]);
		printf("%s",docContent[i]);
		printf("\n");
	}
	
}
	
int searchDocID(int score) {
	int i;
	for(i=0;i<42;i++) {
		if((txt[i].score == score) && usedID[i]==0) {	// if used before, search for another
			usedID[i] = 1;
			return i;
		}
	}
}

int main()
{
    DIR *folder;
    FILE *fp;
    char str[20000];
    char path[50];
    int i=0;  
    char tempC;
    
    struct dirent *entry;
    int files = 0;

    folder = opendir("files");
    if(folder == NULL)
    {
        perror("Unable to read directory");
        return(-1);
    }

    while( (entry=readdir(folder)) ) {
    	i=0;
    	strcpy(path, "files/");
    	if ( !strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..") ) {	// omit . and .. directories
     		// do nothing
		} 
		else {
			strcat(path,entry->d_name);
        	strcpy(txt[files].filename, entry->d_name);
			fp = fopen(path, "r");
			
			while(!feof(fp)) {
				tempC = getc(fp);
				if(tempC == 'á') 
					tempC = '\0';
				str[i++] = tempC;
     		}
			str[i]='\0';
        	strcpy(txt[files].content, str);
        	files++;
    	}
    	
    }
	
    closedir(folder);
    
    // Get keyword from the user
    char tempStr[20000];
    
    
    char keyword[50];
    printf("Enter a keyword: ");
    scanf("%s",&keyword);
    
    
    int count =0;
    int numRelDocs=0;
    
    int k=0;
    
    // Cut the string to words and search for the keyword
    char * pch;
    i=0;
    for(i=0;i<42;i++) {
    	strcpy(tempStr,txt[i].content);
    	pch = strtok (tempStr,"  . ,-!?:\"\t\-\á\t\r\n\v\f()/+");
    	while (pch != NULL) {
    		
    		for(k = 0; pch[k]; k++){
  				pch[k] = tolower(pch[k]);
			}
    		if (strcmp(pch, keyword) == 0)  {
				count++;
			}
    		pch = strtok (NULL, "  . ,-!?:\"\t\-\á\t\r\n\v\f()/+");
 		}
 		txt[i].score = count;
 		if(count>0)
 			numRelDocs++;
 		count=0;
	}
	
	// Heap implementation
	
	struct heap heap;
	heap_initialize(&heap);
	
	// Push values to the heap
	
	for(i=0;i<42;i++) {
		heap_push(&heap,txt[i].score);	
	}

	print_PrQ(&heap, numRelDocs);
	
	
	return 0;
}

