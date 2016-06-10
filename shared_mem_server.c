#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/types.h>
#include <mqueue.h>
#define QNAME  "/myqueue"
#define SEMNAME "/mysem1"
#define SIZE 4096
#define QPERM 0660
#define MAX_MSGS 10
#define MAX_MSG_SIZE 256
#define MAX_MSG_SIZE_DATA 10
#define MQ_FLAGS  0
#define MQ_CUR_MSGS 0
#define C1QNAME "/myc1queue"
 typedef struct mqb_msg {
     long  mtype ;
     char msgdata[MAX_MSG_SIZE_DATA];
   }mqb_msg;

 

 mqd_t OpenMsgQ(struct mq_attr *attr , int type ) {
     mqd_t id;
     if (type==1) {
       if ((id = mq_open (QNAME, O_RDONLY | O_CREAT, QPERM, attr)) == -1) {
               perror ("Server: mq_open (server)");
               exit (1);
       } 
     }
     if (type == 2) {
      if ((id = mq_open (C1QNAME, O_WRONLY)) == 1) {
            perror ("Server: Not able to open client queue");
            exit(1);
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
            // exit(EXIT_FAILURE);
    }
  
 }


int main()
{
    // Message queue implementation starts here 
    struct mq_attr attr;
    long rcv_mtype;
    mqb_msg msg,rcv_msg;
    mqd_t qd_server , qd_client , qd_client2 ; 
    attr.mq_flags = MQ_FLAGS;
    attr.mq_maxmsg = MAX_MSGS;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = MQ_CUR_MSGS;
    
    qd_server = OpenMsgQ(&attr , 1);
    qd_client = OpenMsgQ(&attr,2);
    // while (1) {
            msg.mtype = 1;
            msg.msgdata[0] ='\0';
            SendMsg(qd_client , &msg);
            rcv_mtype =RcvMsg(qd_server,&rcv_msg);
            printf("Value of mtype received= %d",rcv_mtype);
      //  }     
        


    //Message queue implementation ends here 

	const char *name = "CONTAINER";
     //	const int SIZE = 4096;

	int shm_fd;
	void *ptr;
	int i;
    // This is for semaphore opening , name semaphore 

     sem_t * my_sem ;
     my_sem =sem_open(SEMNAME, O_CREAT , 0644 , 1);
     if(my_sem == SEM_FAILED) {
        perror("parent sem_open");
        exit(1);
    }
    // Semaphore opening done 

	/* open the shared memory segment */
	shm_fd = shm_open(name, O_RDWR, 0666);
	if (shm_fd == -1) {
		printf("shared memory failed\n");
		exit(-1);
	}

	/* now map the shared memory segment in the address space of the process */
	ptr = mmap(0,SIZE, PROT_READ| PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		exit(-1);
	}

	/* now read from the shared memory region */
       // while (1) {
              int value ;
              sem_getvalue(my_sem, &value); 
             // printf("The value of the semaphors is %d\n", value);
              sem_wait(my_sem);
	      printf("%s",ptr);
              sem_post(my_sem);
        // }
	/* remove the shared memory segment */
	if (shm_unlink(name) == -1) {
		printf("Error removing %s\n",name);
		exit(-1);
	}

	return 0;
}
