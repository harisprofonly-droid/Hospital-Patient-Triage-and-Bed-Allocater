CC = gcc
CFLAGS = -Wall -Wextra -pthread

all: admissions patient_simulator

admissions: admissions.c
	$(CC) $(CFLAGS) -o admissions admissions.c

patient_simulator: patient_simulator.c
	$(CC) $(CFLAGS) -o patient_simulator patient_simulator.c

run:
	./start_hospital.sh

test:
	./stress_test.sh

clean:
	rm -f admissions patient_simulator /tmp/discharge_fifo
