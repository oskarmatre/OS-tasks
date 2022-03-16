#include<stdio.h>
#include<time.h>
#include "main.h"
int main(){
	char c;
	time_t t = time(NULL);
	  struct tm tm = *localtime(&t);
	printf("Welcome to the alarm clock! The time is\n");
	printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1,
		       	tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        printf("Please enter \"s\" (schedule), \" l \" (list), \"c\" (cancel), \"x\" (exit)");
	scanf("%c",&c);
	switch(c){
		case 's':
			schedule();
			break;
		case 'l':
			list();
			break;
		case 'c':
			cancel();
			break;
		case 'x':
			exit_program();
			break;
		default:
			printf("not a valid option");
			break;
	}

	return 0;
}


void schedule(){
	printf("schedule");
}
void list(){
	printf("list");
}
void cancel(){
	printf("cancel");
}
void exit_program(){
	printf("exit");
}
