#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <sys/types.h>
#include <time.h>

#define SHARED_MEM_KEY 0xBEDF00D
#define TOTAL_BEDS 20

typedef struct {
 int patient_id ;
 char name [64];
 int age ;
 int severity ; /* 1 -10 raw severity from triage */
 int priority ; /* 1 -5 computed triage priority */
 int care_units ; /* memory units required */
 time_t arrival_time ;
 } PatientRecord ;

 /* Single bed partition in the ward memory model */
 typedef struct {
 int partition_id ;
 int start_unit ; /* index in ward array */
 int size ; /* number of care units */
 int is_free ; /* 1 = FREE , 0 = OCCUPIED */
 int patient_id ; /* -1 if free */
 char bed_type [16]; /* " ICU " , " GENERAL " , " ISOLATION " */
 } BedPartition ;

#endif
