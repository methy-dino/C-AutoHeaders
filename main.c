#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <poll.h>
#include "libs/string.h"
#define FLAG_DEF 4
#define FLAG_INC 3
#define FLAG_FUNCTION 1
#define FLAG_GLOB 2
#define FLAG_TDEF 5
#define FLAG_EMPTY 0
String* files;
int len = 0;
int size = 4;
int confirm = 0;
int noAdd = -1;
String* growArr(String* strArr, int len, int inc){
	String* cloneArr = (String*) malloc(sizeof(String*) * (len + inc));
	for (int i = 0; i < len; i++){
		cloneArr[i] = strArr[i];
	}
	free(strArr);
	return cloneArr;
}
int hasEntry(char* ptr){
	for (int i = 0; i < len; i++){
		if (strEqualPtr(&files[i], ptr) == 0){
			return 1;
		}
	}
	return 0;
}
void addEntry(String* entry){
	files[len] = *entry;
	len++;
	if (len == size){
		files = growArr(files, len, 4);
		size += 4;
	}
}
void removeEntry(String* arr, int index){
	//free(&arr[index]);
	for (int i = index; i < len; i++){
		arr[i] = arr[i+1];
	}
}
int checkMain(String* fPath){
	if (fPath->length < 7){
		return 0;
	}
	if (fPath->string[fPath->length-1] == 'c' && fPath->string[fPath->length-2] == '.' && fPath->string[fPath->length-3] == 'n' && fPath->string[fPath->length-4] == 'i' && fPath->string[fPath->length-5] == 'a' && fPath->string[fPath->length-6] == 'm' && fPath->string[fPath->length-7] == '/'){
		return 1;
	}
	return 0;
}
	const char type[8] = "typedef";
	const char struc[] = "struct";
	const char def[] = "#define";
	const char inc[] = "#include";
String* baseDir;
void importEntry(String* newEntry){
	if (hasEntry(newEntry->string) || (noAdd == 1 && confirm == 0)){
		return;
	}
	if (confirm == 1){
		printf("found new file: \"%s\", do you wish to include it in the header generation? (y\\n)", newEntry->string);
		struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };
		char answer = '\0';
		if (poll(&mypoll, 1, 10000)){
			scanf("%c", &answer);
			if (answer == 'N' || answer == 'n'){
				discardStr(newEntry);
				return;
			} else if (answer == 'Y' || answer == 'y'){
				addEntry(newEntry);
				return;
			}
		} else {
			if (noAdd == -1){
				addEntry(newEntry);
			} else {
				discardStr(newEntry);
			}
		}
	} else {
		printf("found new file: \"%s\"\n", newEntry->string);
		addEntry(newEntry);
	}
}
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
    	while(tempStorage[j] == ' ' || tempStorage[j] == '	'){
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
				while(tempStorage[j] == ' ' || tempStorage[j] == '	'){
						j++;
				}
				String* newEntry = emptyStr(32);
				//printf("AAA \"%c\" \n", tempStorage[j]);
				if (tempStorage[j] == '"'){
					j++;
					while (tempStorage[j] != '"'){
						appendChar(newEntry, tempStorage[j]);
						j++;
					}
					newEntry->string[newEntry->length - 1] = 'c';
					importEntry(newEntry);
				while (tempStorage[j] != '\0'){
					j++;
					}
				}
			}
		}
		if (tempStorage[j+1] != '\n' && tempStorage[j+1] && mode == FLAG_EMPTY){
			mode = FLAG_GLOB;
			// basically as a failsafe in case other stuff doesn't detect
		}
		if (mode != FLAG_INC){
			while (tempStorage[j] != '\0'){
				if (tempStorage[j] == '{'){
					bracketDepth++;
					//printf("curr mode: %d \n", mode);
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
				mode = FLAG_EMPTY;
				if (mode == FLAG_TDEF){
			}
		}
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
void checkImports(FILE* read){
	char tempStorage[512];
	int j = 0;
	int k = 0;
	while (fgets(tempStorage, 511, read)!= NULL){
		j = 0;
		while(tempStorage[j] == ' ' || tempStorage[j] == '	'){
			j++;
		}
		k=0;
		while (tempStorage[j+k] == inc[k]){
			k++;
			if (inc[k] == '\0'){
				j += k;
				while(tempStorage[j] == ' ' || tempStorage[j] == '	'){
					j++;
				}
				String* newEntry = emptyStr(32);
				if (tempStorage[j] == '"'){
					j++;
					while (tempStorage[j] != '"'){
						appendChar(newEntry, tempStorage[j]);
						j++;
					}
					newEntry->string[newEntry->length - 1] = 'c';
					importEntry(newEntry);
				}
			}
		}
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
	int start = 1;
	int isSpef = 0;
	while (isSpef == 0){
		if (strcmp(args[start], "confirm") == 0){
			confirm = 1;
			start++;
		} else if(strcmp(args[start], "no-add") == 0){
			noAdd = 1;
			start++;
		} else if (strcmp(args[start], "auto-add") == 0){
			noAdd = -1;
			start++;
		} else {
			isSpef = 1;
		}
	}
	for (int i = start; i < argC; i++){
		int currL = strlen(args[i]);
		if (hasEntry(args[i]) == 0){
		       if (args[i][currL-1] == 'c' && args[i][currL-2] == '.'){
			addEntry(buildStr(args[i], currL));
		       } else {
       			printf("file \"%s\" is not a C file\n", args[i]);			       
		       }
		} else {
			printf("file \"%s\" specified twice\n",args[i]);
		}
	}
	struct stat status;
	appendPtr(baseDir, "/", 1);
	int isMain = 0;
	//int open = 0;
	String* readStr;
	String* writeStr;
	for (int i = 0; i < len; i++){
		appendStr(baseDir, &files[i]);
		if (stat(baseDir->string, &status) == -1){
			printf("file %s seems to be unavailable\n", baseDir->string);
			baseDir->length -= files[i].length;
			baseDir->string[baseDir->length] = '\0';
			removeEntry(files, i);
			len--;
			i--;
		} else {
			readStr = cloneStr(baseDir);
			FILE* read = fopen(readStr->string, "r+");
			FILE* write;
			isMain = checkMain(baseDir);
			baseDir->string[baseDir->length - 1] = 'h';
			writeStr = cloneStr(baseDir);
			if (isMain == 0){
				write = fopen(writeStr->string, "w+");
			} else {
				printf("checking imports from main file\n");
				checkImports(read);
				baseDir->length -= files[i].length;
				baseDir->string[baseDir->length] = '\0';
				continue;
			}
			if (read == NULL){
				printf("failed to read file \"%s\"", baseDir->string);
			} else {
					printf("started creating header for %s\n", baseDir->string);
       	  makeHeader(read, write);
					printf("done creating header for %s\n", baseDir->string);
			}
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
			baseDir->length -= files[i].length;
			baseDir->string[baseDir->length] = '\0';
		}
	}
	return 0;
}	
