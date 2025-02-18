#include <stdlib.h>
#include <stdio.h>
#define FORCE_BREAK 2;
typedef struct string{
	char* string;
	unsigned int length;
	unsigned int maxCapacity;
}String;
void growStr(String* str, unsigned int inc);
void growStrClean(String* str, unsigned int inc);
String* emptyStr(unsigned int allocSize);/* I don't know why someone would want to initialize like this.
 * It is here anyways.
*/
String* charArrToStr(char arr[], unsigned int length);
String* initStr(char* rawStr, unsigned int rawStrLen);
String* buildStr(char* pointer, unsigned int length);
void appendArr(String* str, char chars[], unsigned int arrL);
void appendChar(String* str, char ch);
int appendNoLen(String* str, char* ptr, unsigned int max);
void appendPtr(String* str, char* ptr, unsigned int ptrLen);
void appendHeapPtr(String* str, char* ptr, unsigned int ptrLen);
void appendStr(String* str, String* toAppend);
String* concatStr(String* str, String* toAppend);/* start inclusive, end exclusive, returns string built with exact capacity.
*/
String* subStr(String* str, unsigned int start, unsigned int end);/* start is inclusive, end is exclusive, as by default.
 * not to be confused with removeStr(String*, String*)
 * as a safe guard, ints are converted to % str->length 
 */
void removeSubStr(String* str, unsigned int start, unsigned int end);
void removeCharAt(String* str, unsigned int index);
void removeChar(String* str, char character);
void removeStr(String* str, String* subStr);
void removeFirstStr(String* str, String* subStr);
void removeLastStr(String* str, String* subStr);
long indexOfChar(String* str, char character, int startIndex);
long lastIndexOfChar(String* str, char character, int endOffset);
unsigned int indexOfStr(String* str, String* subStr, int startIndex);
unsigned int LastindexOfStr(String* str, String* subStr, int endOffset);
void replaceChar(String* str, char target, char sub);
void replaceStr(String* str, String* target, String* sub);
void replaceFirstStr(String* str, String* target, String* sub);
void replaceLastStr(String* str, String* target, String* sub);
/* returns 1 if the strings are equal, otherwise returns 0.
 * does not compare after String.length, it may contain trash data after that.
 * trash data is non-zeroed and unsanitized.
 */
int strEqual(String* str1, String* str2);
int strEqualPtr(String* str, char* ptr);
unsigned long long evaluateStr(String* str);
long long strCompare(String* str1, String* str2);
void toUpperCase(String* str);
void toLowerCase(String* str);
/* clones a string, will not clone content after String.length (Incase you are storing data there) */
String* cloneStr(String* str);
unsigned long long hashStr(void* str);
String* joinStr(String* strings, unsigned int len, String* separator);
String* splitByStr(String* str, String* divisor, unsigned int* len);
void reduceStr(String* str, unsigned int reduction);
void trimEnd(String* str);
void discardStr(String* str);   
