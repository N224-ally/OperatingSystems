/**********************************************************************/
/*                                                                    */
/* Program Name: driver - Creates a DISK device driver                */
/* Author:       Niwe Mugeni                                          */
/* Installation: Pensacola Christian College, Pensacola, Florida      */
/* Course:       CS326, Operating Systems                             */
/* Date Written: April 29, 2024                                       */
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
/* This is a disk device driver which accepts read/write requests     */
/* from a FILE SYSTEM, translates them from physical block numbers    */
/* into disk drive cylinder, track, and sector numbers, then          */
/* instructs a disk device to carry out the read and write requests   */
/*                                                                    */
/**********************************************************************/

#include <stdio.h>  
#include <stdlib.h> /* Malloc, free(), exit()                         */

/**********************************************************************/
/*                         Symbolic Constants                         */
/**********************************************************************/
#define TRUE               1    /* True Value                         */
#define FALSE              0    /* False value                        */
#define SECTORS_PER_TRACK  9    /* Number of sectors per track        */
#define FS_MESSAGE_COUNT   20   /* Messages array size                */
#define SENSE_CYLINDER     1    /* Code to sense cylinder position    */
#define SEEK_CYLINDER      2    /* Code to send heads to cylinder     */
#define DMA_SETUP          3    /* Code to set DMA chip registers     */
#define START_MOTOR        4    /* Code to turn DISK drive motor on   */
#define MOTOR_STATUS       5    /* Code to wait for motor's speed     */
#define READ_DISK          6    /* Code to read data                  */
#define WRITE_DISK         7    /* Code to write data                 */
#define STOP_MOTOR         8    /* Code to turn DISK drive motor off  */
#define RECALIBRATE        9    /* Code to reset DISK heads           */
#define BYTES_PER_BLOCK    1024 /* Number of bytes per block          */
#define OP_CODE_ERROR      -1   /* Invalid operation code error       */
#define REQUEST_NUM_ERROR  -2   /* Invalid request number error       */
#define BLOCK_NUM_ERROR    -4   /* Invalid block number error         */
#define BLOCK_SIZE_ERROR   -8   /* Invalid block size error           */
#define DATA_ADDRESS_ERROR -16  /* Invalid data address error         */

/**********************************************************************/
/*                         Program Structures                         */
/**********************************************************************/
/* A file system message                                              */
struct message 
{
                int operation_code,  /* Operation to be performed     */
                    request_number,  /* Unique request number         */
                    block_number,    /* Block number to read/write    */
                    block_size;      /* Block size in bytes           */
  unsigned long int *p_data_address; /* Points to a block in memory   */
};
typedef struct message MESSAGE;

/**********************************************************************/
/*                        Function Prototypes                         */
/**********************************************************************/
void copy_messages(int *request_total);
   /* Copy File System requests to the request list                   */
void sort_messages();
   /* Sort requests list by ascending block number                    */
void convert_block(int block_number, int *p_track, int *p_sector);
   /* Convert a block number to track, and sector numbers             */
void convert_cylinder(int *p_cylinder, int block_number);
   /* Convert a block number to cylinder number                       */
void get_next_request(int *p_index, int cylinder, int request_total);
   /* Get an read/write request                                       */
void remove_request(int completed_request);
   /* Removes a request from the requests list                        */
int  validate(int index);
   /* Validate a pending request for any incorrect parameters         */   
void send_idle_request();
   /* Send an idle message to the file system                         */
void send_message(MESSAGE *p_fs_message);
   /* Send a message to the FILE SYSTEM from the driver               */
int  disk_drive(int code, int arg1, int arg2, int arg3, 
                                             unsigned long int *p_arg4);
   /* Communicates with the disk                                      */

/**********************************************************************/
/*                         Global Variables                           */
/**********************************************************************/
MESSAGE fs_message  [FS_MESSAGE_COUNT]; /* File system messages       */
MESSAGE request_list[FS_MESSAGE_COUNT]; /* Request list messages      */

/**********************************************************************/
/*                         Main Function                              */
/**********************************************************************/
int main() 
{ 
   int counter,               /* Count every request                  */
       cylinder,              /* Cylinder number for the request      */
       disk_cylinder = 0,     /* The Cylinder the heads are on        */
       disk_on       = FALSE, /* Status of the disk motor             */
       error_code,            /* Indicates invalid request paramters  */
       idle_counter  = 0,     /* Countes the idle requests            */
       request,               /* Current request index                */
       request_total = 0,     /* Total number of pending requests     */
       sector,                /* Sector number for the  request       */
       track;                 /* Track number for  the request        */

   /* Initialize the driver's requests list                           */
   for(counter = 0; counter < FS_MESSAGE_COUNT ; counter++)
      request_list[counter].operation_code = 0;

   /* Loop processing file system requests                            */
   while(TRUE) 
   {
      /* Sending idle request until file system responds              */
      while(request_total == 0) 
      {
         idle_counter += 1;
         send_idle_request();
         send_message     (fs_message);
         copy_messages    (&request_total);

         if(idle_counter >= 1) 
         {
            if(disk_on == TRUE)
               disk_on = disk_drive(STOP_MOTOR, 0, 0, 0, 0);
            idle_counter = 0;
         }
      }

      /* Check if the disk is on, then process the request            */
      if(disk_on == TRUE) 
      {
         get_next_request(&request, disk_cylinder, request_total);
         convert_block   (request_list[request].block_number, 
                          &track, &sector);
         convert_cylinder(&cylinder, request_list[request].block_number);

         if(disk_cylinder != cylinder) 
         {
            disk_cylinder = disk_drive(SEEK_CYLINDER, cylinder, 0, 0, 0);
            while(disk_cylinder != cylinder)
               disk_cylinder = disk_drive(RECALIBRATE, 0, 0, 0, 0);
         }

         error_code 
            = validate(request);
         fs_message[0].operation_code
            = error_code;
         fs_message[0].request_number 
            = request_list[request].request_number;
         fs_message[0].block_number   
            = request_list[request].block_number;
         fs_message[0].block_size     
            = request_list[request].block_size;
         fs_message[0].p_data_address 
            = request_list[request].p_data_address;

         /* Verify no errors before disk read/write operations        */
         if(error_code == 0) 
         {
            disk_drive(DMA_SETUP, sector, track,
               request_list[request].block_size, 
               request_list[request].p_data_address);
             
            if(request_list[request].operation_code == 1)
               while(disk_drive(READ_DISK, 0, 0, 0, 0));
            else
               while(disk_drive(WRITE_DISK, 0, 0, 0, 0));
         }
         
         /* Remove, sort, and send message to the disk               */
         remove_request(request);
         request_total--;
         sort_messages ();
         send_message  (fs_message);
         copy_messages (&request_total);
      } 
      else 
      {
         disk_on       = disk_drive(START_MOTOR, 0, 0, 0, 0);
         disk_drive(MOTOR_STATUS, 0, 0, 0, 0);
         disk_cylinder = disk_drive(SENSE_CYLINDER, 0, 0, 0, 0);
      }
  }

  return 0;
}

/**********************************************************************/
/*           Copy file system requests to the request list            */                                                           
/**********************************************************************/
void copy_messages(int *p_request_total) 
{
   int counter = 0; /* Counts all the requests                        */

   while (counter < FS_MESSAGE_COUNT 
          && fs_message[counter].operation_code != 0) 
   {
      request_list[*p_request_total].operation_code 
         = fs_message[counter].operation_code;
      request_list[*p_request_total].request_number 
         = fs_message[counter].request_number;
      request_list[*p_request_total].block_number  
         = fs_message[counter].block_number;
      request_list[*p_request_total].block_size     
         = fs_message[counter].block_size;
      request_list[*p_request_total].p_data_address 
         = fs_message[counter].p_data_address;
      sort_messages();
      (*p_request_total)++;
      counter++;
   }

   return;
}

/**********************************************************************/
/*            Sort requests list by ascending block number            */                                                                
/**********************************************************************/
void sort_messages() 
{
   int     counter;          /* Count every request                   */
   
   MESSAGE temp_message,     /* Temporary message for swap            */
           *p_message,       /* Current request                       */
           *p_message_start  = request_list; 
                             /* Start of the request list             */

   for(counter = 1; counter < FS_MESSAGE_COUNT; counter++) 
      for(p_message = p_message_start; 
                        (p_message - p_message_start) < 19; p_message++) 
         if((p_message + 1)->operation_code != 0) 
         {
            if((p_message)->operation_code == 0) 
            {
               temp_message     = *(p_message + 1);
               *(p_message + 1) = *p_message;
               *p_message       = temp_message;
            } 
            else if((p_message + 1)->block_number < p_message->block_number) 
            {
               temp_message     = *(p_message + 1);
               *(p_message + 1) = *p_message;
               *p_message       = temp_message;
            }
         }

  return;
}

/**********************************************************************/
/*        Convert a block number to track, and sector numbers         */
/**********************************************************************/
void convert_block(int block_number, int *p_track, int *p_sector) 
{
   *p_track  = (block_number - 1) % SECTORS_PER_TRACK;
   *p_track  = *p_track >= SECTORS_PER_TRACK * 0.5f ? 1 : 0;
   *p_sector = (block_number - 1) % (SECTORS_PER_TRACK)*2;

   if(*p_sector > SECTORS_PER_TRACK)
      *p_sector = *p_sector - SECTORS_PER_TRACK;

  return;
}

/**********************************************************************/
/*              Convert a block number to cylinder number             */
/**********************************************************************/
void convert_cylinder(int *p_cylinder, int block_number) 
{  
   *p_cylinder = (block_number - 1) / SECTORS_PER_TRACK;

   return;
}

/**********************************************************************/
/*                     Get an read/write request                      */                                                              
/**********************************************************************/
void get_next_request(int *p_request, int request_cylinder, int request_total) 
{
   int counter  = 0, /* Counts every request                          */
       cylinder = 0, /* Requested Cylinder                            */
       found    = 0; /* Result is not found                           */

   while (counter <= request_total) 
   {
      convert_cylinder(&cylinder, request_list[counter].block_number);
      if(cylinder >= request_cylinder)
      {
         found = 1;
         *p_request = counter;
         break;
      }
      counter++;
   }

   if(found == 0)
      *p_request = 0;

   return;
}

/**********************************************************************/
/*              Removes a request from the requests list              */
/**********************************************************************/
void remove_request(int completed_request) 
{
   request_list[completed_request].operation_code = 0;
   request_list[completed_request].request_number = 0;
   request_list[completed_request].block_number   = 0;
   request_list[completed_request].block_size     = 0;
   request_list[completed_request].p_data_address = NULL;
}

/**********************************************************************/
/*       Validate a pending request for any incorrect parameters      */                                                                 
/**********************************************************************/
int validate(int index) 
{
   int error_code = 0; /* Error of current request                    */

   if(request_list[index].operation_code != 1 &&
      request_list[index].operation_code != 2)
      error_code = OP_CODE_ERROR;
   else if(request_list[index].request_number <= 0)
      error_code = REQUEST_NUM_ERROR;
   else if(request_list[index].block_number <= 0 ||
      request_list[index].block_number > BYTES_PER_BLOCK)
      error_code = BLOCK_NUM_ERROR;
   else if(request_list[index].block_size < 0 ||
     (request_list[index].block_size &(request_list[index].block_size - 1)))
      error_code = BLOCK_SIZE_ERROR;
   else if(request_list[index].p_data_address < 0)
      error_code = DATA_ADDRESS_ERROR;

  return error_code;
}

/**********************************************************************/
/*               Send an idle message to the file system              */
/**********************************************************************/
void send_idle_request() 
{
   fs_message[0].operation_code = 0;
   fs_message[0].request_number = 0;
   fs_message[0].block_number   = 0;
   fs_message[0].block_size     = 0;
   fs_message[0].p_data_address = NULL;
   return;
}
