#!/bin/bash
# ==========================================
# Project   : Hospital Patient Triage & Bed Allocator
# Script    : triage.sh
# Group     : Group XX
# Members   : (24F-0703)
# Date      : 2026-04-22
# Purpose   : Compute triage priority and pipe patient data to the admissions manager process.
# Usage     : ./triage.sh <name> <age> <severity 1-10>
# ==========================================

if [ "$#" -ne 3 ]; then
    echo "Error: Invalid arguments. Usage: ./triage.sh <name> <age> <severity>"
    exit 1
fi

NAME=$1
AGE=$2
SEVERITY=$3

if ! [[ "$AGE" =~ ^[0-9]+$ ]] || ! [[ "$SEVERITY" =~ ^[0-9]+$ ]]; then
    echo "Error: Age and severity must be numeric."
    exit 1
fi

if [ "$SEVERITY" -lt 1 ] || [ "$SEVERITY" -gt 10 ]; then
    echo "Error: Severity must be between 1 and 10."
    exit 1
fi

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

echo "$NAME,$AGE,$SEVERITY,$PRIORITY"
