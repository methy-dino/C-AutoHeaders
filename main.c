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
#define DEF_BIT 4
#define INC_BIT 3
#define FUNCTION_BIT 1
#define GLOB_BIT 2
#define TDEF_BIT 5
#define RESET 0
#define VAR_BIT 6
#define COM_BIT 7
/* data used for confirm prompt*/
#define CHECK_BIT(data, quantity) ((data >> quantity) & 1)
#define SET_BIT(data, bit) (data | (1 << bit))
#define UNSET_BIT(data, bit) (data & (~(1 << bit)))
#define CHECK_FN_COM(data) (CHECK_BIT(data, FUNCTION_BIT) || CHECK_BIT(data, COM_BIT))
#define FILE_IND 0
#define FUNC_IND 1
#define DEF_IND 2
/* dum dum */
String** files;
int f_len = 0;
int f_size = 4;
unsigned int confirm = 0;
unsigned int no_add = 0;
int read_head = 1;
int read_only = 0;
unsigned int cwd_len = 0;
void grow_arr(int inc){
	String** n_arr = malloc(sizeof(String*) * (f_size + inc));
	int i = 0;
	for (i = 0; i < f_size; i++){
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
	int i = 0;
	for (i = 0; i < f_len; i++){
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
	const char uni[] = "union";
	const char enu[] = "enum";
	const char def[] = "#define";
	const char inc[] = "#include";
String* baseDir = NULL;
char confirm_prompt(char conf_bit){
		fflush(stdout);
		fd_set input;
    struct timeval timeout;
		char answer[64];
		int read_status = 0;
		size_t total;
	while (1){
		FD_ZERO(&input);
		FD_SET(STDIN_FILENO, &input);
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		read_status = select(1, &input, NULL, NULL, &timeout);
		if (read_status){
			total = read(0,&answer, 63);
			if (total != 2){
				printf("Invalid answer, try again (y/n) ");
				fflush(stdout);
				continue;
			}
			if (answer[0] == 'N' || answer[0] == 'n'){
				return 0;
			} else if (answer[0] == 'Y' || answer[0] == 'y'){
				return 1;
			} else {
				printf("Invalid answer, try again (y/n) ");
				fflush(stdout);
			}
		} else {
			if (CHECK_BIT(no_add, conf_bit)){
				printf("\nautomatically denied \n");
				return 0;
			} else {
				printf("\nautomatically confirmed \n");
				return 1;
			}
		}
	}
	return 0;
}
char def_check(char* file_line){
	if (CHECK_BIT(confirm, DEF_IND)){
		printf("found definition:\n%sdo you wish to include it? (y/n) ", file_line);
		return confirm_prompt(DEF_IND);
	}
	return !CHECK_BIT(no_add, DEF_IND);
}
char fn_check(char* file_line){
	if (CHECK_BIT(confirm, FUNC_IND)){
		printf("found function:\n%sdo you wish to include it? (y/n) ", file_line);
		return confirm_prompt(FUNC_IND);
	}
	return !CHECK_BIT(no_add, FUNC_IND);
}
void import_entry(String* newEntry){
	if (has_entry(newEntry) || (no_add == 1 && confirm == 0)){
		return;
	}
	if (CHECK_BIT(confirm, FILE_IND)){
		unsigned int dot = lastIndexOfChar(newEntry, '.', 0);
		newEntry->string[dot] = '\0';
		printf("found new file:\n%s\ndo you wish to include it in the header generation? (y/n) ", newEntry->string);
		fflush(stdout);
		newEntry->string[dot] = '.';
		if (confirm_prompt(FILE_IND) == 1){
			add_entry(newEntry);
		} else {
			discardStr(newEntry);
		}
	} else {
		if (CHECK_BIT(no_add, FILE_IND) == 0){
			printf("found new file: \"%s\"\n", newEntry->string);
			add_entry(newEntry);
		} else {
			discardStr(newEntry);
			return;
		}
	}
}
void makeHeader(FILE* read, FILE* write){
	/* grants autogenerated code to be c99/c89 compliant */
	fputs("/*header automatically generated by autoHead*/\n#pragma once\n", write);
	int j = 0;
	int k = 0;
	/*int com_start = 0;*/
	int mode = 0;
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
		/*if (bracketDepth == 0 && !(CHECK_BIT(mode, COM_BIT))){
			mode = RESET;
		}*/
		k = 0;
		while (tempStorage[j+k] == type[k] && mode == RESET){
			k++;
			if (type[k] == '\0'){
				j += k;
				mode = def_check(tempStorage) ? SET_BIT(mode, TDEF_BIT): SET_BIT(mode, FUNCTION_BIT);
			}
		}
		k = 0;
		while (tempStorage[j+k] == struc[k] && mode == RESET){
			k++;
			if (struc[k] == '\0'){
				j += k;
				mode = def_check(tempStorage) ? SET_BIT(mode, TDEF_BIT): SET_BIT(mode, FUNCTION_BIT);
			}
		}
		k = 0;
		while (tempStorage[j+k] == enu[k] && mode == RESET){
			k++;
			if (enu[k] == '\0'){
				j += k;
				mode = def_check(tempStorage) ? SET_BIT(mode, TDEF_BIT): SET_BIT(mode, FUNCTION_BIT);
			}
		}
		k = 0;
		while (tempStorage[j+k] == uni[k] && mode == RESET){
			k++;
			if (uni[k] == '\0'){
				j += k;
				mode = def_check(tempStorage) ? SET_BIT(mode, TDEF_BIT): SET_BIT(mode, FUNCTION_BIT);
			}
		}
		k = 0;
		while (tempStorage[j+k] == def[k] && mode == RESET){
			k++;
			if (def[k] == '\0'){
				j += k;
				mode = def_check(tempStorage) ? SET_BIT(mode, TDEF_BIT): SET_BIT(mode, FUNCTION_BIT);
			}
		}
		k = 0;
		while (tempStorage[j+k] == inc[k] && mode == RESET){
			k++;
			if (inc[k] == '\0'){
				j += k;
				mode = SET_BIT(mode, INC_BIT);
				while(tempStorage[j] == ' ' || tempStorage[j] == '	'){
						j++;
				}
				String* newEntry = subStr(baseDir, cwd_len, lastIndexOfChar(baseDir, '/', 0) + 1);
				if (tempStorage[j] == '"'){
					j++;
					while (tempStorage[j] != '"'){
						appendChar(newEntry, tempStorage[j]);
						j++;
					}
					newEntry->string[newEntry->length - 1] = 'c';
					import_entry(newEntry);	
				}
			}
		}
		if (tempStorage[j] == '/' && tempStorage[j+1] == '/'){
			if (!CHECK_BIT(mode, FUNCTION_BIT)){
				mode = SET_BIT(mode, INC_BIT);
			}
		} else if (tempStorage[j] == '/' && tempStorage[j+1] == '*') {
			mode = SET_BIT(mode, COM_BIT);
		} else if (tempStorage[j+1] != '\n' && tempStorage[j+1] && mode == RESET && bracketDepth == 0){
			mode = SET_BIT(mode, GLOB_BIT);
			/* basically as a failsafe in case other stuff doesn't detect*/
		}
		if (!CHECK_BIT(mode, INC_BIT)){
			while (tempStorage[j] != '\0'){
				if (tempStorage[j] == '=' && !CHECK_FN_COM(mode)){
					j--;
					while (tempStorage[j] == '	' || tempStorage[j] == ' '){
						j--;
					}
					appendNoLen(toAppend, "extern ", 20);
					appendSubPtr(toAppend, tempStorage, 0, j+1);
					appendPtr(toAppend, ";\n", 2);
					if (def_check(tempStorage)){
							fputs(toAppend->string, write);
					}
					mode = RESET;
					break;
				} else if (tempStorage[j] == '{'){
					bracketDepth++;
					if ((CHECK_BIT(mode, GLOB_BIT)|| mode == RESET) && bracketDepth == 1){
						appendNoLen(toAppend, tempStorage, 512);
						/* sub { by ;*/
						toAppend->string[j] = ';';
						toAppend->string[j+1] = '\0';
						toAppend->length = j+1;
						appendPtr(toAppend, "\n", 1);
						if (fn_check(toAppend->string)){
							fputs(toAppend->string, write);
						}
						mode = SET_BIT(mode, FUNCTION_BIT);
					}
				} else if (tempStorage[j] == '}' && !CHECK_BIT(mode, COM_BIT)) {
					bracketDepth--;
				}
				if (tempStorage[j] == '/' && tempStorage[j+1] == '/') {
					mode = SET_BIT(mode, INC_BIT);
					/* INC_BIT should not be turned on inside checker*/
					break;
				}
					if (tempStorage[j] == '/' && tempStorage[j+1] == '*') {
						mode = SET_BIT(mode, COM_BIT);
				}
				/* it is needed to check if there is a comment started*/
				if (CHECK_BIT(mode, COM_BIT) && tempStorage[j] == '*' && tempStorage[j+1] == '/'){
					mode = UNSET_BIT(mode, COM_BIT);
					if (!CHECK_BIT(mode, FUNCTION_BIT)){
						fputs(tempStorage, write);
						}
				}
				j++; 
			}
		}
		if (CHECK_BIT(mode, FUNCTION_BIT)){
			if (bracketDepth == 0){
				mode = RESET;
			}
		} else if (CHECK_BIT(mode, COM_BIT)){
			fputs(tempStorage, write);
		}
		else if (CHECK_BIT(mode, DEF_BIT)){
			fputs(tempStorage, write);
			if (tempStorage[j-2] != '\\'){
				mode = RESET;
			}
		}else if (CHECK_BIT(mode, TDEF_BIT)){
			fputs(tempStorage, write);
			if (bracketDepth == 0){
				mode = RESET;
			}
		} else if (CHECK_BIT(mode, INC_BIT)){
			fputs(tempStorage, write);
			mode = UNSET_BIT(mode, INC_BIT);
		} else if (CHECK_BIT(mode, GLOB_BIT)){
			fputs(tempStorage, write);
			mode = RESET;
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
	int com = 0;
	while (fgets(tempStorage, 511, read)!= NULL){
		j = 0;
		while(tempStorage[j] == ' ' || tempStorage[j] == '	'){
			j++;
		}
		k=0;
		if (tempStorage[j] == '/' && tempStorage[j+1] == '/') {
			continue;
		}
			if (tempStorage[j] == '*' && tempStorage[j+1] == '/'){
				com = 0;
			}
			if (tempStorage[j] == '/' && tempStorage[j+1] == '*'){
				com = 1;
			}
		while (tempStorage[j+k] == inc[k] && !com){
			k++;
			if (inc[k] == '\0'){
				j += k;
				while(tempStorage[j] == ' ' || tempStorage[j] == '	'){
					j++;
				}
				String* new_entry = subStr(baseDir, cwd_len, lastIndexOfChar(baseDir, '/', 0) + 1);	
				if (tempStorage[j] == '"'){
					j++;
					while (tempStorage[j] != '"'){
						appendChar(new_entry, tempStorage[j]);
						j++;
					}

					new_entry->string[new_entry->length - 1] = 'c';
					import_entry(new_entry);
				} else {
					discardStr(new_entry);
				}
			}
		}
		while (tempStorage[j] != '\0'){
			if (tempStorage[j] == '*' && tempStorage[j+1] == '/'){
				com = 0;
			}
			if (tempStorage[j] == '/' && tempStorage[j+1] == '*'){
				com = 1;
			}
			j++;
		}
	}
}
int main(int argC, char**args){	
	files = (String**) malloc(sizeof(String*)*4);
	int start = 1;
	int is_spef = 0;

	while (is_spef == 0 && argC > start){
		if (strcmp(args[start], "confirm-file") == 0){
			confirm = confirm | (1 << FILE_IND);
			start++;
		} else if (strcmp(args[start], "confirm-func") == 0){
			confirm = SET_BIT(confirm, FUNC_IND);
			start++;
		} else if (strcmp(args[start], "confirm-def") == 0){
			confirm = SET_BIT(confirm, DEF_IND);
			start++;
		} else if (strcmp(args[start], "confirm-all") == 0){
			confirm = confirm | (1 << FILE_IND) | (1 << FUNC_IND) | (1 << DEF_IND);
			start++;
		} else if (strcmp(args[start], "no-add-file") == 0){
			no_add = SET_BIT(no_add, FILE_IND);
			start++;
		} else if (strcmp(args[start], "no-add-func") == 0){
			no_add = SET_BIT(no_add, FUNC_IND);
			start++;
		} else if (strcmp(args[start], "no-add-def") == 0){
			no_add = SET_BIT(no_add, DEF_IND);
			start++;
		} else if (strcmp(args[start], "no-add-all") == 0){
			no_add = no_add | (1 << FILE_IND) | (1 << FUNC_IND) | (1 << FUNC_IND);
			start++;
		} else if (strcmp(args[start], "read-head") == 0){
			read_head = 1;
			start++;
		} else if (strcmp(args[start], "no-read-head") == 0){
			read_head = -1;
			start++;
		} else if (strcmp(args[start], "root-dir") == 0){
			start++;
		if (start >= argC){
			printf("no relative root specified\n");
			exit(0);
		}
		if (baseDir != NULL){
			discardStr(baseDir);
			baseDir == NULL;
		}
		if ((args[start][0]  == '.' && args[start][1] == '/') || args[start][0] != '/'){
			char cwd[256]; 
			getcwd(cwd, 256);
			cwd_len = strlen(cwd);
			baseDir = buildStr(cwd,cwd_len);
			appendSubPtr(baseDir, args[start], 2, strlen(args[start]));
			cwd_len++;
		} else {
			baseDir = ptrToStr(args[start]);
		}
		cwd_len = baseDir->length + 1;
		start++;
		} else if (strcmp(args[start], "read-only") == 0) {
			read_only = 1;
			start++;
		} else {
			is_spef = 1;
		}
	}
	if (baseDir == NULL){
		char cwd[256]; 
		getcwd(cwd, 256);
		cwd_len = strlen(cwd);
		baseDir = buildStr(cwd,cwd_len);
		cwd_len++;
	}
	if (argC == start){
		printf("there seems to be no files in your input\n");
		return 0;
	}
	int i = 0;
	for (i = start; i < argC; i++){
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
	String* readStr;
	String* writeStr;
  FILE* read;
	FILE* write;
	i = 0;
	for (i = 0; i < f_len; i++){
		appendStr(baseDir, files[i]);
		if (stat(baseDir->string, &status) == -1){
			printf("file \"%s\" seems to be unavailable\n", baseDir->string);
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
			read = fopen(baseDir->string, "rb+");
			if (read == NULL){
				printf("failed to read file \"%s\".\n", baseDir->string);	
				continue;
			}
			isMain = checkMain(baseDir);
			baseDir->string[baseDir->length - 1] = 'h';
			if (isMain == 0 && read_only == 0){
				write = fopen(baseDir->string, "w+");
			} else {
				if (isMain == 1){
					hasMain = 1;
					printf("checking imports from main file\n");
				} else {
					printf("checking imports from file \"%s\" \n", baseDir->string);
				}
				checkImports(read);
				baseDir->length -= files[i]->length;
				baseDir->string[baseDir->length] = '\0';
				continue;
			}
					printf("started creating header for %s\n", baseDir->string);
       	  makeHeader(read, write);
					printf("done creating header for %s\n", baseDir->string);
			if (write != NULL){
				fclose(write);
				write = NULL;
			}
			if (read != NULL){
				fclose(read);
				read = NULL;
			}
			baseDir->length -= files[i]->length;
			baseDir->string[baseDir->length] = '\0';
		}
	}
	if (hasMain){
		printf("this should compile your project (in your current directory): \ngcc -o3 -o exec ");
		unsigned int i = 0;
		for (i = 0; i < f_len; i++){
			printf("\"%s\" ", files[i]->string);
		}
		printf("\n");
	}
	return 0;
}
