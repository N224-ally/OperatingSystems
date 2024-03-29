# OperatingSystems

This is a folder for my operating systems class.
Two major projects in this class, the scheduler and the driver
____________________________________________________________________________________
The Scheduler simulates how resources are managed and distributed by the scheduler 
to the CPU. 

A Process consists of 8 attributes:
PID 
CPU USED
MAX TIME
STATE
PRI
QUANTUM USED
BLK TIME
WAIT TKS

There are several rules that we were given to help construct a scheduler that prints out 
the process table ONLY when deleting a process, or scheduling another process. I may or may not write 
out the requirements (this is due in 10 days) if I have time.

I chose to use a linked list as opposed to an array because insertion and deletion is 
what linked lists excel at.

Apart from the standard linked list manipulation functions (insert, delete, create a list, count)
I also added an update_table, schedule_table, and 
