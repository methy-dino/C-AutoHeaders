#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#define FORCE_BREAK 2
typedef struct string{
	char* string;
	unsigned int length;
	unsigned int maxCapacity;
}String;
void growStr(String* str, unsigned int inc){
	unsigned int newL = inc + str->maxCapacity;
	char* nStr = (char*)malloc(newL);
	for (unsigned int i = 0; i < str->length; i++){
		nStr[i] = str->string[i];
	}
	free(str->string);
	str->string = nStr;
    str->maxCapacity = newL;
	str->string[str->length] = '\0';
}
void growStrClean(String* str, int inc){
	unsigned int newL = inc + str->maxCapacity;
	char* nStr = (char*)calloc(newL, newL);
	for (unsigned int i = 0; i < str->length; i++){
		nStr[i] = str->string[i];
	}
	free(str->string);
	str->string = nStr;
    str->maxCapacity = newL;
	str->string[str->length] = '\0';
}
String* emptyStr(unsigned int allocSize){
	String* string  = (String*)malloc(sizeof(struct string));
	string->maxCapacity = allocSize;
	string->length = 0;
	string->string = (char*)malloc(string->maxCapacity);
	return string;
}
String* ptrToStr(char* ptr){
	String* toRet = emptyStr(32);
	unsigned int i = 0;
	while (ptr[i] != '\0'){
		toRet->string[toRet->length] = ptr[i];
		toRet->length++;
		if (toRet->length == toRet->maxCapacity - 1){
			growStr(toRet, 6);
		}
        i++;
	}
	toRet->string[toRet->length] = '\0';
	return toRet;
}
/* I don't know why someone would want to initialize like this.
 * It is here anyways.
*/
String* charArrToStr(char arr[], unsigned int length){
	String* string  = (String*)malloc(sizeof(struct string));
	string->maxCapacity = length*1.5+1;
	string->length = length;
	string->string = (char*)malloc(string->maxCapacity);
	for (unsigned int i = 0; i < length; i++){
		string->string[i] = arr[i];
	}
	string->string[string->length] = '\0';
	return string;
}

String* initStr(char* rawStr, unsigned int rawStrLen){
	String* string  = (String*)malloc(sizeof(struct string));
	string->maxCapacity = rawStrLen;
	string->length = rawStrLen;
	string->string = rawStr;
	return string;
}

String* buildStr(char* pointer, unsigned int length){
		String* string  = (String*)malloc(sizeof(struct string));
		string->maxCapacity = length*1.5+1;
		string->length = length;
		string->string = (char*)malloc(string->maxCapacity);
	for (unsigned int i = 0; i < length; i++){
		string->string[i] = pointer[i];
	}
	string->string[string->length] = '\0';
	return string;
}
void appendArr(String* str, char chars[], unsigned int arrL){
	if (str->maxCapacity < str->length + arrL){
		growStr(str, (str->length+1) / 2);
	}
	for (unsigned int i = 0; i < arrL; i++){
		str->string[str->length] = chars[i];
		str->length++;
	}
	str->string[str->length] = '\0';
}
void appendSubPtr(String* str, char* ptr, int start, int end){
	if (str->maxCapacity < str->length + (end-start)+1){
		 growStr(str, (end-start) * 1.5);
	}
	for (int i = start; i < end; i++){	
		str->string[str->length] = ptr[i];
		str->length++;
	}
	str->string[str->length] = '\0';
}
void appendChar(String* str, char ch){
	if (str->length == str->maxCapacity-1){
		growStr(str, 6);
	}
	str->string[str->length] = ch;
	str->length++;
	str->string[str->length] = '\0';
}
int appendNoLen(String* str, char* ptr, unsigned int max){
	unsigned int i = 0;
	while (ptr[i] != '\0'){
		if (str->length == str->maxCapacity){
			growStr(str, 5);
		}
		str->string[str->length] = ptr[i];
		str->length++;
		i++;
		if (max != 0 && i == max){
			return FORCE_BREAK;
		}
	}
	return 0;
}
void appendPtr(String* str, char* ptr, unsigned int ptrLen){
	if (str->maxCapacity < str->length + ptrLen){
		 growStr(str, ptrLen * 1.5);
	}
	for (unsigned int i = 0; i < ptrLen; i++){	
		str->string[str->length] = ptr[i];
		str->length++;
	}
	str->string[str->length] = '\0';
}
void appendHeapPtr(String* str, char* ptr, unsigned int ptrLen){
	if (str->maxCapacity < str->length + ptrLen){
		 growStr(str, ptrLen * 1.5);
	}
	for (unsigned int i = 0; i < ptrLen; i++){
		if (str->length == str->maxCapacity){
			growStr(str, (str->length+1) / 2);   
		}
		str->string[str->length] = ptr[i];
		str->length++;
	}
	str->string[str->length] = '\0';
	free(ptr);
}

void appendStr(String* str, String* toAppend){
	// avoid unnecessary grow checks
	if (str->maxCapacity < str->length + toAppend->length){
	 growStr(str, toAppend->length * 1.5);
	}
	for (unsigned int i = 0; i < toAppend->length; i++){
		str->string[str->length] = toAppend->string[i];
		str->length++;
	}
	str->string[str->length] = '\0';	
}

String* concatStr(String* str, String* toAppend){
	if (str->maxCapacity < str->length + toAppend->length){
	 growStr(str, toAppend->length * 1.5);
	}
	for (unsigned int i = 0; i < toAppend->length; i++){
		str->string[str->length] = toAppend->string[i];
		str->length++;
	}
	str->string[str->length] = '\0';
	free(toAppend->string);
	free(toAppend);
	return str;	
}
void toUpperCase(String* str){
	// storing the character codes prevents errors due to different standards.
	int Acode = 'A';
	int aCode = 'a';
	for (unsigned int i = 0; i < str->length; i++){
		if (aCode <= str->string[i] && str->string[i] < aCode + 26){
			str->string[i] = Acode + (str->string[i] - aCode);
		}
	}
}
void toLowerCase(String* str){
	// storing the character codes prevents errors due to different standards.
	int Acode = 'A';
	int aCode = 'a';
	for (unsigned int i = 0; i < str->length; i++){
		if (Acode <= str->string[i] && str->string[i] < Acode + 26){
			str->string[i] = aCode + (str->string[i] - Acode);
		}
	}
}
/* start inclusive, end exclusive, returns string built with exact capacity.
*/
String* subStr(String* str, unsigned int start, unsigned int end){
	start = str->length % start;
	end = str->length % end;
	String* ret = malloc(sizeof(String));
	ret->length = end - start;
	ret->maxCapacity = ret->length;
	ret->string = (char*) malloc(sizeof(char) * ret->length);
	for (unsigned int i = 0; i < end-start; i++){
		ret->string[i] = str->string[i+start];
	}
	return ret;
}
/* start is inclusive, end is exclusive, as by default.
 * not to be confused with removeStr(String*, String*)
 * as a safe guard, ints are converted to % str->length 
 */
void removeSubStr(String* str, unsigned int start, unsigned int end){
	start = str->length % start;
	end = str->length % end;
    str->length -= end-start;
	for (unsigned int i = start; i < str->length; i++){
		str->string[i] = str->string[i+end-start];
	}
}

void removeCharAt(String* str, unsigned int index){
	for (unsigned int i = index + 1; i < str->length; i++){
		str->string[i-1] = str->string[i];
	}
	str->length--;
}

void removeChar(String* str, char character){
	unsigned int removed = 0;
	for (unsigned int i = 0; i < str->length-removed; i++){
		while (str->string[i+removed] == character){
			removed++;
		}
		str->string[i] = str->string[i+removed];
	}
	str->length -= removed;
	str->string[str->length] = '\0';
}

void removeStr(String* str, String* subStr){
	unsigned int j = 0;
	unsigned int removed = 0;
	for (unsigned int i = 0; i < str->length; i++){
		j = 0;
		while (str->string[i+j] == subStr->string[j]){	
			j++;
			if (subStr->string[j+1] == '\0'){
				i += j+1;
				removed+= subStr->length;
			}
		}
		str->string[i-removed] = str->string[i]; 	
	}
	str->length-=removed;
	str->string[str->length] = '\0';
}
void removeFirstStr(String* str, String* subStr){
	unsigned int j = 0;
	unsigned int removed = 0;
	for (unsigned int i = 0; i < str->length; i++){
		j = 0;
		while (str->string[i+j] == subStr->string[j]){	
			j++;
			if (j+1 == subStr->length){
				i += j+1;
				removed+= subStr->length;
				while (i < str->length){
					str->string[i-removed] = str->string[i];
				}
				str->length -= removed;
				str->string[str->length] = '\0';
			}
		}
	}
}
void removeLastStr(String* str, String* subStr){
	unsigned int j = 1;
	unsigned int removed = 0;
	for (unsigned int i = str->length; i > subStr->length - 1; i--){
		while (str->string[i - j] == subStr->string[subStr->length - j]){
			j++;
			if (subStr->length == j){
				removed += subStr->length;
				while (i < str->length){
					str->string[i-removed] = str->string[i];
					i++;
				}
				str->length-=removed;
				str->string[str->length] = '\0';
				// loop break.
				return;
			}
		}
	}
}
unsigned int indexOfChar(String* str, char character, int startIndex){
	long start = (str->length + startIndex) % str->length;
	while (start < str->length){
		if (str->string[start] == character){
			return (unsigned int)start;
		}
		start++;
	}
	return -1;
}

unsigned int lastIndexOfChar(String* str, char character, int endOffset){
	long start = (str->length + endOffset - 1) % str->length;
	while (start > -1){
		if (str->string[start] == character){
			return (unsigned int) start;
		}
		start--;
	}
	return -1;
}

unsigned int indexOfStr(String* str, String* subStr, unsigned int startIndex){
	unsigned int start = (str->length + startIndex) % str->length;
	unsigned int i = 0;
	while (start < str->length){
/* must be done in this way, for cases like ("abaabaac", "abaac")
* since the string start can be messed up by an check advancing after it
*  It's possible to check if the start is seen, but it has barely any benefit*/
		while (str->string[start+i] == subStr->string[i]){
			i++;
			if (i+1 == subStr->length){
				return start;
			}
		}
		i = 0;
		start++;
	}
	return -1;
}

unsigned int lastIndexOfStr(String* str, String* subStr, unsigned int endOffset){
	unsigned int start = str->length - (endOffset % str->length);
	unsigned int i = subStr->length;
	while (start > subStr->length - 1){
		while (str->string[start-i] == subStr->string[subStr->length - i]){
			i--;
			if (i == 0){
				return start-subStr->length;
			}	
		}
		start--;
		i = subStr->length;
	}
	return -1;
}
void replaceChar(String* str, char target, char sub){
	for (unsigned int i = 0; i < str->length; i++){
		if (str->string[i] == target){
			str->string[i] = sub;
		}
	}
}
void replaceStr(String* str, String* target, String* sub){
	unsigned int j = 0;
	for (unsigned int i = 0; i < str->length - target->length; i++){
		j = 0;
		while (str->string[i+j] == target->string[j]){
			j++;
			if (j == target->length){
				if (str->maxCapacity < str->length + sub->length - target->length){
					// allocate extra space for less reallocs (heap slow!1!1!1)
					growStr(str, (sub->length - target->length)*2);
				}
				str->length += sub->length - target->length;
				str->string[str->length] = '\0';
				// has to be backwards since we are moving chars further.
				for (unsigned int k = str->length-1; k > i+sub->length-1; k--){
					str->string[k] = str->string[k - sub->length + target->length];
				}
				for (unsigned int k = 0; k < sub->length; k++){
					str->string[i+k] = sub->string[k];
				}
				i += sub->length - target->length;
				
			}
		}
	}
	str->string[str->length] = '\0';
}
void replaceFirstStr(String* str, String* target, String* sub){
	unsigned int j = 0;
	for (unsigned int i = 0; i < str->length - target->length; i++){
		j = 0;
		while (str->string[i+j] == target->string[j]){
			j++;
			if (j == target->length){
				if (str->maxCapacity < str->length + sub->length - target->length){
					//there is no need for extra allocation.
					growStr(str, (sub->length - target->length));
				}
				str->length += sub->length - target->length;
				str->string[str->length] = '\0';
				for (unsigned int k = str->length-1; k > i+sub->length-1; k--){
					str->string[k] = str->string[k - sub->length + target->length];
				}
				for (unsigned int k = 0; k < sub->length; k++){
					str->string[i+k] = sub->string[k];
				}
				i += sub->length - target->length;
				return;
			}
		}
	}
	str->string[str->length] = '\0';
}
void replaceLastStr(String* str, String* target, String* sub){
	unsigned int j = 1;
	for (unsigned int i = str->length; i > target->length-1; i--){
		j = 1;
		while (str->string[i-j] == target->string[target->length - j]){
			j++;
			if (j == target->length+1){
				if (str->maxCapacity < str->length + sub->length - target->length){
					growStr(str, (sub->length - target->length));
				}
				str->length += sub->length - target->length;
				i -= target->length;
				str->string[str->length] = '\0';
				for (unsigned int k = str->length-1; k > i+sub->length-1; k--){
					str->string[k] = str->string[k - sub->length + target->length];
				}
				for (unsigned int k = 0; k < sub->length; k++){
					str->string[i+k] = sub->string[k];
				}
				return;
			}
		}
	}
	str->string[str->length] = '\0';
}
/* returns 1 if the strings are equal, otherwise returns 0.
 * does not compare after String.length, it may contain trash data after that.
 * trash data is non-zeroed and unsanitized.
 */
int strEqual(String* str1, String* str2){
	if (str1->length == str2->length){
		for (int i = 0; i < str1->length; i++){
			if (str1->string[i] != str2->string[i]){
			return 0;
			}
		}
		return 1;
	}
	return 0;
}
unsigned long long evaluateStr(String* str){
	unsigned long long value = 0;
	for (unsigned int i = 0; i < str->length; i++){
		value += str->string[i];
	}
	return value;
}
long long strCompare(String* str1, String* str2){
	unsigned long long str1Val = 0;
	unsigned long long str2Val = 0;
    long long i;
	if (str1->length > str2->length){
		i = str1->length  - 1;
		while (i > str2->length){
			str1Val += str1->string[i];
			i--;
		}
		while (i > -1){
			str1Val += str1->string[i];
			str2Val += str2->string[i];
			i--;
		}
	} else {
		i = str2->length  - 1;
		while (i > str1->length){
			str2Val += str2->string[i];
			i--;
		}
		while (i > -1){
			str1Val += str1->string[i];
			str2Val += str2->string[i];
			i--;
		}
	}
	return str1Val - str2Val;
}
/* clones a string, will not clone content after String.length (Incase you are storing data there) */
String* cloneStr(String* str){
	String* nStr = (String*) malloc(sizeof(String));
	nStr->length = str->length;
	nStr->maxCapacity = str->maxCapacity;
	nStr->string = (char*) malloc(sizeof(char) * nStr->maxCapacity);
	for (unsigned int i = 0; i < nStr->length; i++){
		nStr->string[i] = str->string[i];
	}
	nStr->string[nStr->length] = '\0';
	return nStr;
}
unsigned long long hashStr(void* str){
	unsigned long long value;
	unsigned int charSize = sizeof(char);
	charSize = charSize * 8;
	String* string = (String*) str;
	for (unsigned int i = 0; i < string->length; i++){
		// the bit shift by charSize is to grant compatibility with other charset, such as UTF-16. 
		value = ((value << charSize) | string->string[i]) & 1000000007;
	}
	return value;
}
String* joinStr(String** strings, unsigned int len, String* separator){
	String* joined;
	unsigned int sizes = 0;
	for (unsigned int i = 0; i < len; i++){
		sizes += strings[i]->length;
	}
	sizes += (separator->length - 1) * (len - 1);
	joined = emptyStr(sizes);
	for (unsigned int i = 0; i < len-1; i++){
		appendStr(joined, strings[i]);
		appendStr(joined, separator);
	}
	appendStr(joined, strings[len-1]);
	return joined;
}

String* splitByStr(String* str, String* divisor, unsigned int* len){
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int prev = 0;
	String* toRet = malloc(sizeof(String*) * 8);
	unsigned int alloc = 8;
	*len = 0;
	while (i < str->length - divisor->length){
		while(str->string[i+j] == divisor->string[j]){
			j++;
			if (divisor->string[j] == '\0'){
				toRet[*len] = *subStr(str, prev, i);
				i += j;
				prev = i;
				*len += 1;
				if (*len == alloc){
				alloc += 4;
				String* newRet = malloc(sizeof(String*) * alloc);
				for(unsigned int k = 0; k < *len; k++){
					newRet[k] = toRet[k];
					}
				free(toRet);
				toRet = newRet;
				}
			}
		}
		j = 0;
	}
	toRet[*len] = *subStr(str, prev, str->length);
	*len += 1;
	return toRet;
}
void reduceStr(String* str, unsigned int reduction){
	unsigned int newL = str->maxCapacity - reduction;
	char* newString = (char*) malloc(newL);
	for (unsigned int i = 0; i < newL; i++){
		newString[i] = str->string[i];
	}
	free(str->string);
	str->string = newString;
	str->maxCapacity = newL;
	str->length = newL;
	str->string[newL] = '\0';
}
void trimEnd(String* str){
	reduceStr(str, str->maxCapacity - str->length);
}
/* it is a void* to easier integration to libs with need of free functions. */
void discardStr(void* str){
	free(((String*)str)->string);
	free(str);
}
void debugPrintStr(String* str, int verbosity){
	printf("-  -  -  -\n");
	if (verbosity > 0){
		printf("details of String at: %p\n", (void*) str);
        printf("it's capacity is %u", str->maxCapacity);
        printf(" of which %u are within the string's length\n", str->length);
	} else {
		printf("details of a String\n");
	}
	if (verbosity < 2){
		printf("it's contents are: \"%s\"\n", str->string);
	} else {
		size_t i = 0;
		size_t limit = 0;
		if (verbosity == 2){
			limit = str->length + 1;
		} else {
			limit = str->maxCapacity;
		}
		printf("it's contents are: \"");
		while (i < limit){
			switch (str->string[i]){
				case '\0':
					printf("\'\\0\'");
					break;
				case '\n':
					printf("\'\\n\'");
					break;
				case '\r':
					printf("\'\\r\'");
					break;
				case '\t':
					printf("\'\\t\'");
					break;
				case '\v':
					printf("\'\\v\'");
					break;
				default: 
					printf("%c", str->string[i]);
					break;
			}
            i++;
		}
        printf("\"\n");
	}
	printf("-  -  -  -\n");
}
