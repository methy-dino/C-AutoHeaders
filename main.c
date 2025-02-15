#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "libs/string.h"
#define FLAG_STRUCT 1
#define FLAG_TYPEDEF 2
#define FLAG_TRASH -1
#define FLAG_EMPTY 3
#define FLAG_FUNC 4
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
int main(int argC, char**args){
	const char type[8] = "typedef";
	const char struc[] = "struct";
	if (argC == 1){
		printf("no files specified");
		return 0;
	}
	char cwd[256];
	getcwd(cwd, 256);
	String* baseDir = buildStr(cwd,strlen(cwd));
	String* files = (String*) malloc(sizeof(String*)*4);
	int len = 0;
	int size = 4;
	for (int i = 1; i < argC; i++){
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
	int j = 0;
	int k = 0;
	int mode = FLAG_TRASH;
	int bracketDepth = 0;
	//int open = 0;
	String* writer = emptyStr(64);
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
			FILE* read = fopen(baseDir->string, "r+");
			baseDir->string[baseDir->length - 1] = 'h';
			FILE* write = fopen(baseDir->string, "w+");
			if (!read){
				printf("failed to read file \"%s\"", baseDir->string);
			} else {
				char tempStorage[512];
				while (fgets(tempStorage, 511, read)!= NULL){
						printf("test\n");
						j = 0;
						writer->length = 0;
						writer->string[0] = '\0';
						while (tempStorage[j] != '\0'){
							while((mode == FLAG_TRASH || mode == FLAG_EMPTY) && (tempStorage[j] == '	' || tempStorage[i] == ' ')&& tempStorage[j] != '\0'){
								j++;
								mode = FLAG_EMPTY;		
							}
							k=0;
							while ((mode == FLAG_TRASH || mode == FLAG_EMPTY) && tempStorage[j+k] == type[k]){
								k++;
								if (type[k] == '\0'){
									mode = FLAG_TYPEDEF;
								}
								if (tempStorage[j+k] == '\0'){
									break;
								}
							}
							k=0;
							while ((mode == FLAG_TRASH || mode == FLAG_TYPEDEF || mode == FLAG_EMPTY) && tempStorage[j+k] == struc[k]){
								k++;
								if (struc[k] == '\0'){
									mode = FLAG_STRUCT;
								}
								if (tempStorage[j+k] == '\0'){
									break;
								}

							}
							if (tempStorage[j] == '{'){
								bracketDepth++;
								if (mode == FLAG_TRASH && bracketDepth == 1){
									appendNoLen(writer, tempStorage, 510);
									mode = FLAG_FUNC;
									writer->length -= 2;
									writer->string[writer->length] = '\0';
									fwrite(writer->string,1,writer->length,write);
									break;
								}
							} else if (tempStorage[j] == '}'){
								bracketDepth--;
								if (bracketDepth == 0 && mode != FLAG_FUNC){
									mode = FLAG_TRASH;
									break;
								}
							}
							j++;
						}
						printf("AAA\n");
						if (mode != FLAG_TRASH && mode != FLAG_EMPTY && mode != FLAG_FUNC){
							printf("B\n");
							appendNoLen(writer, tempStorage, 510);
							printf("C\n");
							fwrite(writer->string, 1, writer->length, write);
							printf("RAAAAAAH\n");
						}
						if (mode == FLAG_TRASH && bracketDepth == 0){
							printf("D\n");
							appendNoLen(writer, tempStorage, 510);
							printf("E\n");
							fwrite(writer->string, 1, writer->length, write);
						}
						if (mode == FLAG_FUNC && bracketDepth == 0){
							mode = FLAG_TRASH;
						}
						printf("huh\n");
					}
			}
			printf("hello");
			baseDir->length -= files[i].length;
			baseDir->string[baseDir->length] = '\0';
		}
	}
	return 0;
}
