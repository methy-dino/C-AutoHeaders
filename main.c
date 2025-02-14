#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "libs/string.h"

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
	for(int i = 0; i < len; i++){
		appendStr(baseDir, &files[i]);
		if (stat(baseDir->string, &status) == -1){
			printf("file %s seems to be unavailable\n", baseDir->string);
			baseDir->length -= files[i].length;
			baseDir->string[baseDir->length] = '\0';
			removeEntry(files, i, len);
			len--;
			i--;
		} else {
			baseDir->length -= files[i].length;
			baseDir->string[baseDir->length] = '\0';
		}
	}
	return 0;
}
