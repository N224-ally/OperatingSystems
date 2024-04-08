/**********************************************************************/
/*                                                                    */
/* Program Name: Scheduler - Schedules processes and prints the       */
/*                           process table context switches           */
/* Author:       Niwe Mugeni                                          */
/* Installation: Pensacola Christian College, Pensacola, Florida      */
/* Course:       CS326, Operating Systems                             */
/* Date Written: April 5, 2024                                        */
/*                                                                    */
/**********************************************************************/

/**********************************************************************/
/*                                                                    */
/* I pledge this assignment is my own first time work.                */
/* I pledge I did not copy or try to copy work from the Internet.     */
/* I pledge I did not copy or try to copy work from any student.      */
/* I pledge I did not copy or try to copy work from any where else.   */
/* I pledge the only person I asked for help from was my teacher.     */
/* I pledge I did not attempt to help any student on this assignment. */
/* I understand if I violate this pledge I will receive a 0 grade.    */
/*                                                                    */
/*                                                                    */
/*                      Signed: _____________________________________ */
/*                                           (signature)              */
/*                                                                    */
/*                                                                    */
/**********************************************************************/

/**********************************************************************/
/*                                                                    */
/* The program simulates the environment in Unix where new processes  */
/* are continualy arriving, existing processes are vying for the CPU, */
/* process are using their given quantum(CPU bound) or blocking       */
/* because of I/O operations, and processes are terminating when      */
/* their work is finished.                                            */
/*                                                                    */
/**********************************************************************/

#include <stdio.h>  
#include <stdlib.h> 
#include <math.h> 

/**********************************************************************/
/*                         Symbolic constants                         */
/**********************************************************************/
#define AFTER                  1   /* Position after another element  */
#define BEFORE                 0   /* Position before another element */
#define BLOCKED_STATE          'B' /* Blocking state of process       */
#define FALSE                  0   /* False value                     */
#define HEADER_NODE_ERR        1   /* Can't allocate header memory    */
#define INIT_PROCESS_VALUE     5   /* Number of initialized processes */
#define INSERT_NODE_ERR        3   /* Can't allocate new node memory  */
#define LIST_HEADER  MIN_PID - 1   /* Start of linked list value      */
#define LIST_TRAILER MAX_PID + 1   /* Last item in linked list value */
#define MAX_PID                100 /* Greatest possible PID           */
#define MIN_PID                1   /* Lowest possible PID             */
#define READY_STATE            'R' /* Ready state of process          */
#define RUNNING_STATE          'N' /* Running state of process        */
#define TRAILER_NODE_ERR       2   /* Can't allocate trailer memory   */
#define TRUE                   1   /* True value                      */


/**********************************************************************/
/*                       Program Structures                           */
/**********************************************************************/
/* Process Information for Scheduling                                 */
struct PROCESS
{
   int            block_time,      /* Process block time              */
                  cpu_used,        /* Clock ticks used by process     */
                  max_time,        /* Maximum CPU time to complete    */
                  pid,             /* Process ID number               */
                  pri,             /* Process Priority number         */
                  quantum_used,    /* Quantum used by the process     */
                  wait_tks;        /* Ticks used in ready state       */ 
   char           state;           /* State of the process            */
   struct PROCESS *p_next_process; /* Points to the next process      */
};
typedef struct PROCESS PROCESS;

/**********************************************************************/
/*                        Function Prototypes                         */
/**********************************************************************/
void    print_table(int status, int process_pid, int number_of_processes);
   /* Prints the process table                                        */
PROCESS *create_table();
   /* Creates the process table                                       */
void    add_process(int *p_process_pid, int *p_number_of_processes);
   /* Add a process to the process table                              */
void    schedule_table();
   /* Schedule the process tbale                                      */
PROCESS *find_ruuning_process( int *p_running);
   /* Find a running process within the table                         */
void    update_tick();
   /* Update the process table every tick                             */
void    sort_table(int number_of_processes);
   /* Sort the process stable                                         */
int     new_priority(PROCESS *p_process);
   /* Calculate priority value for process                            */
void    remove_process(int delete_pid, int *p_number_of_processes);
   /* Remove a process from the process table                         */

/**********************************************************************/
/*                       Global Variables                             */
/**********************************************************************/
PROCESS *p_table = NULL; /* Points to the process table               */

/**********************************************************************/
/*                          Main Function                             */
/**********************************************************************/
int main() 
{
   int      counter,                     /* Counts every process      */
            process_pid         = 1,     /* Next PID queued           */
            running_process_bit = FALSE, /* Running process           */
            number_of_processes = 0,     /* Amount in table           */
            block_amount        = 0;     /* Blocked process count     */

   PROCESS  *running_process    = NULL,   /* Points to the running    */
            *process;                     /* Points to an process     */

   /* Created, Initialize, and print process list                     */
   p_table = create_table();
   for (counter = 1; counter <= INIT_PROCESS_VALUE; counter++)
      add_process(&process_pid, &number_of_processes);

   print_table(BEFORE,process_pid,number_of_processes);
   running_process = find_ruuning_process(&running_process_bit);
   print_table(AFTER,process_pid,number_of_processes);


   /* Loop processing of all the processes                            */
   while(process_pid <= MAX_PID)
   {
      process  = p_table;

      /* Block process if necessary                                   */
      if(running_process->cpu_used >= running_process->max_time && 
         running_process_bit == TRUE)
      {
         print_table   (BEFORE, process_pid,number_of_processes);
         remove_process(running_process->pid, &number_of_processes);

         if(block_amount != number_of_processes)
         {
            running_process_bit = FALSE;  
            running_process     = 
            find_ruuning_process(&running_process_bit);
         }
         else
            running_process_bit = FALSE;

         sort_table (number_of_processes);
         print_table(AFTER, process_pid,number_of_processes);
      }

      /*                                                              */
      if(running_process->quantum_used == running_process->block_time 
         && running_process_bit == TRUE)
      {
         print_table(BEFORE, process_pid,number_of_processes);

         if(running_process->block_time == 6)
         {
            running_process->pri   = new_priority(running_process);
            running_process->state = READY_STATE;
         }
         else
         {
            running_process->pri   = 
                               ((new_priority(running_process)) * -1);
            running_process->state = BLOCKED_STATE;
            block_amount++;
         }

         if(block_amount != number_of_processes)
         {
            running_process->quantum_used = 0;
            running_process_bit           = FALSE;
            running_process               = 
                           find_ruuning_process(&running_process_bit);
            sort_table(number_of_processes);
         }
         else
            running_process_bit = FALSE;
         sort_table(number_of_processes);
         print_table(AFTER, process_pid,number_of_processes);
      }

      /* Blocked process becomes unblocked                            */
      while(process = process->p_next_process,
            process->pid != LIST_TRAILER)
         if (process->state == BLOCKED_STATE)
         {
            if(((rand()% 20) + 1) == 1)
            {
               if(block_amount == number_of_processes)
               {
                   print_table(BEFORE, process_pid,number_of_processes);
                   process->state      = READY_STATE;
                   running_process_bit = FALSE;
                   running_process     = 
                            find_ruuning_process(&running_process_bit);
                   sort_table (number_of_processes);
                   print_table(AFTER, process_pid,number_of_processes);
                   block_amount--;
               }
               else
               {
                  print_table(BEFORE, process_pid,number_of_processes);
                  process->state = READY_STATE;
                  sort_table (number_of_processes);
                  print_table(AFTER, process_pid,number_of_processes);
                  block_amount--;
               }
            }
         }
      
      /* Update Process table */
      update_tick();

      /*                                                              */
      if(number_of_processes < 10)
      {
         if(((rand()% 5) + 1) == 1)
         {
            if(number_of_processes == 0 ||
               block_amount == number_of_processes)
            {
               add_process(&process_pid, &number_of_processes);
               print_table(BEFORE, process_pid,number_of_processes);
               running_process_bit = FALSE;
               running_process     = 
                            find_ruuning_process(&running_process_bit);
               sort_table (number_of_processes);
               print_table(AFTER, process_pid,number_of_processes);
            }
            else
               add_process(&process_pid, &number_of_processes);
         }  
      }
   }

   /* Free memory allocated to the process table                      */ 
   while (p_table != NULL)
   {
      /* Reset pointer to top of list */
      process = p_table;
      p_table    = p_table -> p_next_process;
      free(process);
   }


   return 0;
}

/**********************************************************************/
/*                   Print the process Table                          */
/**********************************************************************/
void print_table(int status, int process_pid, int number_of_processes)
{
   PROCESS *p_process = p_table; /* Points to every process           */

   if(status == BEFORE)
      printf("\n\n BEFORE ");
   else if(status == AFTER)
      printf("\n\n AFTER ");

   printf("SCHEDULING CPU:  Next PID = %d,  Number of Processes = %d",
          process_pid, number_of_processes);
   printf("\n PID   CPU Used   Max Time   STATE   PRI   ");
   printf("QUANTUM USED   BLK TIME   WAIT TKS");
   while (p_process = p_process->p_next_process,
          p_process->pid != LIST_TRAILER)
   {
      printf("\n  %2d",        p_process->pid);
      printf("     %2d",       p_process->cpu_used);
      printf("       %2d",     p_process->max_time);
      printf("           %c",  p_process->state);
      printf("      %2d",      p_process->pri);
      printf("        %2d",    p_process->quantum_used);
      printf("            %d", p_process->block_time);
      printf("          %3d",  p_process->wait_tks);
   }
   return;
}

/**********************************************************************/
/*    Create an empty linked list with header and trailer nodes       */
/**********************************************************************/
PROCESS *create_table()
{
   PROCESS *p_new_list; /* Points to the newly created list           */

   /* Get a new node and make it a list header                        */
   if((p_new_list = (PROCESS *) malloc(sizeof(PROCESS))) == NULL)
   {
      printf("\nAn error #%d occurred in initialize process table",
                                                 HEADER_NODE_ERR);
      printf("\nCannot allocate memory for the table header");
      printf("\nProgram is aborting");
      exit(HEADER_NODE_ERR);
   }
      
   p_new_list->pid = LIST_HEADER;
   
   /* Allocate new memory for trailer node                            */
   if((p_new_list->p_next_process = (PROCESS *) malloc(sizeof(PROCESS))) == NULL)
   {
      printf("\nAn error #%d occured in initialize process table",
                                               TRAILER_NODE_ERR);
      printf("\nCannot allocate memory for the table trailer");
      printf("\nProgram is aborting");
      exit(TRAILER_NODE_ERR);
   }
    
   p_new_list->p_next_process->pid            = LIST_TRAILER;
   p_new_list->p_next_process->p_next_process = NULL;
   
   return p_new_list;
}

/**********************************************************************/
/*        Insert a process at the bottom of the linked list           */
/**********************************************************************/
void add_process(int *process_pid, int *p_number_of_processes)
{
   PROCESS *p_process       = p_table, /* Points to every process     */
           *p_after_process = p_process->p_next_process,
                                       /* Points to the next process  */
           *p_new_process;             /* Points to the new process   */

   /* Navigate to bottom of the list to insert process                */
   while(p_after_process->pid != LIST_TRAILER)
   {
      p_process         = p_process->p_next_process;
      p_after_process   = p_after_process->p_next_process;
   }

   /* Allocate memory for a new process                               */
   if((p_new_process = (PROCESS *) malloc(sizeof(PROCESS))) == NULL)
      printf("I messed up");

   /* Link new node into linked list                                  */
   p_new_process->p_next_process = p_process->p_next_process;
   p_process->p_next_process     = p_new_process;

   /* Initialize standard process table values                        */
   p_new_process->pid          = *process_pid;        
   p_new_process->cpu_used     = 0;                  
   p_new_process->max_time     = ((rand()% 18) + 1);  
   p_new_process->state        = READY_STATE;                
   p_new_process->pri          = 0;                  
   p_new_process->quantum_used = 0;   
   p_new_process->wait_tks     = 0;                
   (*p_number_of_processes)++;
   (*process_pid)++;
   
   /* Assign block time                                               */
   if(((rand() % 3) + 1) == 1) 
      p_new_process->block_time = 6;
   else 
      p_new_process->block_time = ((rand() % 5)+1);

   return;
}

/**********************************************************************/
/*                        Schedule the process table                  */
/**********************************************************************/
PROCESS *find_ruuning_process(int *running_process)
{
   PROCESS *p_process = p_table, /* Points to every process           */
           *running   = NULL;    /* Points to the running process     */

   while(p_process = p_process->p_next_process, p_process->pid != LIST_TRAILER)
      if(p_process->state == READY_STATE && *running_process == FALSE )
      {
         p_process->state = RUNNING_STATE;
         *running_process = TRUE;
         running          = p_process;
      }

   return running;
}

/**********************************************************************/
/*                   Update the process table values                  */
/**********************************************************************/
void update_tick()
{
   PROCESS *p_process = p_table; /* Points to the every process       */

   while(p_process = p_process->p_next_process, 
      p_process->pid != LIST_TRAILER)
   {
      if(p_process->state == RUNNING_STATE)
         p_process->cpu_used++;

      if(p_process->state == RUNNING_STATE)
         p_process->quantum_used++;

      if(p_process->state == READY_STATE)
         p_process->wait_tks++; 
   }

   return;
}

/**********************************************************************/
/*                       Sort table by priortiy                       */
/**********************************************************************/
void sort_table( int number_of_processes) 
{
   int counter;
   PROCESS *p_previous, /* Points to the previous process             */
           *p_current,  /* points to the current process              */
           *p_next,     /* Points to the next process                 */
           *p_temp,     /* Temporary pointer for the swap             */
           *p_counter;  /* Temporary pointer for loop control         */


   for (counter = 1; counter < number_of_processes; counter++) 
   {
      p_previous = p_table;
      p_current = p_table->p_next_process;
      p_next = p_current->p_next_process;

      for (; p_next->pid != LIST_TRAILER;
           p_current = p_current->p_next_process,
           p_next = p_current->p_next_process) 
      {
         if (p_current->pri > p_next->pri) 
         {
            p_temp = p_next->p_next_process;
            p_current->p_next_process = p_temp;
            p_next->p_next_process = p_current;
            p_previous->p_next_process = p_next;
            
            p_current = p_next;
            p_next = p_temp;
         }
         p_previous = p_current;
      }
   }

   for (p_counter = p_table->p_next_process; p_counter->pri < 0;
        p_counter = p_counter->p_next_process) 
   {
      p_previous = p_table;
      p_current = p_table->p_next_process;
      p_next = p_current->p_next_process;

      for (; p_next->pri < 0; p_current = p_current->p_next_process,
           p_next = p_current->p_next_process) 
      {
         if (p_current->pri < p_next->pri) 
         {
            p_temp = p_next->p_next_process;
            p_current->p_next_process = p_temp;
            p_next->p_next_process = p_current;
            p_previous->p_next_process = p_next;
            
            p_current = p_next;
            p_next = p_temp;
         }
         p_previous = p_current;
      }
   }
            
   return;
}

/**********************************************************************/
/*                  Calculate new priority                            */
/**********************************************************************/
int new_priority(PROCESS *process)
{
   return (int)((abs(process->pri) + process->quantum_used) * 0.5f + 0.5f);
}

/**********************************************************************/
/*                           Remove Process                           */
/**********************************************************************/
void remove_process(int delete_pid, int *p_number_of_processes)
{
   PROCESS *p_process  = p_table, /* Points to every process          */
           *p_previous_process;   /* Points to the previous process   */

   while(p_process->p_next_process != NULL && 
         p_process->pid  != LIST_TRAILER)
      if(p_process->pid == delete_pid)
      {
         p_previous_process->p_next_process = p_process->p_next_process;
         (*p_number_of_processes)--;
         free(p_process);
         break; 
      }
      else
      {
         p_previous_process = p_process;
         p_process = p_process->p_next_process;
      }
   return;
}
