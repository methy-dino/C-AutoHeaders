#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
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
// dum dum
String** files;
int f_len = 0;
int f_size = 4;
int confirm = 0;
int no_add = -1;
int read_head = 1;
unsigned int cwd_len = 0;
void grow_arr(int inc){
	String** n_arr = malloc(sizeof(String*) * (f_size + inc));
	for (int i = 0; i < f_size; i++){
		n_arr[i] = files[i];
	}
	f_size += inc;
	free(files);
	files = n_arr;
}
void add_entry(String* file){
	if (f_size == f_len){
		grow_arr(8);
	};
	files[f_len] = file;
	f_len++;
}
void remove_entry(int index){
	discardStr(files[index]);
	while(index < f_size-1){
		files[index] = files[index+1];
		index++;
	}
	f_len--;
}
int has_entry(String* entry){
	for (int i = 0; i < f_len; i++){
	//	printf("%d == %s \n", i, files[i]->string);
		if (strEqual(files[i], entry) == 1){
			return 1;
		}
	}
	return 0;
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
void import_entry(String* newEntry){
	//printf("importing entry: %s \n", newEntry->string);
	if (has_entry(newEntry) || (no_add == 1 && confirm == 0)){
		return;
	}
	if (confirm == 1){
		printf("found new file: \"%s\", do you wish to include it in the header generation? (y/n) ", newEntry->string);
		fflush(stdout);
		fd_set input;
    struct timeval timeout;
		char answer = '\0';
		int read_status = 0;
		while (1){
			FD_ZERO(&input);
			FD_SET(STDIN_FILENO, &input);
			timeout.tv_sec = 10;
			timeout.tv_usec = 0;
			read_status = select(1, &input, NULL, NULL, &timeout);
			if (read_status){
				read(0,&answer, 1);
				if (answer == 'N' || answer == 'n'){
					discardStr(newEntry);
					return;
				} else if (answer == 'Y' || answer == 'y'){
					add_entry(newEntry);
					return;
				}
			} else {
				if (no_add == -1){
					printf("\nautomatically added file \n");
					add_entry(newEntry);
					return;
				} else {
					printf("\nautomatically rejected file \n");
					discardStr(newEntry);
					return;
				}
			}
		}
	} else {
		if (no_add == -1){
			printf("found new file: \"%s\"\n", newEntry->string);
			add_entry(newEntry);
		} else {
			discardStr(newEntry);
			return;
		}
	}
}
void makeHeader(FILE* read, FILE* write){
	fputs("//header automatically generated by autoHead\n#pragma once\n", write);
	int j = 0;
	int k = 0;
	int mode = FLAG_EMPTY;
	int bracketDepth = 0;
	String* toAppend = emptyStr(32);
	char tempStorage[512];
    while (fgets(tempStorage, 511, read)!= NULL){	
		j = 0;
		toAppend->length = 0;
		toAppend->string[0] = '\0';
    	while(tempStorage[j] == ' ' || tempStorage[j] == '	'){
			j++;
		}
		if (bracketDepth == 0){
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
				String* newEntry = subStr(baseDir, cwd_len, lastIndexOfChar(baseDir, '/', 0) + 1);
				//printf("AAA \"%c\" \n", tempStorage[j]);
				if (tempStorage[j] == '"'){
					j++;
					while (tempStorage[j] != '"'){
						appendChar(newEntry, tempStorage[j]);
						j++;
					}
					newEntry->string[newEntry->length - 1] = 'c';
					import_entry(newEntry);	
				}
				//while (tempStorage[j] != '\0'){
					//j++;
				//}
			}
		}
		if (mode == FLAG_EMPTY && ((tempStorage[j] == '/' && (tempStorage[j+1] == '/' || tempStorage[j+1] == '*')) || tempStorage[j] == '*')){
			mode = FLAG_INC;
			//while (tempStorage[j] != '\0'){
				//j++;
			//}
		}
		if (tempStorage[j+1] != '\n' && tempStorage[j+1] && mode == FLAG_EMPTY && bracketDepth == 0){
			mode = FLAG_GLOB;
			// basically as a failsafe in case other stuff doesn't detect
		}
		if (mode != FLAG_INC){
			while (tempStorage[j] != '\0'){
				if (tempStorage[j] == '{'){
					bracketDepth++;
					if ((mode == FLAG_GLOB || mode == FLAG_EMPTY) && bracketDepth == 1){
						appendNoLen(toAppend, tempStorage, 512);
						// sub { by ;
						//printf("%d %d\n", j, toAppend->length);
						toAppend->string[j] = ';';
						toAppend->string[j+1] = '\0';
						toAppend->length = j+1;
						appendPtr(toAppend, "\n", 1);
						fputs(toAppend->string, write);
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
		//printf("curr mode: %d \n", mode);
		if (mode == FLAG_DEF){
			fputs(tempStorage, write);
			if (bracketDepth == 0 && tempStorage[j-2] != '\\'){
				mode = FLAG_EMPTY;
			}
		}else if (mode == FLAG_TDEF){
			//printf("%s", tempStorage);
			fputs(tempStorage, write);
			if (bracketDepth == 0){
				mode = FLAG_EMPTY;
			}
		} else if (mode == FLAG_INC){
			fputs(tempStorage, write);
		} else if (mode == FLAG_GLOB){
			fputs(tempStorage, write);
		}
		toAppend->length = 0;
		toAppend->string[0] = '\0';
	}
	discardStr(toAppend);
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
				String* new_entry = subStr(baseDir, cwd_len, lastIndexOfChar(baseDir, '/', 0) + 1);	
				//printf("%s is the new z\n", new_entry->string);
				if (tempStorage[j] == '"'){
					j++;
					while (tempStorage[j] != '"'){
						appendChar(new_entry, tempStorage[j]);
						j++;
					}

					new_entry->string[new_entry->length - 1] = 'c';
					//printf("%s\n", new_entry->string);
					import_entry(new_entry);
				} else {
					discardStr(new_entry);
				}
			}
		}
	}
}
int main(int argC, char**args){
	char cwd[256]; 
	getcwd(cwd, 256);
	cwd_len = strlen(cwd);
	baseDir = buildStr(cwd,cwd_len);
	cwd_len++;
	files = (String**) malloc(sizeof(String*)*4);
	int start = 1;
	int is_spef = 0;
	while (is_spef == 0 && argC > start){
		if (strcmp(args[start], "confirm") == 0){
			confirm = 1;
			start++;
		} else if (strcmp(args[start], "no-add") == 0){
			no_add = 1;
			start++;
		} else if (strcmp(args[start], "auto-add") == 0){
			no_add = -1;
			start++;
		} else if (strcmp(args[start], "read-head") == 0){
			read_head = 1;
			start++;
		} else if (strcmp(args[start], "no-read-head") == 0){
			read_head = -1;
			start++;
		} else {
			is_spef = 1;
		}
	}
	if (argC == start){
		printf("there seems to be no files in your input\n");
		return 0;
	}
	for (int i = start; i < argC; i++){
		int currL = strlen(args[i]);
		String* str = buildStr(args[i], currL);
		if (has_entry(str) == 0){
	    if (args[i][currL-1] == 'c' && args[i][currL-2] == '.'){
				add_entry(str);
		  } else {
  			printf("file \"%s\" is not a C file\n", args[i]);
				discardStr(str);
		  }
		} else {
			printf("file \"%s\" specified twice\n",args[i]);
			discardStr(str);
		}
	}
	struct stat status;
	appendPtr(baseDir, "/", 1);
	int isMain = 0;
	char hasMain = 0;
	//int open = 0;
	String* readStr;
	String* writeStr;
  FILE* read;
	FILE* write;
	for (int i = 0; i < f_len; i++){
		appendStr(baseDir, files[i]);
		if (stat(baseDir->string, &status) == -1){
			printf("file %s seems to be unavailable\n", baseDir->string);
			if (read_head == 1) {
				printf("attempting to read as a standalone header... ");
				baseDir->string[baseDir->length - 1] = 'h';
				read = fopen(baseDir->string, "rb+");
				if (read != NULL){
					printf("read success!\n");
					checkImports(read);
					fclose(read);
				} else {
					printf("read failed!\n");
				}
			}
			baseDir->length -= files[i]->length;
			baseDir->string[baseDir->length] = '\0';
			remove_entry(i);
			i--;
			continue;
		} else {
			//printf("%s\n ",baseDir->string);
			//readStr = cloneStr(baseDir);
			//printf("len val: %u strlen: %u ", readStr->length, strlen(readStr->string));
			
			//readStr->string[40] = '\0';
			read = fopen(baseDir->string, "rb+");
			if (read == NULL){
				printf("failed to read file \"%s\".\n", readStr->string);	
				continue;
			}
			isMain = checkMain(baseDir);
			baseDir->string[baseDir->length - 1] = 'h';
			//writeStr = cloneStr(baseDir);
			if (isMain == 0){
				write = fopen(baseDir->string, "w+");
			} else {
				printf("checking imports from main file\n");
				checkImports(read);
				hasMain = 1;
				baseDir->length -= files[i]->length;
				baseDir->string[baseDir->length] = '\0';
				continue;
			}
					printf("started creating header for %s\n", baseDir->string);
       	  makeHeader(read, write);
					printf("done creating header for %s\n", baseDir->string);
			if (write != NULL){
				//printf("free at line 332\n");
				fclose(write);
				//write = NULL;
			}
			if (read != NULL){
				//printf("free at line 337\n");
				fclose(read);
				//read = NULL;
			}
			//printf("free at line 341\n");
			//discardStr(readStr);
			//printf("free at line 343\n");
			//discardStr(writeStr);
			baseDir->length -= files[i]->length;
			baseDir->string[baseDir->length] = '\0';
		}
	}
	if (hasMain){
		printf("this should compile your project (in your current directory): \n gcc -o3 -o exec ");
		for (unsigned int i = 0; i < f_len; i++){
			printf("%s ", files[i]->string);
		}
		printf("\n");
	}
	return 0;
}
