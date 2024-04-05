/*******************************************************************/
/*                      Include Statements                         */
/*******************************************************************/
#include <stdio.h>  /* gets, printf             */
#include <stdlib.h> /* malloc, free, exit, rand */
#include <math.h>   /* round                    */

/*******************************************************************/
/*                       Program Structures                        */
/*******************************************************************/
/* A process */
struct PROCESS
{
   int  pid;          /* Process ID number                      */
   int  cpu_used;     /* Total clock ticks used by process      */
   int  max_time;     /* Maximum CPU time needed by the process */
   char state;        /* Running, Blocked, or Ready state       */
   int  pri;          /* Process Priority number                */
   int  quantum_used; /* Amount of quantum used by the process  */
   int  block_time;   /* Ticks untill the process blocks        */
   int  wait_tks;     /* Ticks processed while in ready state   */ 

   struct PROCESS *p_next_process; /* Points to the next node     */
};
typedef struct PROCESS PROCESS;

/*******************************************************************/
/*                       Global Variables                          */
/*******************************************************************/
#define HEADER_NODE_ERR    1  /* Can't alloc header memory         */
#define TRAILER_NODE_ERR   2  /* Can't alloc trailer memory        */
#define INSERT_NODE_ERR    3  /* Can't alloc new node memory       */
#define MIN_PID            1  /* Lowest possible PID               */
#define MAX_PID            99 /* Greates possible PID              */
#define LIST_TRAILER       MAX_PID + 1 /* Last item in linked list value   */
#define LIST_HEADER        MIN_PID - 1 /* Start of linked list value      */
#define INIT_PROCESS_VALUE 5 /* Number of initialized process      */
#define TRUE               1 /* True value */
#define FALSE              0 /* False value */

/*******************************************************************/
/*                       Function Prototypes                       */
/*******************************************************************/
void print_table(PROCESS *p_table);
   /* Print the process table output                               */
PROCESS *create_table();
   /* Create an empty process table with header and trailer nodes  */
void add_process(PROCESS *p_table, int process_pid);              
   /* Add new process to the bottom of the process table           */
int count_process(PROCESS *p_table);                                             
   /* Count the number of process in the process table             */
void update_tick(PROCESS *p_table);
   /* Update table values for each clock tick                      */
void sort_table(PROCESS *p_table, int number_of_processes);
   /* Sort table by PRI                                            */
void remove_process(PROCESS *p_table, int delete_pid);
   /* Remove a process from the process table                      */
int schedule_table(PROCESS *p_table);
   /* Schedules process' on the process table                      */
int calculate_pri(int old_pri, int quantum_used);
   /* Calculates positive pri value                                */
int check_running(PROCESS *p_table);
   /* Checks if there is a running process or not                  */
int set_running(PROCESS *p_table);
   /* Function that sets the next process running                  */

/*******************************************************************/
/*                          Main Function                          */
/*******************************************************************/
int main() 
{
   int process_pid;         /* Loop control PID variable           */
   int number_of_processes; /* Number of process in table          */
   int running_process_bit = 0; /* Bit marks if process is running     */
   int print_schedule_bet;  /* Bit marks if table should print     */

   PROCESS *p_table,    /* Points to the process table             */
   	   *p_table_id, /* Used to free memory                     */
           *p_current;  /* Points to the process' within the table */

   /* Get pointer to newly created process list                    */
   p_table = create_table();

   /* Initialize process table with 5 processes                     */
   for (process_pid = 1; process_pid <= INIT_PROCESS_VALUE; process_pid++)
      add_process(p_table, process_pid);

   // Loop processing clock tick till PID is 99 
   while( process_pid <= 100 )
   {
      p_current = p_table->p_next_process; /* Points to every process in the process table */

      /* Loop traversing process table */
      while (p_current->pid != LIST_TRAILER)
      {
         /* Check if a process is running, set running bit, if not, set process running */
         if (check_running(p_table) == 0 )
         {
            printf("\n\nHere is the process table before scheduling");
            print_table(p_table);

            set_running(p_table);
            running_process_bit = 1;

            printf("\n\nHere is the process table after scheduling");
            print_table(p_table);
         }

         /* Block a running process */
         if (p_current->cpu_used == p_current->block_time 
             && p_current->state == 'N' 
             && running_process_bit == 1)
         {
            printf("\n\nHere is the process table before scheduling");
            print_table(p_table);

            if(p_current->block_time == 6)
            {
               p_current->pri = calculate_pri(p_current->pri, p_current->quantum_used); 
               p_current->state = 'R';
            }
            else
            {
               p_current->pri =(-1)*(calculate_pri(p_current->pri, p_current->quantum_used));
               p_current->state = 'B';
            }
         
            running_process_bit = 0;
            p_current->quantum_used = 0;

            // Sort table in order of priority 
            sort_table(p_table, count_process(p_table));
        
            /* Set next process running */
            set_running(p_table);
            running_process_bit = 1;

            printf("\n\nHere is the process table after scheduling");
            print_table(p_table);

         }

         /* Delete a running process and set next process to run */
         if (p_current->cpu_used == p_current->max_time 
             && p_current->state == 'N' && running_process_bit == 1)
         {
            printf("\n\nHere is the process table before scheduling");
            print_table(p_table);

            /* Delete process */
            remove_process(p_table, p_current->pid);

            /* Sort table */
            sort_table(p_table, count_process(p_table));
         
            /* Set next Ready process to running */
            running_process_bit = set_running(p_table);

            printf("\n\nHere is the process table after scheduling");
            print_table(p_table);
         }

      // Move down the list to the next process
      p_current = p_current->p_next_process;
   } 
      /* Update the table on every tick */
      update_tick(p_table);

      // 1 in 5 chance new process is added every clock tick
      if((((rand() % 5) + 1) == 1) && count_process(p_table) < 10 )
      {
         add_process(p_table, process_pid);
         process_pid++;
      }
   }

   // Release allocated memory for process table
   while (p_table != NULL)
   {
      /* Reset pointer to top of list */

      p_table_id = p_table;
      p_table    = p_table -> p_next_process;
      free(p_table_id);
   }

   return 0;
}

/**********************************************************************/
/*                   Print the process Table                          */
/**********************************************************************/
void print_table(PROCESS *p_table)
{
   PROCESS *p_current_process = p_table->p_next_process; // Points to every process within a linked list

   // Print struct members, only stop when reached trailer node 
   while (p_current_process->pid != LIST_TRAILER)
   {
      printf("\n        %3d", p_current_process->pid);
      printf(  "        %2d", p_current_process->cpu_used);
      printf(  "        %2d", p_current_process->max_time);
      printf(  "        %1c", p_current_process->state);
      printf(  "        %2d", p_current_process->pri);
      printf(  "        %2d", p_current_process->quantum_used);
      printf(  "        %2d", p_current_process->block_time);
      printf(  "        %2d", p_current_process->wait_tks);
 
      // Move to next node
      p_current_process = p_current_process->p_next_process;
   }
   return;
}


/***********************************************************************/
/*  Create an empty linked list with header and trailer nodes          */
/***********************************************************************/
PROCESS *create_table()
{
   PROCESS *p_new_list; /* Points to the newly created list            */

   /* Get a new node and make it a list header                         */
   (p_new_list = (PROCESS *) malloc(sizeof(PROCESS))) == NULL;

   p_new_list->pid = LIST_HEADER;
   
   /* Allocate new memory for trailer node                              */
   (p_new_list->p_next_process = (PROCESS *) malloc(sizeof(PROCESS))) == NULL;

   p_new_list->p_next_process->pid            = LIST_TRAILER;
   p_new_list->p_next_process->p_next_process = NULL;
   
   /* Return the pointer to the newly created linked list             */
   return p_new_list;
}

/**********************************************************************/
/*             insert a process at the bottom of the linked list      */
/**********************************************************************/
void add_process(PROCESS *p_table, int process_pid)
{
   PROCESS *p_current_process = p_table, // Points to every process
           *p_after_process = p_current_process->p_next_process,
           *p_new_process;     // Points to new process

   // Navigate to bottom of the list to insert process
   while (p_after_process->pid != LIST_TRAILER)
   {
      p_current_process = p_current_process->p_next_process;
      p_after_process   = p_after_process->p_next_process;
   }

   // Allocate memory for a new process
   (p_new_process = (PROCESS *) malloc(sizeof(PROCESS))) == NULL;

   // Link new node into linked list
   p_new_process->p_next_process = p_current_process->p_next_process;
   p_current_process->p_next_process = p_new_process;

   // Initialize standard process table values
   p_new_process->pid = process_pid;               /* Assign process id number               */
   p_new_process->cpu_used = 0;                    /* Total clock ticks used by process      */
   p_new_process->max_time = ((rand()% 18) + 1);  /* Maximum CPU time needed by the process */
   p_new_process->state = 'R' ;                    /* Running, Blocked, or Ready state       */
   p_new_process->pri = 0;                         /* Process Priority number                */
   p_new_process->quantum_used = 0;                /* Amount of quantum used by the process  */
   
   // Assign block time
   if(((rand() % 3) + 1) == 1) 
      p_new_process->block_time = 6;
   else 
      p_new_process->block_time = ((rand() % 5)+1);

   p_new_process->wait_tks = 0;                    /* Ticks processed while in ready state   */ 
   return;
}

/************************************************************************/
/*                     Count the processes in the table                 */
/************************************************************************/
int count_process(PROCESS *p_table)
{
   int process_count = 0; // Counts the number of process in the table
   
   while ( p_table=p_table->p_next_process, p_table->pid != LIST_TRAILER)
      process_count++;

   return(process_count);
}

/************************************************************************/
/*                   Update the process table values                    */
/************************************************************************/
void update_tick(PROCESS *p_table)
{
   PROCESS *p_current_process = p_table->p_next_process; // Points to the current process

   // Traverse the loop and update each value
   while(p_current_process->pid != LIST_TRAILER)
   {
      // Update CPU Used
      if (p_current_process->state == 'N')
         p_current_process->cpu_used++;

      // Update quantum used
      if (p_current_process->state == 'N')
      p_current_process->quantum_used++;
         
      // Update wait TKS
      if (p_current_process->state == 'R')
         p_current_process->wait_tks++; 

      // 1/20 chance any blocked process becomes unblocked every clock tick
      if (p_current_process->state == 'B')
      {
         if(((rand()% 20) + 1) == 1)
            p_current_process->state = 'R';
      }


      p_current_process = p_current_process->p_next_process;
   }
   return;
}


/************************************************************************/
/*                   Sort table by priortiy                             */
/************************************************************************/
void sort_table(PROCESS *p_table, int number_of_processes) 
{
   int counter;
   PROCESS *p_previous, *p_current, *p_next, *p_temp, *p_counter;

   // Sort list in ascending order by PRI
   for (counter = 1; counter < number_of_processes; counter++) 
   {
      p_previous = p_table;
      p_current = p_table->p_next_process;
      p_next = p_current->p_next_process;

      for (; p_next->pid != LIST_TRAILER; p_current = p_current->p_next_process, p_next = p_current->p_next_process) 
      {
         if (p_current->pri > p_next->pri) 
         {
            p_temp = p_next->p_next_process;
            p_current->p_next_process = p_temp;
            p_next->p_next_process = p_current;
            p_previous->p_next_process = p_next;
            
            // Update p_current and p_next to their correct positions after the swap
            p_current = p_next;
            p_next = p_temp;
         }
         p_previous = p_current;
      }
   }

   // Sort the top half of the list (the negative numbers) in descending order
   for (p_counter = p_table->p_next_process; p_counter->pri < 0; p_counter = p_counter->p_next_process) 
   {
      p_previous = p_table;
      p_current = p_table->p_next_process;
      p_next = p_current->p_next_process;

      for (; p_next->pri < 0; p_current = p_current->p_next_process, p_next = p_current->p_next_process) 
      {
         if (p_current->pri < p_next->pri) 
         {
            p_temp = p_next->p_next_process;
            p_current->p_next_process = p_temp;
            p_next->p_next_process = p_current;
            p_previous->p_next_process = p_next;
            
            // Update p_current and p_next to their correct positions after the swap
            p_current = p_next;
            p_next = p_temp;
         }
         p_previous = p_current;
      }
   }

   
           
   return;
}

/************************************************************************/
/*                           Remove Process                             */
/************************************************************************/
void remove_process(PROCESS *p_table, int delete_pid)
{
   PROCESS *p_current_process = p_table->p_next_process, // Points to the current process
           *p_previous_process = p_table,                // Points to the previous process
           *p_temp;                                      // To switch processes

   while (p_current_process->pid != LIST_TRAILER)
   {
      // Check if the current process should be deleted
      if (p_current_process->pid == delete_pid)
      {
         // Unhook process from linked list
         p_previous_process->p_next_process = p_current_process->p_next_process;
         p_temp = p_current_process;
         p_current_process = p_current_process->p_next_process;

         // Free memory for allocated for that node
         free(p_temp);
      }
      else
      {
         // Move to the next process
         p_previous_process = p_current_process;
         p_current_process = p_current_process->p_next_process;
      }
   }
}
/************************************************************************/
/*                           Calculate PRI                              */
/************************************************************************/
int calculate_pri(int old_pri, int quantum_used)
{
   return ((int)((float)(((abs(old_pri) + quantum_used) * 0.5f) + 0.5f)));
}

/************************************************************************/
/*                   Check if a Process is Running                      */
/************************************************************************/
int check_running(PROCESS *p_table)
{
   p_table = p_table->p_next_process; /* Point to every process */

   while(p_table->pid != LIST_TRAILER)
   {
      if (p_table->state == 'N')
         return (1);

      p_table = p_table->p_next_process;
   } 

   return 0;
}
/************************************************************************/
/*                      Set a Process Running                           */
/************************************************************************/
int set_running(PROCESS *p_table)
{
   PROCESS *p_current = p_table->p_next_process; /* Points to every process */

   while(p_current->pid != LIST_TRAILER)
   {
      if (p_current->state == 'R')
      {
         p_current->state = 'N';
         return 1;
      }
      p_current = p_current->p_next_process;
   }

   return (0);
}
