#include<stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include <time.h>
#include<fcntl.h>
#include <string.h>

int main(int argc,char *argv[]){
    // we grab the arguments passed by our execv function
    if(argc<4) {
    printf("Simulator error: Missing arguments\n");
       exit(1);
    }
    
  // we convert the string arguments back to integers
  int patient_id=atoi(argv[1]);
   int priority=atoi(argv[2]);
    int bed_id =atoi(argv[3]);

    printf("[Patient %d] Arrived and assigned to Bed %d\n",patient_id,bed_id);
    printf("[Patient %d] Treatment started...\n", patient_id);

    // we seed the random number generator using the unique process id so everyone gets random times
   srand(time(NULL)^(getpid()<<16));

    // we determine sleep duration based on bed type and priority
  int sleep_time=0;
    if(priority==1||priority==2){
        // icu beds sleep 5 to 15 seconds
       sleep_time=(rand()%11)+5; 
    }else if(priority==3) {
     // isolation beds sleep 3 to 10 seconds
     sleep_time=(rand()%8)+3;  
    } else{
     // general beds sleep 2 to 8 seconds
      sleep_time=(rand()%7)+2;  
    }

   sleep(sleep_time);

    printf("[Patient %d] Treatment complete. Discharging after %d seconds.\n",patient_id,sleep_time);

    // we write to the discharge fifo to tell admissions manager the bed is free
    int fd=open("discharge_fifo",O_WRONLY);
    if(fd!=-1){
       char msg[16];
      sprintf(msg,"%d\n",patient_id);
        write(fd,msg,strlen(msg));
       close(fd);
    }

    return 0;
}