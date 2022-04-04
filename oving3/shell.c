
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int read_command ( char cmd[], char *par[] )
{
    char line[1024];
    int count = 0, i = 0, j = 0;
    char *array[100];
    char *pch;
    // for (int i = 0; i < sizeof(*par); i++) {
    //     //par[i] = "0";
    //     //printf("%ld ", sizeof(*par[i]));
    //     //memset(par[i], 0, sizeof(*par[i]));
    //     printf("%s ", par[i]);
    // }
    while (1) {
        int c = fgetc ( stdin );
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
    for ( int j = 0; j < i; j++ ) 
        par[j] = array[j];
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
    
    printf("%s > ", cwd);    //display prompt
}


int main() 
{
    char cmd[100], command[100];
    
    //memset(parameters, 0, sizeof(parameters));
    //parameters[20] = malloc(sizeof(char)*20);
    
    
    //execl(path, NULL);
    // char *envp[] = { (char *) "PATH=/BIN", 0 };
    while ( 1 ) {
        char *parameters[20];
        //*parameters = calloc(parameters,20);
        //printf("%s", parameters[0]);
        //printf("%s", parameters[1]);
        for (int i = 0; i < 2; i++) {
            // memset(parameters[i], '$', sizeof(parameters[i]));
             parameters[i] = (char*) malloc(sizeof(parameters[i]));
            //printf("%s", parameters[i]);
        }
        //printf("%s", parameters[1]);
        type_prompt();
        int length = read_command ( command, parameters );
        if(strcmp(command,"cd") == 0){
            chdir(parameters[1]);

            }
            else{
            //execl(command,parameters[1],NULL);
                pid_t pid = fork();
                // char *args[20];
                // for(int i = 0; i<20;i++){
                //     if(parameters[i] != NULL){
                //         strcpy(args[i], parameters[i]);
                //         // args[i][] = parameters[i];
                //     }
                // }
                if (pid == 0) {
                    parameters[length-1] = NULL;
                    execv(command,parameters); // The first argument in parameters, by convention, should point to the filename associated with the file being executed
                }
                else{
                    wait(NULL);
                }
            }
        for (int i = 0; i < 2; i++) {
             free(parameters[i]);
        }
        /*
        pid_t pid = fork();
        if (pid < 0)
        {
            printf("%s", "could not fork");
            return 0;
        }
        else if(pid > 0){
            int status;
            if(waitpid(pid,&status,0) == -1){
                printf("%s","failed");
                exit(EXIT_FAILURE);
            }
            if(WIFEXITED(status)){
                int es = WEXITSTATUS(status);
                printf("%s, %d \n", "Exit status was", es);
            }
        }
        else if(pid == 0){
            if(strcmp(command,"cd") == 0){
            chdir(parameters[1]);

            }
            else{
            //execl(command,parameters[1],NULL);
                execv(command,parameters); // The first argument in parameters, by convention, should point to the filename associated with the file being executed
            }
            exit(0);
        }
        else{
            wait(NULL);
            return 0;
        }
        */
        //Kan det hende at det går galt på forken!😦🐱‍🐉🐱‍👓🐱‍🚀
        
        //    for (int i = 0; i < 20; i++) {
        //     // memset(parameters[i], '$', sizeof(parameters[i]));
        //     free(parameters[i]);
           
        // }

    }
        
        
        
        // if ( fork() != 0 ) 
        //     wait ( NULL );
        // else {
        //     strcpy ( cmd, "/bin/" ); //copies /bin/ to cmd
        //     strcat ( cmd, command ); //concatinates (joins) command with cmd 
        //     execve ( cmd, parameters, envp ); //execute command
        
        // if ( strcmp (command, "exit" ) == 0) 
        //     break;
    

    return 0;
}