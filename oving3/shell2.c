
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
int read_command ()
{
    char parameters[100][20];
    char arg[100];
    char c;
    int arglen = 0;
    int paramslen = 0;
    for( int i = 0; i<100;i++){
        memset(parameters[i],0,100);
    }
    memset(arg,0,100);
    while(1){
        scanf("%c", &c);

        if(feof(stdin) != 0){
            printf("%s", "exited program");
            exit(0);
        }
        if(c == '\n'){
            strcpy(parameters[paramslen],arg);
            break;
        }
        if(c == ' ' || c == '\t'){
            strcpy(parameters[paramslen],arg);
            memset(arg,0,100);
            paramslen++;
            arglen = 0;
            continue;
        }
        printf(" entered: %c \n",c);
        arg[arglen] = c;
        arglen++;
    }
    return 0;
}

void type_prompt()
{
    static int first_time = 1;
    if ( first_time ) {
        const char* CLEAR_SCREEN_ANSI = " \e[1;1H\e[2J"; //clear screen
        write(STDOUT_FILENO,CLEAR_SCREEN_ANSI,12);
        first_time = 0;
    }

    char path[151];
    char *cwd = getcwd(path, 150);
    
    printf("%s: ", cwd);    //display prompt
}


int main() 
{
    
    pid_t background_pid = 0;
    while ( 1 ) { //parse input
        //break;
        type_prompt();
        char parameters[100][20];
        char arg[100];
        char c;
        int arglen = 0;
        int paramslen = 0;
        for( int i = 0; i<100;i++){
            memset(parameters[i],0,100);
        }
        memset(arg,0,100);
        while(1){
            scanf("%c", &c);

            if(feof(stdin) != 0){ // avslutte ved ctrl+d
                printf("%s", "exited program");
                exit(0);
            }
            if(c == '\n'){
                strcpy(parameters[paramslen],arg);
                break;
            }
            if(c == ' ' || c == '\t'){ //splitte argumenter på mellomrom eller tab
                strcpy(parameters[paramslen],arg);
                memset(arg,0,100);
                paramslen++;
                arglen = 0;
                continue;
            }
            arg[arglen] = c;
            arglen++;
        }

        char *parameters2[20]; // execvp tar bare inn et char* array ikke  char[][]
        for (int i = 0; i <= paramslen; i++) {
             parameters2[i] = (char*) malloc(sizeof(parameters2[i]));
        }
        printf("paramlen %d",paramslen);
        for (int i = 0; i <= paramslen; i++) {
              printf("param %d: %s\n", i,parameters[i]);
        }
        int background = (strcmp(parameters[paramslen],"&"));

            if(strcmp(parameters[0],"cd") == 0){
                chdir(parameters[1]);
            }
            else{
                pid_t pid = fork();
                if (background == 0) {
                    background_pid = pid;
                }
                if (pid == 0) { //Start of child
                    int in = 0;
                    int out = 0;
                    // in endrer inputen og out endrer outputen
                    for(int i = 0; i<=paramslen; i++){
                        if(strcmp(parameters[i], "<") == 0){
                            in = i;
                            printf("%s","found <");
                        }
                        if(strcmp(parameters[i], ">") == 0){
                            out = i;
                            printf("%s","found >");
                        }
                    }
        
                    if(in){
                        int fd0 = open(parameters[in+1], O_RDONLY);
                        dup2(fd0, STDIN_FILENO);
                        close(fd0);
                    }
                    if(out){
                        int fd1 = creat(parameters[out + 1] , 0644) ;
                        dup2(fd1, STDOUT_FILENO);
                        close(fd1);
                    }
                    
                    printf("in: %d, out: %d",in,out);
                    if(in || out){
                        int min;
                        if (in < out) {
                            if (in) {
                                min = in;
                            }
                            else {
                                min = out;
                            }
                        }
                        else {
                            if(out){
                                min = out;
                            }
                            else {
                                min = in;
                            }
                        }
                        //printf("%s", parameters[0]);
                        paramslen = min -1;
                        for (int i = 0; i <= paramslen; i++) {
                            printf("new param %d: %s\n", i,parameters[i]);
                        }
                        //strcpy(parameters[min],NULL);
                    }
                    // printf("%s", parameters[length-2]);

                    if(background == 0){
                        printf("%s", "inneholder &\n");
                        paramslen--;
                        for (int i = 0; i <= paramslen; i++) {
                            strcpy(parameters2[i],parameters[i]);
                        }
                        parameters2[paramslen+1] = NULL;
                        execvp(parameters[0],parameters2);
                    }
                    else{
                        printf("%s", "inneholder ikke &\n");
                        for (int i = 0; i <= paramslen; i++) {
                            strcpy(parameters2[i],parameters[i]);
                        }
                        parameters2[paramslen+1] = NULL;
                        // printf("paramlen %d \n",paramslen);
                        // for (int i = 0; i <= 10; i++) {
                        //     printf("param %d: %s\n", i,parameters2[i]);
                        // }
                        execvp(parameters[0],parameters2); // The first argument in parameters, by convention, should point to the filename associated with the file being executed
                    }
                    //End of Child
                }
                else{ //PARENT
                    if(background != 0){ // Ingen &
                        wait(NULL);
                    }
                    //inneholder & -> skal ikke vente
                }
            }
        for (int i = 0; i <= paramslen; i++) {
             free(parameters2[i]);
        }
    }
    return 0;
}