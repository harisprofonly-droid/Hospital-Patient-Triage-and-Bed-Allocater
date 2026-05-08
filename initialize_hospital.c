#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<unistd.h>
#include<semaphore.h>
#include <sys/shm.h>
#include<string.h>
#include"data_structures.h"

int main(){

    // we print a line signalling start of shared mem initialization
    printf("Initializing Hospital memory");
    // we calculate the total size of the memory required in bytes
    // we do this by multiplying the size of our Bedpartition
    //struct and the total number of Beds i.e 20 in this case
 size_t shared_mem_size = sizeof(BedPartition)* TOTAL_BEDS;
// we no create our shared memory using shmget()
// we use our Hex key, the total size we just calculated and 
// the permission to read and write in shared memory
//our shared memory's id gets returned to us in shmid 
 int shmid=shmget(SHARED_MEM_KEY,shared_mem_size,IPC_CREAT | 0666);
//check if shmget failed 
if(shmid<0){
    printf("shmget failed");
    exit(1);
}
// we get the memory now we attach it to our program and 
// convert that memory chunk to BedPartition type
BedPartition* ward_memory=(BedPartition*)shmat(shmid,NULL,0);
// we format the memory according to instructions in manual
//4 ICU, 4 Isolation, 12 General wards
//"ICU"
for (int i = 0; i < 4; i++)
{
ward_memory[i].partition_id=i;
ward_memory[i].size=3;
ward_memory[i].is_free=1;
ward_memory[i].patient_id=-1;
strcpy(ward_memory[i].bed_type,"ICU");
}
//Isolation
for (int i = 4; i < 8; i++)
{
ward_memory[i].partition_id=i;
ward_memory[i].size=2;
ward_memory[i].is_free=1;
ward_memory[i].patient_id=-1;
strcpy(ward_memory[i].bed_type,"Isolation");
}
//General
for (int i = 8; i < 20; i++)
{
ward_memory[i].partition_id=i;
ward_memory[i].size=1;
ward_memory[i].is_free=1;
ward_memory[i].patient_id=-1;
strcpy(ward_memory[i].bed_type,"General");
}

}