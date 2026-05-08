#!/bin/bash

echo "Initiating Hospital Shutdown Protocol..."

#Kill the background processes
echo "Stopping Admissions Manager and Simulators..."
pkill -f "./admissions" 2>/dev/null
pkill -f "./patient_simulator" 2>/dev/null

#Remove the named pipes
echo "Cleaning up IPC pipes..."
rm -f triage_admissions_IPC_pipe discharge_fifo

#Free the shared memory
echo "Freeing shared memory block (Key: 0xBEDF00D)..."
ipcrm -M 0xBEDF00D 2>/dev/null || true

echo "System completely shut down and cleaned up."