# STC8A8K-Multi-computer-Communication V2.0

###  *NEWEST INFORMATION

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

TB8=0 send datas

TB8=1 send address

---------------------

###  *NEWEST INFORMATION

Through the host set slave machine address, set slave machine frequency, duty cycle, water lamp pattern.

The host is set by keyboard, LCD12864 display.

**KEY 1** SLAVE1

First input frequency renge 99999HZ to 30.5HZ(31HZ) 5bit

Second input duty cycle renge 100 to 0 3bit

**KEY 2** SLAVE2

**KEY 15** COMFIRM

**ans1** frequency

**ans2** dutycycle

---------------------

###  *NEWEST INFORMATION

Development board limitation, unable to use LCD display

*21/12/9* 
Triggers external interrupts with timer interrupts; Use serial port interrupt to trigger data transmission
Interrupt trigger: TI/RI, that is, after sending, receiving can enter the interrupt program
Updated to main2. C

*21/12/11* 
Interrupt is not suitable to call the function, put the serial port send function to the main function, but then interrupt firing is difficult
Updated to main3. C

*21/12/12* 
There is no need to use interrupts, delete interrupts, rewrite protocols,
Updated to main4. C
Update main42.2. c with modes one, two, and data check keys

*21/12/13* 
is updated to main5.c, and the duty cycle and frequency are combined to send 2/1 data. The flow light only allows to send one data, and the data transmission function is modified
Frequency is sent in high and low bits, because only eight bits can be sent at a time
Collate parameter types and names

>**NEWEST PROBLEM:**

>1.Data type in operation, multi-bit transmission

>**PROBLEM NOTE:**

>1.A function inside an interrupt cannot be broken by an interrupt

>2.Automatic address recognition, unable to recognize new addresses

###  Design of multi-machine communication system

Requirements: single chip microcomputer as the core design of a multi - machine communication system.

1. Implement point-to-point communication between one host and two slave machines.
2. Set the slave machine address through the host, set the slave machine frequency, duty cycle, water lamp pattern.
3. Display communication data.
