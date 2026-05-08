#!/bin/bash
# ==========================================
# Project   : Hospital Patient Triage & Bed Allocator
# Script    : start_hospital.sh
# Group     : Group XX
# Members   : Muhammad Haris Hamid (24F-0699), Muhammad Khalil (24F-0525)
# Date      : 2026-04-23
# Purpose   : Initialize environment and launch the admissions manager process.
# Usage     : ./start_hospital.sh
# ==========================================

echo "=========================================="
echo "    HOSPITAL SYSTEM INITIALIZATION        "
echo "=========================================="
echo "Ward Capacity:"
echo "  - ICU Beds       : 4"
echo "  - Isolation Beds : 4"
echo "  - General Ward   : 12"
echo "------------------------------------------"
echo "Total Capacity   : 20 Beds"
echo "=========================================="

if [ ! -p triage_admissions_IPC_pipe ]; then
    mkfifo triage_admissions_IPC_pipe
fi
# this line initializes the hospital 
./initialize_hospital &
# this line runs admissions.c as a process in the background 
sleep 1
./admissions &

