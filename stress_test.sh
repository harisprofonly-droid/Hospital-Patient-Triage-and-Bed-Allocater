#!/bin/bash
# ==========================================
# Script    : stress_test.sh
# Purpose   : Spawns 20 concurrent patient arrivals
# ==========================================

echo "Launching 20-patient stress test..."

for i in {1..20}; do
    # Generate a random age (10-89) and random severity (1-10)
    AGE=$(( ( RANDOM % 80 ) + 10 ))
    SEV=$(( ( RANDOM % 10 ) + 1 ))
    
    # Run the triage script in the background so they hit simultaneously
    ./triage.sh "TestPatient_$i" $AGE $SEV &
done

echo "All 20 patients dispatched to triage!"