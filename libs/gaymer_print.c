#include <stdio.h>
#define BLUE "\033[38;2;35;35;255m" 
#define RED "\033[38;2;255;0;0m" 
#define YELLOW "\033[38;2;255;255;0m" 
#define ORANGE "\033[38;2;255;128;0m"
#define PURPLE "\033[38;2;150;25;255m"
#define GREEN "\033[38;2;0;255;0m"
#define GRAY "\033[38;2;176;196;222m"
#define LIGHT_BLUE "\033[38;2;135;206;235m" 

void gaymer_print(char* str){
	int i = 0;
	while (str[i] != '\0'){
		switch ((str[i] + str[i+1]) % 8){
			case 0:
				printf(BLUE);
				break;
			case 1:
				printf(RED);
				break;
			case 2:
				printf(YELLOW);
				break;
			case 3:
				printf(ORANGE);
				break;
			case 4:
				printf(PURPLE);
				break;
			case 5:
				printf(GREEN);
				break;
			case 6:
				printf(GRAY);
				break;
			case 7:
				printf(LIGHT_BLUE);
				break;
		}
		printf("%c", str[i]);
		i++;
	}
	printf("\033[0m");
}
