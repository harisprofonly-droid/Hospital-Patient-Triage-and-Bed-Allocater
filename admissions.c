#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    if (size>=MAX_SIZE)
    {
       printf("Queue is full");
       return;
    }
    else
    {
priority_queue[size]=new_patient;
int curr=size;
size++;
while (curr>0&&priority_queue[curr].priority<priority_queue[(curr-1)/2].priority)
{
    swap(&priority_queue[curr],&priority_queue[(curr-1)/2]);
    curr=(curr-1)/2;
}

    }
    
}
PatientRecord extract_patient() {
    // If the queue is empty we return a blank record
    PatientRecord empty_record = {0}; 
    if (size <= 0) {
        printf("Queue is empty\n");
        return empty_record;
    }
    
    // If there is only one patient, just pop them and reduce size
    if (size == 1) {
        size--;
        return priority_queue[0];
    }

    // Store the highest priority patient
    PatientRecord highest_priority_patient = priority_queue[0];
    
    //Move the very last patient in the array to the root position
    priority_queue[0] = priority_queue[size - 1];
    size--;

    //Heapify Down
    int curr = 0;
    while (1) {
        int left_child = (2 * curr) + 1;
        int right_child = (2 * curr) + 2;
        int smallest = curr;

        // Check if left child is smaller than current
        if (left_child < size && priority_queue[left_child].priority < priority_queue[smallest].priority) {
            smallest = left_child;
        }
        // Check if right child is even smaller
        if (right_child < size && priority_queue[right_child].priority < priority_queue[smallest].priority) {
            smallest = right_child;
        }
        
        // If the smallest is not the current we swap them and continue down
        if (smallest != curr) {
            swap(&priority_queue[curr], &priority_queue[smallest]);
            curr = smallest;
        } else {
            // if it is in the right place we just break the loop
            break;
        }
    }
    
    return highest_priority_patient;
}

int main() {

char buffer[256];
int age, severity, priority;
char name[64];
 // Open with "r+" to prevent the infinite EOF loop when triage.sh disconnects
FILE *pipe = fopen("triage_admissions_IPC_pipe", "r+");
if (pipe == NULL) {
 perror("File open failed");
 exit(1);
 }

printf("Admissions Manager listening for patients...\n");

while (1) {
      // Read a line from the FIFO. This will block and wait until triage.sh sends data.
 if (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            
    // %63[^,] reads a string up to the comma, max 63 chars to prevent overflow
    // %d parses directly into our integer variables
 int items_parsed = sscanf(buffer, "%63[^,],%d,%d,%d", name, &age, &severity, &priority);
            
if (items_parsed == 4) {
    //we print the current patient that has arrived 
     printf("\n--- New Arrival ---\n");
      printf("Name: %s\nAge: %d\nSeverity: %d\nPriority: %d\n", name, age, severity, priority);
      //we pass the parsed info to the new patientrecord and insert it into priority queue
      PatientRecord new_patient;
      strcpy(new_patient.name,name); 
        new_patient.age=age;
        new_patient.severity=severity;
        new_patient.priority=priority;
        insert_patient(new_patient);
   }
    else 
   {
     printf("Error: Received malformed data -> %s\n", buffer);
    }
        
}
 }

    fclose(pipe);
    return 0;
}