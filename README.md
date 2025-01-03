# OperatingSystems

This is a folder for my operating systems class.
Two major projects in this class, the scheduler and the driver
____________________________________________________________________________________
The Scheduler simulates how resources are managed and distributed by the scheduler 
to the CPU. 

A Process Structure consists of 8 attributes:
- PID = the process id number
- CPU USED = the amount of cpu cycles used
- MAX TIME = the maximum amount of time the process may use
- STATE = whether the state was running, blocked, or deleted
- PRI = the priority level of the process
- QUANTUM USED = the amount of cpu cycles it had already ran
- BLK TIME = the amount cpy cycles the process had been blocked for
- WAIT TKS = how long the process had to wait 

There are several rules that we were given to help construct a scheduler. Among them were that the scheduler
only prints when deleting a process, or scheduling another process.

I chose to use a linked list as opposed to an array because insertion and deletion is 
what linked lists excel at.

MOST IMPORTANT THING WHEN WRITING THIS IS HOW YOU STRUCTURE YOUR MAIN.
- I used alot of if-else statements but I think a switch statement may work better.
- Test each function. Test edge cases. Test thoroughly
- **Scheduler is essentially 4 movements between 3 states**
  - Ready to Blocked, Blocked to Ready, Running to Ready, Running to Blocked, and Delete
  - the whole program should be structured around performing those actions
