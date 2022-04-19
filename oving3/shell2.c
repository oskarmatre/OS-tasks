
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>


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

//Info about the current running backround processes 
struct processInfo {
    pid_t pid; 
    char commandline[100][20];
};

//limit it to 10, as for a simple shell, no more than 10 background processes are needed 
struct processInfo activeProcesses[10]= {{0}};

int main() 
{

    while ( 1 ) {

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

        while(1){ //parse input
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
        
        //Checks if process is active, removes it from active list if it is not active
        for(int i = 0; i<10;i++){
            if(activeProcesses[i].pid != 0){    
                int s;
                int w = waitpid(activeProcesses[i].pid,&s,WNOHANG);
                if(w == activeProcesses[i].pid){
                    activeProcesses[i].pid = 0;
                    printf("Exit status was: %d\n", s);
                }
            }
        }
        char *parameters2[20]; // execvp tar bare inn et char* array ikke  char[][]
        for (int i = 0; i <= paramslen; i++) {
             parameters2[i] = (char*) malloc(sizeof(parameters2[i]));
        }
    //  Sjekker for ampersand & og flagger bakgrunn om den finner det 
        int background = (strcmp(parameters[paramslen],"&"));

            //sjekker for cd 
            if(strcmp(parameters[0],"cd") == 0){
                chdir(parameters[1]);
            }
            //sjekker for jobs
            else if(strcmp(parameters[0],"jobs") == 0){
                int jobs = 0;
                for(int i = 0; i<10;i++){
                    if(activeProcesses[i].pid != 0){
                        jobs = 1;
                        printf("pid %d: ",activeProcesses[i].pid);
                        char command[100] = "";
                        
                                for (int j = 0; j <= paramslen; j++) {
                                    strcat(command,activeProcesses[i].commandline[j]);
                                    strcat(command," ");
                                }    
                        printf("Command line: %s",command);
                            
                    }
                
                }
                if(!jobs){
                    printf("%s","No active jobs\n");
                }
            }
            else{
                pid_t pid = fork();
                if(pid < 0){
                    printf("Could not fork a child. \n");
                }
                else if(pid > 0){
                    int status;
                    
                    if(background != 0){
                        if(waitpid(pid, &status, 0)==1){
                            printf("Waitpid failed \n");
                            exit(EXIT_FAILURE);
                        }
                    }
                    else{
                        for(int i = 0; i< 10; i++){
                        if(activeProcesses[i].pid == 0){
                            activeProcesses[i].pid = pid;
                            for (int j = 0; j <= paramslen; j++) {
                                strcpy(*activeProcesses[j].commandline,parameters[j]);
                                //kopierer kommandolinjen over i ovesikten over aktive prossesser 
                            }
                            break;
                        }
                    }
                    }
                    if (WIFEXITED(status)){
                        int exitstatus = WEXITSTATUS(status);
                        printf("Exit status was: %d\n", exitstatus);
                        //printer exit status 
                    }
                }
                else if (pid == 0) { //Start of child
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
                    
                    if(in || out){ // if < or > is in arguments find position of the first of them, as you only want to execute the command before the symbol
                        int min;
                        if (in < out) {
                            if (in) { //if in < out && in inputted
                                min = in;
                            }
                            else {
                                min = out;
                            }
                        }
                        else {
                            if(out){//if out > in && out is inputted
                                min = out;
                            }
                            else {
                                min = in;
                            }
                        }
                        paramslen = min -1;
                    }

                    if(background == 0){
                        paramslen--;
                    }
                    
                    for (int i = 0; i <= paramslen; i++) {
                        strcpy(parameters2[i],parameters[i]);
                    }
                    parameters2[paramslen+1] = NULL;
                    execvp(parameters[0],parameters2); // The first argument in parameters, by convention, should point to the filename associated with the file being executed
                
                    //End of Child
                }

            }
        for (int i = 0; i <= paramslen; i++) {
             free(parameters2[i]);
        }
        
    }
    return 0;
}