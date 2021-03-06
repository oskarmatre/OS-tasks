#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

void read_command ( char cmd[], char *par[] )
{
    char line[1024];
    int count = 0, i = 0, j = 0;
    char *array[100], *pch;

    for ( ;; ) {
        int c = fgetc ( stdin );
        line[count++] = (char) c;
        if ( c == '\n' ) break;
    }

    //Function stops if the only input is newline i.e the user clicked enter without inputting anything
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
    
    printf("%s > ", path);    //display prompt
}


int main() 
{
    char cmd[100], command[100], *parameters[20];

    char *envp[] = { (char *) "PATH=/BIN", 0 };
    while ( 1 ) {
        type_prompt();
        read_command ( command, parameters );
        printf("%s \n",parameters[1]);
        char strings[20][20];
        for(int i = 0; i<19; i++){
            if(parameters[i+1] == NULL){
                break;
            }
            // strings[i] = parameters[i];
            strcpy(strings[i], parameters[i+1]);
        }
        //printf("%s \n",strings[0]);

        if ( fork() != 0 ) 
            wait ( NULL );
        else {
            strcpy ( cmd, "/bin/" ); //copies /bin/ to cmd
            strcat ( cmd, command ); //concatinates (joins) command with cmd 
            execve ( cmd, parameters, envp ); //execute command
        }
        if ( strcmp (command, "exit" ) == 0) 
            break;
    }

    return 0;
}