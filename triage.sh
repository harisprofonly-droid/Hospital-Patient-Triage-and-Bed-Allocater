#!/bin/bash
# ==========================================
# Project   : Hospital Patient Triage & Bed Allocator
# Script    : triage.sh
# Group     : Group XX
# Members   : Muhammad Haris Hamid (24F-0699),Muhammad Khalil (24F-0525)
# Date      : 2026-04-22
# Purpose   : Compute triage priority and pipe patient data to the admissions manager process.
# Usage     : ./triage.sh <name> <age> <severity 1-10>
# ==========================================

if [ "$#" -ne 3 ]; then #this checks the number of arguments
    echo "Error: Invalid arguments. Usage: ./triage.sh <name> <age> <severity>"
    exit 1
fi


NAME=$1
AGE=$2
SEVERITY=$3

# this condition checks if age and severity are numeric or not
if ! [[ "$AGE" =~ ^[0-9]+$ ]] || ! [[ "$SEVERITY" =~ ^[0-9]+$ ]]; then
    echo "Error: Age and severity must be numeric."
    exit 1
fi

#check for correct severity
if [ "$SEVERITY" -lt 1 ] || [ "$SEVERITY" -gt 10 ]; then
    echo "Error: Severity must be between 1 and 10."
    exit 1
fi
#if else statement to assign priority based on severity
if [ "$SEVERITY" -ge 9 ]; then
    PRIORITY=1
elif [ "$SEVERITY" -ge 7 ]; then
    PRIORITY=2
elif [ "$SEVERITY" -ge 5 ]; then
    PRIORITY=3
elif [ "$SEVERITY" -ge 3 ]; then
    PRIORITY=4
else
    PRIORITY=5
fi
# we output the user info as a string and then use piping to pass this to
# admissions.c process.
echo "$NAME,$AGE,$SEVERITY,$PRIORITY" | ./admissions
