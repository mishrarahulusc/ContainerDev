#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PORTNO 5234
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[2000];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
    // if (argc < 2) {
       //  fprintf(stderr,"ERROR, no port provided\n");
      //   exit(1);
    // }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     //portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(PORTNO);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,1);
     clilen = sizeof(cli_addr);
     while(1) {

         newsockfd = accept(sockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);
          if (newsockfd < 0)
          error("ERROR on accept");
     bzero(buffer,2000);int in, index = 0 ,limit = 2000;

        while (( n = recv(newsockfd,&buffer[index],limit,0)) > 0 ) {
            index  += n;
            limit -= n;
            printf("packet received:%s\n",buffer);

         }
     //if (n < 0) error("ERROR reading from socket");
         printf("Packet received on server: %s\n",buffer);
         n = send(newsockfd,buffer,sizeof(buffer),0);
        // if (n < 0) error("ERROR writing to socket");
         close(newsockfd);
    }
     close(sockfd);
     return 0;
}

