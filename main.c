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
#define FLAG_DEF 5
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
int main(int argC, char**args){
	const char type[8] = "typedef";
	const char struc[] = "struct";
	const char def[] = "#define";
	const char inc[] = "#include";
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
	int j = 0;
	int k = 0;
	int mode = FLAG_TRASH;
	int bracketDepth = 0;
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
			baseDir->string[baseDir->length - 1] = 'h';
			 writeStr = cloneStr(baseDir);
			isMain = checkMain(baseDir);
			FILE* write = NULL;
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
				while (fgets(tempStorage, 511, read)!= NULL){
						//printf("test\n");
						j = 0;
						row++;
						writer->length = 0;
						writer->string[0] = '\0';
						while (tempStorage[j] != '\0'){
							k=0;
							while((mode == FLAG_TRASH || mode == FLAG_EMPTY) && (tempStorage[j+k] == '	' || tempStorage[j+k] == ' ')&& tempStorage[j] != '\0'&& j == 0){

							 	k++;
								mode = FLAG_EMPTY;
								//printf("in blank detection\n");		
							}
							j += k;
							if (tempStorage[j] != ' ' && tempStorage[j] != '	' && tempStorage[j] != '\n' && tempStorage[j] != '\0' && mode == FLAG_EMPTY){
								mode = FLAG_TRASH;		
							}
							if (mode == FLAG_DEF){
								while (tempStorage[j] != '\0'){
									j++;
								}
								if (tempStorage[j-1] != '\\'){
									mode = FLAG_TRASH;	
								} else {
									mode = FLAG_DEF;
									if (isMain == 0){
									fwrite(tempStorage, 1, j, write);
									}
								}
								break;
							}
							k=0;
							while ((mode == FLAG_TRASH || mode == FLAG_EMPTY) && tempStorage[j+k] == inc[k]){
								k++;
								if (inc[k] == '\0'){
									j += k;
									mode = FLAG_TRASH;
									//printf("hm2\n");
									while (tempStorage[j] == ' ' || tempStorage[j] == '	'){
										j++;
									}
									//printf("hm3\n");
									if (tempStorage[j] != '<'){
										j++;
										String* newFile = emptyStr(16);
										while (tempStorage[j] != '"'){
											appendChar(newFile, tempStorage[j]);
											j++;																				}
										if (hasEntry(newFile->string, files, len) == 0){
		       									if (newFile->string[newFile->length-1] == 'h' && newFile->string[newFile->length-2] == '.'){
												newFile->string[newFile->length-1] = 'c';
												if (confirm == 0){
													printf("auto-adding %s \n", newFile->string);
													files[len] = *newFile;
													len++;
												} else {
													printf("found new file: %s \n", newFile->string);
													printf("do you wish to add it to header generation? (y/n)  ");
													char ans = '\0';
													while (1 == 1){
														scanf("%c",&ans);
														if (ans == 'y' || ans == 'Y'){
															files[len] = *newFile;
															newFile->string[newFile->length-1] = 'c';
															len++;
															break;
														}
														if (ans == 'n' || ans == 'N'){
															discardStr(newFile);
															break;
														}
													}
												}
													if (len == size){
														files = growArr(files, len, 4);
														size += 4;
													}

		       									}
										}
									break;
									}
								}
								//printf("hm\n");
							}
							k=0;
							while ((mode == FLAG_TRASH || mode == FLAG_EMPTY) && tempStorage[j+k] == def[k]){
								k++;
								if (def[k] == '\0'){
									j+= k;
									while (tempStorage[j] != '\0'){
										j++;
									}
									if (tempStorage[j-2] != '\\'){
										mode = FLAG_TRASH;
									} else {
										mode = FLAG_DEF;
										if (isMain == 0){
											fwrite(tempStorage, 1, j, write);
										}
									}
									break;
								}
								if (tempStorage[j+k] == '\0'){
									break;
								}
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
									writer->length -= 1;
									writer->string[writer->length-1] = '\n';
									writer->string[writer->length] = '\0';
									if (isMain == 0){
										fwrite(writer->string,1,writer->length,write);
									}
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
						//printf("%d\n", row);
						if (mode != FLAG_TRASH && mode != FLAG_EMPTY && mode != FLAG_FUNC && mode != FLAG_DEF && isMain == 0){
							//printf("B\n");
							appendNoLen(writer, tempStorage, 510);
							//printf("C\n");
							fwrite(writer->string, 1, writer->length, write);
							//printf("RAAAAAAH\n");
						}
						//printf("a\n");	
						if (mode == FLAG_TRASH && bracketDepth == 0){
							//printf("D\n");
							appendNoLen(writer, tempStorage, 510);
							//printf("E\n");
							fwrite(writer->string, 1, writer->length, write);
						}
						//printf("b\n");
						if ((mode == FLAG_FUNC) && bracketDepth == 0 && isMain == 0){
							mode = FLAG_TRASH;
						}	
						//printf("huh\n");
					}
			}
			//printf("booo\n");
			if (isMain == 0 && write != NULL){
				fclose(write);
			}
			//printf("testing\n");
			if (read != NULL){
				fclose(read);
			}
			//printf("between\n");
			free(readStr->string);
			free(writeStr->string);
			// mfw I can't free the String*
			//discardStr(writeStr);
			//printf("bah\n");
			printf("done creating header for %s\n", baseDir->string);
			baseDir->length -= files[i].length;
			baseDir->string[baseDir->length] = '\0';
		}
	}
	return 0;
}
