/*
 * =========================================
 * Project : Hospital Patient Triage & Bed Allocator
 * File    : admissions.c
 * Group   : Group XX
 * Members : Muhammad Haris Hamid (24F-0699), Muhammad Khalil (24F-0525)
 * Date    : 2026-04-23
 * Purpose : Central admissions manager process spawning,
 * IPC, thread pool, scheduling, and bed allocation.
 * Compile : gcc -Wall -o admissions admissions.c -lpthread
 * =========================================
 */
#include<stdio.h>
#include<unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include"data_structures.h"
int main(){

//declaring variables
int shmid;
BedPartition *ward_memory;
size_t shared_memory_size= sizeof(BedPartition)* TOTAL_BEDS;
//getting our shared memory
shmid= shmget(SHARED_MEM_KEY,shared_memory_size, IPC_CREAT | 0666);
//if failed to create shared memory give error
if (shmid < 0) {
        perror("shmget failed");
        exit(1);
    }
//we attach the memory segment of our ward
ward_memory= (BedPartition*)shmat(shmid,NULL,0);
//if failed to attach memory give error
if(ward_memory == (void*)-1) {
	perror("shmat failed");
	exit(1);
}
// now for each ward number we allocate its attributes

//for ICU
for (int i = 0; i < 4; i++) {
        ward_memory[i].partition_id = i;
        ward_memory[i].size = 3;
        ward_memory[i].is_free = 1;
        ward_memory[i].patient_id = -1;
        strcpy(ward_memory[i].bed_type, "ICU");
    }

// for ISOLATION
    for (int i = 4; i < 8; i++) {
        ward_memory[i].partition_id = i;
        ward_memory[i].size = 2;
        ward_memory[i].is_free = 1;
        ward_memory[i].patient_id = -1;
        strcpy(ward_memory[i].bed_type, "ISOLATION");
    }

// for GENERAL
    for (int i = 8; i < 20; i++) {
        ward_memory[i].partition_id = i;
        ward_memory[i].size = 1;
        ward_memory[i].is_free = 1;
        ward_memory[i].patient_id = -1;
        strcpy(ward_memory[i].bed_type, "GENERAL");
    }

printf("Admissions Manager Booted. Shared memory initialized successfully.\n");

    while(1) {
        sleep(1); 
    }



return 0;

}
