#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "libs/string.h"
#define FLAG_DEF 4
#define FLAG_INC 3
#define FLAG_FUNCTION 1
#define FLAG_GLOB 2
#define FLAG_TDEF 5
#define FLAG_EMPTY 0
String* files;
String* growArr(String* strArr, int len, int inc){
	String* cloneArr = (String*) malloc(sizeof(String*) * (len + inc));
	for (int i = 0; i < len; i++){
		cloneArr[i] = strArr[i];
	}
	free(strArr);
	return cloneArr;
}
int hasEntry(char* ptr, String* arr, int len){
	for (int i = 0; i < len; i++){
		if (strEqualPtr(&arr[i], ptr) == 0){
			return 1;
		}
	}
	return 0;
}
void removeEntry(String* arr, int index, int len){
	//free(&arr[index]);
	for (int i = index; i < len; i++){
		arr[i] = arr[i+1];
	}
}
int checkMain(String* fPath){
	if (fPath->string[fPath->length-1] == 'c' && fPath->string[fPath->length-2] == '.' && fPath->string[fPath->length-1] == 'n' && fPath->string[fPath->length-1] == 'i' && fPath->string[fPath->length-1] == 'a' && fPath->string[fPath->length-1] == 'm' && fPath->string[fPath->length-1] == '/'){
		return 1;
	}
	return 0;
}
	const char type[8] = "typedef";
	const char struc[] = "struct";
	const char def[] = "#define";
	const char inc[] = "#include";
String* baseDir;
void makeHeader(FILE* read, FILE* write){
    int j = 0;
	int k = 0;
	int mode = FLAG_EMPTY;
	int bracketDepth = 0;
	String* toAppend = emptyStr(64);
	char tempStorage[512];
    while (fgets(tempStorage, 511, read)!= NULL){	
		j = 0;
		toAppend->length = 0;
		toAppend->string[0] = '\0';
        while(tempStorage[j] == ' ' && tempStorage[j] == '	'){
			j++;
		}
		if (mode != FLAG_FUNCTION && mode != FLAG_DEF && mode != FLAG_TDEF && bracketDepth == 0){
			mode = FLAG_EMPTY;
		}
		k = 0;
		while (tempStorage[j+k] == type[k] && mode != FLAG_FUNCTION){
			k++;
			if (type[k] == '\0'){
				j += k;
				mode = FLAG_TDEF;
			}
		}
		k = 0;
		while (tempStorage[j+k] == struc[k] && mode != FLAG_FUNCTION){
			k++;
			if (struc[k] == '\0'){
				j += k;
				mode = FLAG_TDEF;
			}
		}
		k = 0;
		while (tempStorage[j+k] == def[k] && mode != FLAG_FUNCTION){
			k++;
			if (def[k] == '\0'){
				j += k;
				mode = FLAG_DEF;
			}
		}
		k = 0;
		while (tempStorage[j+k] == inc[k] && mode != FLAG_FUNCTION){
			k++;
			if (inc[k] == '\0'){
				j += k;
				mode = FLAG_INC;
				// insert c file detect.
				while (tempStorage[j] != '\0'){
					j++;
				}
			}
		}
		if (tempStorage[j+1] != '\n' && mode == FLAG_EMPTY && mode != FLAG_FUNCTION){
			mode = FLAG_GLOB;
		}
		if (mode != FLAG_INC){
			while (tempStorage[j] != '\0'){
				if (tempStorage[j] == '{'){
					bracketDepth++;
					printf("curr mode: %d \n", mode);
					if (mode == FLAG_EMPTY){
						appendNoLen(toAppend, tempStorage, 512);
						// sub { by ;
						toAppend->string[j] = ';';
						toAppend->string[j+1] = '\0';
						toAppend->length = j+1;
						fwrite(toAppend->string, 1, toAppend->length, write);
						mode = FLAG_FUNCTION;
					}
				} else if (tempStorage[j] == '}') {
					bracketDepth--;
					if (mode == FLAG_FUNCTION && bracketDepth == 0){
						mode = FLAG_EMPTY;				
					}
				}
				j++; 
			}
		}
		if (mode == FLAG_DEF){
			fwrite(tempStorage, 1, j, write);
			if (bracketDepth == 0 && tempStorage[j-2] != '\\'){
				printf("leaving define flag\n");
				mode = FLAG_EMPTY;
			}
		}
		if (mode == FLAG_TDEF){
			printf("%d Bdepth\n", bracketDepth);
			fwrite(tempStorage, 1, j, write);
			if (bracketDepth == 0){
				mode = FLAG_EMPTY;
			}
		}
		if (mode == FLAG_INC){
			fwrite(tempStorage, 1, j, write);
		}
		if (mode == FLAG_GLOB){
			fwrite(tempStorage, 1, j, write);
		}
		toAppend->length = 0;
		toAppend->string[0] = '\0';
	}
}
int main(int argC, char**args){
	if (argC == 1){
		printf("no files specified");
		return 0;
	}
	char cwd[256]; 
	getcwd(cwd, 256);
	baseDir = buildStr(cwd,strlen(cwd));
	files = (String*) malloc(sizeof(String*)*4);
	int len = 0;
	int size = 4;
	int start = 1;
	int isSpef = 0;
	int confirm = 0;
	while (isSpef == 0){
		//printf("test\n");
		if (strcmp(args[start], "confirm-add") == 0){
			confirm = 1;
			//printf("WHY \n");
			start++;
		} else {
			isSpef = 1;
		}
		//printf("test2\n");
	}
	for (int i = start; i < argC; i++){
		int currL = strlen(args[i]);
		if (hasEntry(args[i], files, len) == 0){
		       if (args[i][currL-1] == 'c' && args[i][currL-2] == '.'){
			files[len] = *buildStr(args[i], currL);
			len++;
		       } else {
       			printf("file \"%s\" is not a C file\n", args[i]);			       
		       }
		} else {
			printf("file \"%s\" specified twice\n",args[i]);
		}
		if (len == size){
			files = growArr(files, len, 4);
			size += 4;
		}
	}
	struct stat status;
	appendPtr(baseDir, "/", 1);
	int isMain = 0;
	//int open = 0;
	String* writer = emptyStr(64);
	String* readStr;
	String* writeStr;
	for (int i = 0; i < len; i++){
		appendStr(baseDir, &files[i]);
		if (stat(baseDir->string, &status) == -1){
			printf("file %s seems to be unavailable\n", baseDir->string);
			baseDir->length -= files[i].length;
			baseDir->string[baseDir->length] = '\0';
			removeEntry(files, i, len);
			len--;
			i--;
		} else {
			readStr = cloneStr(baseDir);
			FILE* read = fopen(readStr->string, "r+");
			FILE* write;
			baseDir->string[baseDir->length - 1] = 'h';
			writeStr = cloneStr(baseDir);
			isMain = checkMain(baseDir);
			if (isMain == 0){
				write = fopen(writeStr->string, "w+");
				if (write == NULL){
					printf("wtf");
				}
			}
			if (read == NULL){
				printf("failed to read file \"%s\"", baseDir->string);
			} else {
				char tempStorage[512];
				int row = 0;
                makeHeader(read, write);
			if (write != NULL){
				fclose(write);
				write = NULL;
			}
			if (read != NULL){
				fclose(read);
				read = NULL;
			}
			discardStr(readStr);
			discardStr(writeStr);
			printf("done creating header for %s\n", baseDir->string);
			baseDir->length -= files[i].length;
			baseDir->string[baseDir->length] = '\0';
		}
		}
	}
	return 0;
}	
