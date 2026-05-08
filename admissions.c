#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>
#include<semaphore.h>
#include<fcntl.h>
// we need these headers for shared memory
#include<sys/ipc.h>
#include<sys/shm.h>
#include"data_structures.h"
#define MAX_SIZE 100

//implementing a priority queue using an array
PatientRecord priority_queue[MAX_SIZE];
int size=0;

void swap(PatientRecord* a, PatientRecord* b){
    PatientRecord temp=*a;
    *a=*b;
    *b=temp;
}

void insert_patient(PatientRecord new_patient){
    if(size>=MAX_SIZE){
       printf("Queue is full\n");
       return;
    }else{
        priority_queue[size]=new_patient;
        int curr=size;
        size++;
        while(curr>0&&priority_queue[curr].priority<priority_queue[(curr-1)/2].priority){
            swap(&priority_queue[curr],&priority_queue[(curr-1)/2]);
            curr=(curr-1)/2;
        }
    }
}

PatientRecord extract_patient() {
    // If the queue is empty we return a blank record
    PatientRecord empty_record={0}; 
    if(size<=0){
        printf("Queue is empty\n");
        return empty_record;
    }
    
    // If there is only one patient, just pop them and reduce size
    if(size==1){
        size--;
        return priority_queue[0];
    }

    // Store the highest priority patient
    PatientRecord highest_priority_patient=priority_queue[0];
    
    //Move the very last patient in the array to the root position
    priority_queue[0]=priority_queue[size-1];
    size--;

    //Heapify Down
    int curr=0;
    while(1){
        int left_child=(2*curr)+1;
        int right_child=(2*curr)+2;
        int smallest=curr;

        // Check if left child is smaller than current
        if(left_child<size&&priority_queue[left_child].priority<priority_queue[smallest].priority){
            smallest=left_child;
        }
        // Check if right child is even smaller
        if(right_child<size&&priority_queue[right_child].priority<priority_queue[smallest].priority){
            smallest=right_child;
        }
        
        // If the smallest is not the current we swap them and continue down
        if(smallest!=curr){
            swap(&priority_queue[curr],&priority_queue[smallest]);
            curr=smallest;
        }else{
            // if it is in the right place we just break the loop
            break;
        }
    }
    
    return highest_priority_patient;
}

//this is our bed allocation function that assigns patients their beds based on priority and care units required
int allocate_bed(PatientRecord p, BedPartition* ward_memory) {
    int required_size=1;
    if(p.priority==1||p.priority==2){
        required_size=3; // icu needs 3 units
    }else if(p.priority==3){
        required_size=2; // isolation needs 2 units
    }

    int best_index=-1;
    int smallest_leftover=9999; // start with an impossibly high number

    // scan the ENTIRE ward to find the tightest fit
    for(int i=0;i<TOTAL_BEDS;i++){
        if(ward_memory[i].is_free==1&&ward_memory[i].size>=required_size){
            
            // calculate how much space would be wasted if we used this bed
            int leftover_space=ward_memory[i].size-required_size;
            
            // if this bed wastes less space than our previous best, it becomes the new best
            if(leftover_space<smallest_leftover){
                best_index=i;
                smallest_leftover=leftover_space;
            }
            
            // if we found a perfect fit (0 waste), stop searching immediately to save time!
            if(smallest_leftover==0){
                break;
            }
        }
    }

    // if we found a valid bed anywhere in the array, assign it
    if(best_index!=-1){
        ward_memory[best_index].is_free=0;
        ward_memory[best_index].patient_id=p.patient_id;
        return ward_memory[best_index].partition_id;
    }

    // hospital is full or no bed is big enough
    return -1;
}

// here we will admit our patient and fork our program 
void admit_patient(PatientRecord p, int bed_id) {
    pid_t pid=fork();
    if(pid<0){
        printf("Fork failed\n");
        return;
    }
    if(pid==0){
        char p_id[16],p_prio[16],p_bed[16];
        sprintf(p_id,"%d",p.patient_id);
        sprintf(p_prio,"%d",p.priority);
        sprintf(p_bed,"%d",bed_id);
        
        char *args[]={"./patient_simulator",p_id,p_prio,p_bed,NULL};
        execv("./patient_simulator",args);
        
        printf("Exec failed\n");
        exit(1);
    }
}

void sigchld_handler(int sig) {
    (void)sig; // This line tells the compiler "I know this is here, don't warn me"
    while(waitpid(-1,NULL,WNOHANG)>0);
}

int main() {
    // we open the lock created by the initialize script
    sem_t *mutex=sem_open("/hospital_sem",0);
    
    // we register our signal handler to clean up zombie processes
    signal(SIGCHLD,sigchld_handler);
    
    // we need an id counter for our incoming patients
    int next_patient_id=1;

    // we attach to the shared memory created by our initialize script
    size_t shared_mem_size=sizeof(BedPartition)*TOTAL_BEDS;
    int shmid=shmget(SHARED_MEM_KEY,shared_mem_size,0666);
    if(shmid<0){
        printf("Admissions: shmget failed. Did initialize_hospital run first?\n");
        exit(1);
    }
    BedPartition* ward_memory=(BedPartition*)shmat(shmid,NULL,0);

    // open pipes in non-blocking mode so the loop doesnt freeze
    // FIXED: Changed O_RDONLY to O_RDWR here so the pipe stays awake for all 20 patients
    int triage_fd=open("triage_admissions_IPC_pipe",O_RDWR|O_NONBLOCK);
    int discharge_fd=open("discharge_fifo",O_RDWR|O_NONBLOCK);

    printf("Admissions Manager listening for patients...\n");

    while(1){
        char buffer[1024];
        
        // door 1 arrivals
        int bytes=read(triage_fd,buffer,sizeof(buffer)-1);
        if(bytes>0){
            buffer[bytes]='\0';
            
            char *line=strtok(buffer,"\n");
            while(line!=NULL){
                int age,severity,priority;
                char name[64];
                if(sscanf(line,"%63[^,],%d,%d,%d",name,&age,&severity,&priority)==4){
                    //we print the current patient that has arrived 
                    printf("\n--- New Arrival ---\n");
                    printf("Name: %s\nAge: %d\nSeverity: %d\nPriority: %d\n",name,age,severity,priority);
                    
                    //we pass the parsed info to the new patientrecord and insert it into priority queue
                    PatientRecord new_patient;
                    new_patient.patient_id=next_patient_id++; // assign the unique id here
                    strcpy(new_patient.name,name); 
                    new_patient.age=age;
                    new_patient.severity=severity;
                    new_patient.priority=priority;
                    insert_patient(new_patient);

                    // LOCK MEMORY
                    sem_wait(mutex);

                    // we pull the highest priority patient from queue and look for a bed
                    PatientRecord ready_patient=extract_patient();
                    int assigned_bed_id=allocate_bed(ready_patient,ward_memory);
                    
                    // if we found a free bed we admit them else we put them back in queue
                    if(assigned_bed_id!=-1){
                        printf("Admitting %s to Bed ID: %d\n",ready_patient.name,assigned_bed_id);
                        admit_patient(ready_patient,assigned_bed_id);
                    }else{
                        printf("Hospital full! %s is waiting in the priority queue.\n",ready_patient.name);
                        insert_patient(ready_patient);
                    }

                    // UNLOCK MEMORY
                    sem_post(mutex);
                }
                line=strtok(NULL,"\n");
            }
        }

        // door 2 discharges
        char d_buffer[256];
        int d_bytes=read(discharge_fd,d_buffer,sizeof(d_buffer)-1);
        if(d_bytes>0){
            d_buffer[d_bytes]='\0';
            
            char *d_line=strtok(d_buffer,"\n");
            while(d_line!=NULL){
                int discharged_id=atoi(d_line);
                
                // LOCK MEMORY
                sem_wait(mutex);

                // find their bed and free it
                for(int i=0;i<TOTAL_BEDS;i++){
                    if(ward_memory[i].patient_id==discharged_id){
                        ward_memory[i].is_free=1;
                        ward_memory[i].patient_id=-1;
                        printf("\n[System] Bed %d is now FREE.\n",ward_memory[i].partition_id);
                        break;
                    }
                }
                
                // check if anyone is waiting in the queue for this new free bed
                if(size>0){
                    PatientRecord waiting_patient=extract_patient();
                    int new_bed_id=allocate_bed(waiting_patient,ward_memory);
                    
                    if(new_bed_id!=-1){
                        printf("Queue Pull: Admitting %s to Bed ID: %d\n",waiting_patient.name,new_bed_id);
                        admit_patient(waiting_patient,new_bed_id);
                    }else{
                        insert_patient(waiting_patient); 
                    }
                }

                // UNLOCK MEMORY
                sem_post(mutex);

                d_line=strtok(NULL,"\n");
            }
        }
        // sleep a tiny bit so cpu doesnt max out
        usleep(50000); 
    }

    return 0;
}