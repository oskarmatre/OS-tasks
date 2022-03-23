#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sem.h"
#include "bbuffer.h"
#include <pthread.h>

#define PORT 6789
#define MAXREQ (4096 * 1024)

char buffer[MAXREQ], body[MAXREQ], msg[MAXREQ];
void error(const char * msg) {

  perror(msg);

  exit(1);
}
void *doWork(char path[50], int port, BNDBUF *bbuffer){
  printf("%s \n", "bruh1");
  serverSocket(path, port, bb_get(bbuffer));
  printf("%s \n", "bruh2");
}

int main() {
  char path[50];
  int port = 0;
  int threads;
  int bufferslots;
  while (!port)

  {
    scanf("%s %d %d %d", &path, &port, &threads, &bufferslots);
  }

  struct BNDBUF * bbuffer = bb_init(bufferslots);

  int newsockfd;
  int n;
  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) error("ERROR opening socket");
  int true = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, & true, sizeof(int));
  struct sockaddr_in serv_addr, cli_addr;
  bzero((char * ) & serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr * ) & serv_addr, sizeof(serv_addr)) < 0)

  {
    error("ERROR on binding");
  }

  listen(sockfd, 5);

  bb_add(bbuffer, sockfd);

  pthread_t thr;
  
  struct connectionInfo {
     char path[50];
     int port;
     BNDBUF *buffer;
  };

  struct connectionInfo info;
  info.path[50] = path;
  info.port = port;
  info.buffer = bbuffer;
  
  serverSocket(path, port, bb_get(bbuffer));

  pthread_t threads1[threads];
      int  iret1, iret2;
      for(int i = 0; i< threads; i++){
        pthread_create( &threads1[i], NULL, doWork, (void*) &info);
        pthread_join( threads1[i], NULL); 
      }
}

void serverSocket(char path[50], int port, int sockfd) {  

  int newsockfd, n;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  while (1)
  {
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr * ) & cli_addr, &
      clilen);

    if (newsockfd < 0)

      error("ERROR on accept");

    bzero(buffer, sizeof(buffer));
    n = read(newsockfd, buffer, sizeof(buffer) - 1);
    if (n < 0)

      error("ERROR reading from socket");

    char * htmlstring = 0;
    long length;
    FILE * fp;
    fp = fopen(path, "r");

    if (fp)
      if (fp) {
        fseek(fp, 0, SEEK_END);
        length = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        htmlstring = malloc(length);

        if (htmlstring)

        {
          fread(htmlstring, 1, length, fp);
        }

        fclose(fp);
      }
      
    snprintf(body, sizeof(body),
      htmlstring, buffer);
    snprintf(msg, sizeof(msg),
      "HTTP/1.0 200 OK\n"
      "Content-Type: text/html\n"
      "Content-Length: %d\n\n%s",

      strlen(body), body);
    n = write(newsockfd, msg, strlen(msg));

    if (n < 0) error("ERROR writing to socket");

    close(newsockfd);
  }
    
}