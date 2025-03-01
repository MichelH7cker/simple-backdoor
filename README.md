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
  - [Description of the functionality of the main functions](#description-of-the-functionality-of-the-main-functions)
    - [`backdoor.c`](#backdoorc)
    - [`client.py`](#clientpy)
    - [`server.py`](#serverpy)
  - [Link to GitHub](#link-to-github)
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

    # insmod backdoor.ko
    OR
    # make load

To unload the module (and clear the logs), run:

    # rmmod backdoor.ko

To run the python programs just do
    
    # python3 server.py ip port

and

    # python3 client.py

where ip is the IP's server and port is the server port.
The client is set by defaut to IP 127.0.0.1 and port 8080.

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

## Description of the functionality of the main functions 
### `backdoor.c`
In this file the most important function is the 

    void keycode_to_string(int keycode, int shift_is_pressed, char *buffer);

 function. When a keyboard interrupt occurs, an integer code is generated. This function is responsible for translating this entire code into understandable strings for us humans. In the development of this function, the question arose whether it was easier to perform the cast instead of mapping all the keys. We chose to use mapping because that way it was possible to translate the code into anything we wanted. For example, the 'ESC' key does not have a corresponding character to perform the cast. Mapping the keycodes we wrote that the entire code responsible for the esc key would be saved as string "\_ESC\_", which improves the keylogger.

 In the backdoor file, another functionality is important, which is to generate a file containing all the keys typed by the user. This functionality is found in the keyboard key interrupt callback function. It is very simple because there is already a reading and writing function 
    
    simple_read_from_buffer(buffer, len, offset, keys_buf, buf_pos);

### `client.py`
when executing the client's code, it is necessary to pass the cm argument which functionality will be exercised. If the keylogger argument is passed, the client will send all the keystrokes to the server. This is done by accessing the keylogger file generated by the kernel module. Therefore, the program will read the file and send the entire file and send. If the prtsc argument is passed, the client will wait for the print command given by the server. When the command is given, the client will perform the print via a terminal call using the scrot command.

### `server.py`
The server works as a multithreaded server. For the specific case of this program, only two clients will connect, and each one will have a different functionality, as previously explained. The 

    client_thread = threading.Thread(target=handle_clients, args=(client, client_id)) 

section starts a client thread. When a thread is started, the function 

    handle_clients(client, id)

is automatically executed. This function directs what the client is going to do based on your codes. As the keylogger is always run first (see the minecraft.sh script) so it is the first thread so it will always enter the first if

    if id == KEYLOGGER:
         handle_keylogger(client)
     elif id == PRINTSCREEN:
         handle_print_screen(client)
     else:
         print('[-] There is no function for this client')

The function related to receiving the data sent by the client executing the keylogger is the ```handle_keylogger(client)``` function, while the function responsible for receiving the data sent by the print screen client is the ```handle_print_screen(client)``` function. I believe that these two functions are very simple to understand since they both basically do the same thing: receive data and process it.

## Link to GitHub
<a href="https://github.com/MichelH4cker/simple-backdoor">GitHub</a>

## License
<a href="https://github.com/jarun/googler/blob/master/LICENSE"><img src="https://img.shields.io/badge/license-GPLv2-yellow.svg?maxAge=2592000" alt="License" /></a>

## Developer
Copyright © 2023 [Michel Hecker Faria - 12609690](mailto:michel.hecker@usp.br)
