CC = gcc
CFLAGS = -Wall -Wextra -pthread

all: admissions initialize_hospital

admissions: admissions.c
	$(CC) $(CFLAGS) -o admissions admissions.c

initialize_hospital: initialize_hospital.c
	$(CC) $(CFLAGS) -o initialize_hospital initialize_hospital.c

clean:
	rm -f admissions initialize_hospital triage_admissions_IPC_pipe
	ipcrm -M 0xBEDF00D 2>/dev/null || true

run: all
	./start_hospital.sh

test: all
	./stress_test.sh