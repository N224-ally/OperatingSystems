/**********************************************************************/
/*                                                                    */
/* Program Name: Convert - Recieve and convert a block number to its  */
/*                         corresponding disk location                */
/* Author:       Niwe Mugeni                                          */
/* Installation: Pensacola Christian COlelge, Pensacola Flordia       */
/* Course:       CS326, Operating Systems                             */
/* Date Written: April 10, 2024                                       */
/*                                                                    */
/**********************************************************************/

/**********************************************************************/
/*                                                                    */
/* This program prints the corresponding cylinder, track, and sector  */
/* number of disk numbers from 1 to 360                               */
/*                                                                    */
/**********************************************************************/

#include <stdio.h>
#include <stdlib.h>

/**********************************************************************/
/*                          Symbolic Consants                         */
/**********************************************************************/
#define BLOCK_BYTES  1024 /* Number of bytes per disk block           */
#define SECTOR_BYTES 512  /* Number of bytes per sector               */
#define CYLINDER_NUM 40   /* Number of cylinders on the disk          */
#define SECTOR_NUM   9    /* Number of sectors per track              */
#define TRACK_NUM    2    /* Number of tracks per cylinder            */


/**********************************************************************/
/*                       Function Prototypes                          */
/**********************************************************************/
void convert_block(int block_number, int *p_cylinder, int *p_track,
                                                      int *p_sector);
   /* Converts block number to corresponding cylinder, track, and     */
   /* sector                                                          */

/**********************************************************************/
/*                        Main Function                               */
/**********************************************************************/
int main()
{
   int block_num,    /* Current block number                          */
       block_total,  /* Total number of blocks                        */
       cylinder_num, /* Cylinder value                                */
       sector_num,   /* Sector value                                  */
       track_num;    /* Track value                                   */

   /* Calculate block total                                           */
   block_total = ((CYLINDER_NUM * TRACK_NUM * SECTOR_NUM *
                   SECTOR_BYTES) / BLOCK_BYTES);

   /* Print heading                                                   */
   printf("\n Block       Cylinder       Track      Sector");
   printf("\n--------    -----------    -------     -------");
   
   /* Loop processing and displaying all blocks                       */
   for (block_num = 1; block_num <= block_total; block_num++)
   {
      convert_block(block_num, &cylinder_num, &track_num, &sector_num);

      printf("\n %3d",          block_num); 
      printf("            %2d", cylinder_num);                          
      printf("            %d",  track_num);
      printf("           %d",   sector_num);
   }

   return 0;
}

/**********************************************************************/
/* Convert block number to corresponding cylinder, track, and sector  */
/**********************************************************************/
void convert_block(int block_number, int *p_cylinder, int *p_track, 
                                                      int *p_sector)
{
   /* Convert block number to cylinder number                         */
   *p_cylinder = ((block_number - 1) / SECTOR_NUM);

   /* Convert block number to track number                            */
   if(((block_number - 1) % SECTOR_NUM) < ((float)SECTOR_NUM * 0.5f))
      *p_track = 0;
   else
      *p_track = 1;

   /* Convert block number to sector number                           */
   *p_sector = ((block_number - 1) % SECTOR_NUM) * TRACK_NUM;

   if(SECTOR_NUM <= *p_sector)
      *p_sector -= SECTOR_NUM;

   return;
}
