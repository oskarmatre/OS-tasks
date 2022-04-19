
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int read_command ( char cmd[], char *par[] )
{
    char line[1024];
    int count = 0, i = 0, j = 0;
    char *array[100];
    char *pch;
    while (1) {
        int c = fgetc ( stdin );
        printf("character: %c \n",c);
        break;
        line[count++] = (char) c;
        if ( c == '\n' ) break;
    }
    if ( count == 1 ) return;
    pch = strtok ( line, " \n" );

    //parse line into words
    while ( pch != NULL ) {
        array[i++] = strdup ( pch );
        pch = strtok ( NULL, " \n" );
    }
    // first word is the command
    strcpy ( cmd, array[0] );

    //others are parameters
    for ( int j = 0; j < i; j++ ) {
        par[j] = array[j];}
    par[i] = NULL; //NULL-terminate the parameter list
    return i;
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


void sig_child (int sig) {
    pid_t pid;
    int stat;

    pid = wait(&stat);
    // if() {
    //     kill();
    // }
    printf("\nChild %d is terminated\n", pid);
    return;
}

int main() 
{
    char cmd[100], command[100];
    pid_t background_pid = 0;
    while ( 1 ) {
        char *parameters[20];
        for (int i = 0; i < 20; i++) {
             parameters[i] = (char*) malloc(sizeof(parameters[i]));
        }
        //kill zombies
        
        type_prompt();
       
        
        signal(SIGCHLD, sig_child);
        // pid_t pid_c = waitpid(-1, NULL, WNOHANG);
        // while(!pid_c) {
        //     pid_c = waitpid(-1, NULL, WNOHANG);
        // }
        // if(background_pid){
        //     waitpid(background_pid, NULL, WNOHANG);

        //     printf("%d ",background_pid);
            
        //     int status = kill(background_pid, SIGKILL);
        //     printf("%d ",status);
        //     background_pid = 0;
        // }
        
        int length = read_command ( command, parameters );
        int background = (strcmp(parameters[length-2],"&"));

        // printf("%d \n",background);

        if(strcmp(command,"cd") == 0){
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
                    for(int i = 0; i<length; i++){
                        if(strcmp(parameters[i], "<") == 0){
                            in = i;
                        }
                        if(strcmp(parameters[i], ">") == 0){
                            out = i;
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
                        parameters[min] = NULL;
                    }
                    else{
                    parameters[length-1] = NULL;
                    }
                    // printf("%s", parameters[length-2]);

                    if(background == 0){
                        //printf("%s", "inneholder &\n");
                        parameters[length-2] = NULL;
                        execvp(command,parameters);
                    }
                    else{
                        //fprintf(stderr, "Failed to exec %s\n", command);    
                        execvp(command,parameters); // The first argument in parameters, by convention, should point to the filename associated with the file being executed
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
        for (int i = 0; i < 20; i++) {
             free(parameters[i]);
        }
    }
        

    return 0;
}