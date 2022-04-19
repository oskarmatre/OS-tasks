
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


// struct element {
//     pid_t pid;
//     struct element *next; 
// };

// struct list {
//     struct element *head;
//     struct element **tail;  
// };

// void enqueue (struct list *list, struct element *item){
//     item -> next = NULL;
//     printf("%s","\ntest3\n");
//     *list -> tail = item;
//     printf("%s","\ntest4\n");
//     list -> tail = &item-> next; 
// }

// void dequeue (struct list *list, pid_t pid){
//     struct element *start = list -> head;
//     struct element *prev = NULL;
//     int flag = 0; //flag if the item is not in list 
//     while (start -> pid != pid)
//     {
//         if(start == NULL){
//             flag = 1; 
//             break;
//         }
//         prev = start;
//         start =  start -> next;

//     }
//     if(!flag){
//         prev ->next = start->next;
//     }
// }

struct processInfo {
    pid_t pid; 
    char commandline[100][20];
};

struct processInfo activeProcesses[10]= {{0}};

int main() 
{
    // struct list *l;
    // l = malloc(sizeof(l));
    // struct element *e1; //malloc?
    // e1 = malloc(sizeof(e1));
    // e1->pid = 1;
    // // e1->next = NULL;
    // printf("%s","\ntest1\n");
    // enqueue(l,e1);
    // printf("%s","\ntest2\n");
    // struct element *e2;
    // e2 = malloc(sizeof(e2));
    // e2->pid = 2;
    // enqueue(l,e2);
    // printf("pid: %d",l->head->pid);
    // //printf("pid: %d",l->head->next->pid);
    // free(l);
    // free(e1);
    // free(e2);
    while ( 1 ) { //parse input

        for(int i = 0; i<10;i++){
            int status;
            struct stat sts;
            // char buffer[10] = "proc/";
            // strcat(buffer,activeProcesses[i].pid);
            // strcat(buffer,"/");
            // char buffer[1024];
            // snprintf(buffer, sizeof(buffer), "proc/%c/", activeProcesses[i].pid);
            // if (stat(buffer, &sts) == -1 && errno == ENOENT ){
            //     printf("%s","test");
            //     activeProcesses[i].pid = 0;
            // }
            // if(kill(activeProcesses[i].pid, 0) == -1){
            //     //printf("Exit status was: %d\n", 0);
            // }
            
        }
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

        int background = (strcmp(parameters[paramslen],"&"));

            if(strcmp(parameters[0],"cd") == 0){
                chdir(parameters[1]);
            }
            else if(strcmp(parameters[0],"jobs") == 0){
                int jobs = 0;
                for(int i = 0; i<10;i++){
                    if(activeProcesses[i].pid != 0){
                        jobs = 1;
                        printf("pid %d: ",activeProcesses[i].pid);
                                for (int j = 0; j <= paramslen; j++) {
                                printf("Command line %s : ",activeProcesses[i].commandline[j]);
                            }
                        
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
                            }
                            break;
                        }
                    }
                    }
                    if (WIFEXITED(status)){
                        int exitstatus = WEXITSTATUS(status);
                        printf("Exit status was: %d\n", exitstatus);
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
                        //strcpy(parameters[min],NULL);
                    }
                    // printf("%s", parameters[length-2]);

                    if(background == 0){
                        paramslen--;
                    }
                    
                    for (int i = 0; i <= paramslen; i++) {
                        strcpy(parameters2[i],parameters[i]);
                    }
                    parameters2[paramslen+1] = NULL;
                    // printf("paramlen %d \n",paramslen);
                    // for (int i = 0; i <= 10; i++) {
                    //     printf("param %d: %s\n", i,parameters2[i]);
                    // }
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