CC = gcc
CFLAGS = -Wall -Wextra -pthread

# 1. Add patient_simulator to the "all" target
all: admissions initialize_hospital patient_simulator

admissions: admissions.c
	$(CC) $(CFLAGS) -o admissions admissions.c

initialize_hospital: initialize_hospital.c
	$(CC) $(CFLAGS) -o initialize_hospital initialize_hospital.c

# 2. Create the recipe for patient_simulator
patient_simulator: patient_simulator.c
	$(CC) $(CFLAGS) -o patient_simulator patient_simulator.c

# 3. Update clean to remove the new executable
clean:
	rm -f admissions initialize_hospital patient_simulator triage_admissions_IPC_pipe
	ipcrm -M 0xBEDF00D 2>/dev/null || true

run: all
	./start_hospital.sh

test: all
	./stress_test.sh