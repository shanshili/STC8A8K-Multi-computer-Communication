# STC8A8K-Multi-computer-Communication V2.0
=============================
###  NEWEST INFORMATION

**NAME**:mainframe
**Serial port 1**: 9 bit data, variable baud rate
**Timer 1**: 16 bit automatic overload
**Serial port 1 baud rate generator**: Timer 1
**Timer 0**: KEY 4  interrupted
**Timer 2**: KEY 8  interrupted
**UART interrupt**: Determine the response from the machine selection, prompting to send data
**Baud rate**: 9600
**RxD** P3.0
**TxD** P3.1

==================================================
>**NEWEST PROBLEM:**

>1.

>2.

>**PROBLEM NOTE:**

>1.A function inside an interrupt cannot be broken by an interrupt

>2.Automatic address recognition, unable to recognize new addresses

###  Design of multi-machine communication system

Requirements: single chip microcomputer as the core design of a multi - machine communication system.

1. Implement point-to-point communication between one host and two slave machines.
2. Set the slave machine address through the host, set the slave machine frequency, duty cycle, water lamp pattern.
3. Display communication data.


------------------------------
TB8=0 send datas
TB8=1 send address
