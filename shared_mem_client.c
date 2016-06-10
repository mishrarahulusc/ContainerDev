#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <mqueue.h>
#include <semaphore.h>
#define SEMNAME "/mysem1"
#define MSGOBJ_NAME    "/mqcontainer"
#define MAX_MSG_LEN 8
#define SIZE 4096
#define QPERM 0660
#define MAX_MSGS 10
#define MAX_MSG_SIZE 256
#define MAX_MSG_SIZE_DATA 10
#define MQ_FLAGS  0
#define MQ_CUR_MSGS 0
#define C1QNAME "/myc1queue"
#define QNAME  "/myqueue" 
typedef struct mqb_msg {
     long  mtype ;
     char msgdata[MAX_MSG_SIZE_DATA];
   }mqb_msg;



 mqd_t OpenMsgQ(struct mq_attr *attr , int type ) {
     mqd_t id;
     if (type==1) {
       if ((id = mq_open (C1QNAME, O_RDONLY | O_CREAT, QPERM, attr)) == -1) {
               perror ("Server: mq_open (server)");
             //  exit (1);
       }
     }
     if (type == 2) {
      if ((id = mq_open (QNAME, O_WRONLY)) == 1) {
            perror ("Server: Not able to open client queue");
           // exit(1);
       }
     }

    return id;
 }


 long RcvMsg(mqd_t IdFromRcv , mqb_msg *msg ){
    if (mq_receive (IdFromRcv, msg, MAX_MSG_SIZE, NULL) == -1) {
            perror ("Server: mq_receive");
            exit(EXIT_FAILURE);
     } else {
        return msg->mtype;
     }
  }


 void SendMsg(mqd_t IdToSend , mqb_msg  *msg){
   if (mq_send (IdToSend, msg, sizeof(struct mqb_msg), 0) == -1) {
             perror ("Server: Not able to send message to client");
         //    exit(EXIT_FAILURE);
    }

 }



int main()
{
 //Deal with message queue first , this will be used for synchronization and signaling
  /*if(0) {
    mqd_t msgq_id ;
    unsigned int msgprio =0;
    pid_t my_pid =getpid();
    char msgcontent[MAX_MSG_LEN];
    int create_queue = 0;
    char ch;
    time_t currtime;
    msgq_id = mq_open(MSGQOBJ_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, NULL);
    if (msgq_id == (mqd_t)-1) {
        perror("In mq_open()");
        exit(1);
      }
   }
*/
  // Message queue implementation 
   struct mq_attr attr;
    long rcv_mtype;
    mqb_msg msg,rcv_msg;
    mqd_t qd_server , qd_client , qd_client2 ;
    attr.mq_flags = MQ_FLAGS;
    attr.mq_maxmsg = MAX_MSGS;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = MQ_CUR_MSGS;

    qd_server = OpenMsgQ(&attr , 1);
    //qd_client = OpenMsgQ(&attr,2);
     while (1) {
           qd_client = OpenMsgQ(&attr,2);
            msg.mtype = 2;
            msg.msgdata[0] ='\0';
            SendMsg(qd_client , &msg);
            rcv_mtype =RcvMsg(qd_server,&rcv_msg);
            printf("Value of mtype received= %d",rcv_mtype);
        }




  // Message queue implementation ends here 

//This is section for the semaphore 
    sem_t *my_sem;
    sem_unlink(SEMNAME);
    my_sem =sem_open(SEMNAME, O_CREAT , 0644 , 1); // Using binary semaphore , on required counted semaphore could be used 
//This section usage named semaphore ,which will be shared with process running on the host machine . 
    if(my_sem == SEM_FAILED) {
        perror("parent sem_open");
        exit(1);
    }
    int value ;
        sem_getvalue(my_sem, &value);    
        printf("The value of the semaphors is %d\n", value);

//Semaphore ready and initialized use it now . 


//SHARED MEMORY STUFF STARTS FROM HERE 
    //const int SIZE = 4096;
    const char *name = "CONTAINER";
    const char *message0 = "Hello dude studying";
    const char *message1 ="Whats there in containerized env";
    int shm_fd ;
     void *ptr ;
     shm_fd = shm_open (name , O_CREAT |    O_RDWR , 0666);
     ftruncate(shm_fd,SIZE);
     ptr = mmap(0,SIZE,PROT_READ |  PROT_WRITE, MAP_SHARED, shm_fd , 0);
   if (ptr == MAP_FAILED )
     {
          perror("map failed");
          exit(1);
       }
  // acquire lock for the critical section before using
    while(1) { 
          sem_wait(my_sem);
          sprintf(ptr,"%s",message0);
         //sem_post(my_sem);
         // ptr = ptr + strlen(message0);
         sprintf(ptr,"%s",message1);
         sem_post(my_sem);
         }
 //release lock 

  return 0;
}

