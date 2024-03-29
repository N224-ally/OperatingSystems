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
#define HEADER_NODE_ERR    1   /* Can't alloc header memory        */
#define TRAILER_NODE_ERR   2   /* Can't alloc trailer memory       */
#define INSERT_NODE_ERR    3   /* Can't alloc new node memory      */
#define MIN_PID            1   /* Lowest possible PID              */
#define MAX_PID            99  /* Greates possible PID             */
#define LIST_TRAILER       MAX_PID + 1 /* Last item in linked list value   */
#define LIST_HEADER        MIN_PID - 1 /* Start of linked list value      */
#define INIT_PROCESS_VALUE 5   /* Number of initialized process    */
#define MIN_

/*******************************************************************/
/*                       Function Prototypes                       */
/*******************************************************************/
void print_table(PROCESS *p_table);
   /* Print the process table output                               */
PROCESS *create_table();
   /* Create an empty process table with header and trailer nodes  */
void add_process(PROCESS *p_table, int process_pid);              
   /* Add new process to process table                             */
int count_process(PROCESS *p_table);                                             
   /* Count the number of process in the process table             */
void delete_process(PROCESS *p_table, int process_pid);
   /* Delete a process from the table                              */
void update_tick(PROCESS *p_table);
   /* Update table values for each clock tick                      */
void sort_table(PROCESS *p_table, int number_of_processes);
   /* Sort table by PRI                                            */

/*******************************************************************/
/*                          Main Function                          */
/*******************************************************************/
int main() 
{
   int process_pid;     /* Loop control PID variable               */
   int number_of_processes; /* Number of process in the process table    */

   PROCESS *p_table,    /* Points to the process table             */
           *p_table_id; /* Points to the process' within the table */

   /* Get pointer to newly created process list                    */
   p_table = create_table();

   /* Initialize process table with 5 processes                     */
   for (process_pid = 1; process_pid <= INIT_PROCESS_VALUE; process_pid++)
      add_process(p_table, process_pid);

   int number = 0;
   // Testing update function
   while( number != 5)
   {
      printf("\n\nThis is the table before updating any values");
      print_table(p_table);

      update_tick(p_table);

      printf("\n\nThis is the table after updating any values");
      print_table(p_table);

      number_of_processes = count_process(p_table);
      printf("\n\nThis is the table after sorting values");
      sort_table(p_table, number_of_processes);

      number++;
   }

   // print table
   print_table(p_table);

   // count process
   count_process(p_table);

   // Release allocated memory for process table
   while (p_table != NULL)
   {
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
           *p_new_process;     // Points to new process

   // check if this is the right process to link too
   while (p_current_process->pid != (process_pid - 1))
      p_current_process = p_current_process->p_next_process;      

   // Allocate memory for a new process
   (p_new_process = (PROCESS *) malloc(sizeof(PROCESS))) == NULL;

   // Link new node into linked list
   p_new_process->p_next_process = p_current_process->p_next_process;
   p_current_process->p_next_process = p_new_process;

   // Initialize standard process table values
   p_new_process->pid = process_pid;               /* Assign process id number               */
   p_new_process->cpu_used = 0;                    /* Total clock ticks used by process      */
   p_new_process->max_time = ((rand()% 18) + 1 );  /* Maximum CPU time needed by the process */
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

   return process_count;
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

      // Update PRI
      if (p_current_process->state != 'N')
      {
         if (p_current_process->state == 'B')
            p_current_process->pri = (-1) * ((abs(p_current_process->pri) + p_current_process->quantum_used) / 2);
      }
      else
       p_current_process->pri = ((abs(p_current_process->pri) + p_current_process->quantum_used) / 2);

      // Update quantum used
      if (p_current_process->state == 'N')
      p_current_process->quantum_used++;

      // Update BLK time 
      if (p_current_process->state == 'B')
      {
         p_current_process->block_time -= p_current_process->quantum_used;
         p_current_process->quantum_used = 0;
      }
         
      // Update wait TKS
      if (p_current_process->state == 'R')
         p_current_process->wait_tks++;  

      p_current_process = p_current_process->p_next_process;
   }
   return;
}

/************************************************************************/
/*                   Sort table by priortiy                             */
/************************************************************************/
void sort_table(PROCESS *p_table, int number_of_processes)
{
   int counter;        // Count the number of processes in the table 
   PROCESS *p_current = p_table->p_next_process, // Points to the top of the process table
           *p_next    = p_current->p_next_process ,    // Points to next process
           *p_temp;    // Temporary pointer for loop variables

   // Sort list in ascending order by PRI
   for (counter = 1; counter < number_of_processes; counter++)
      for (p_current; p_next->pid != LIST_TRAILER; p_current->p_next_process)
         if (p_current->pri > p_next->pri)
         {
            p_temp    = p_current;
            p_current = p_next;
            p_next    = p_temp;
         }
   
   // Reverse the order of negtive numbers which are at the top of the list
   for (p_temp = p_table->p_next_process; p_temp->pri < 0; p_temp->p_next_process)
      for (p_current; p_next->pri > 0; p_current->p_next_process)
         if (p_current->pri < p_next->pri)
         {
            p_temp    = p_current;
            p_current = p_next;
            p_next    = p_temp;
         }
   return;
}
