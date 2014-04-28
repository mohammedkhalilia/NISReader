/**********************************************************************
* split()
*	Parameters:
*		del - string delimiter
*		str - string to split
*
*   Return:
*       arr- array of tokens
**********************************************************************/
int *split(const char *del, char *str){
	int *arr, counter=1;
	char *tok;

	arr = malloc(sizeof(int) * 263);

	tok = strtok(str,del);
	while(tok != NULL){
		arr[counter++] = atoi(tok);
		tok = strtok(NULL,del);
	}
	return arr;
}

/**********************************************************************
* substring()
*	Parameters:
*		pos - position (where to start copying)
*		len - how many characters to copy
*		str - source to copy from
*       dest- destination string
*
*   Return:
*       dest- the substring
**********************************************************************/
char *substring(int pos, int len, char *str, char *dest){
	printf("%d - %d - %d\n",pos,len, strlen(str));
    if(pos+len > strlen(str))
	        return NULL;

	strncpy(dest,str+pos,len);
	return dest;
}

/**********************************************************************
* randomNumbers()
*	Parameters:
*		start - starting range
*		end - ending range
*		k - number of values to randomly choose within that range
*
*   Return:
*       final - sorted array
**********************************************************************/
int *randomNumbers(int start, int end, int k){
	int *numbers, i, randInt, temp, *final;

	numbers = malloc(sizeof(int) * (end-start));
	final = malloc(sizeof(int)*k);
	//initialize array of numbers

	for(i=0;i<end;i++) numbers[i] = start + i;

	//randomly shuffle the array
	for(i=end-1;i>0;i--){
		srand(time(NULL));
		randInt = rand() % i;
		temp = numbers[randInt];
		numbers[randInt] = numbers[i];
		numbers[i] = temp;
	}

	//copy k elements only
	for(i=0;i<k;i++)
		final[i] = numbers[i];

	free(numbers);
	quicksort(final, 0, k-1);

	return final;
}

/**********************************************************************
* swap()
*	Parameters:
*		pointer to x
*		pointer to y
*
*   Return:
*       None
**********************************************************************/
void swap(int *x,int *y){
   int temp;
   temp = *x;
   *x = *y;
   *y = temp;
}

/**********************************************************************
* quicksort()
*	Parameters:
*		items
*		left
*		right
*
*   Return:
*       None
**********************************************************************/
void quicksort(int items[], int left, int right){
	int i=left, j=right, pivot=items[(left+right)/2];

	do {
		while((items[i] < pivot) && (i < right)) i++;
		while((pivot < items[j]) && (j > left)) j--;
		if(i <= j) {
			swap(&items[i],&items[j]);
			i++; j--;
		}
	} while(i <= j);

	if(i < right) quicksort(items, i, right);
	if(left < j) quicksort(items, left, j);
}
