/************************************************************************
 *
 * FILE:
 *       test_ups_memory.c
 * 
 * DESCRIPTION: 
 *       Test the memory management routines
 *
 * AUTHORS:
 *       Eileen Berman
 *       David Fagan
 *       Lars Rasmussen
 *
 *       Fermilab Computing Division
 *       Batavia, Il 60510, U.S.A.
 *
 * MODIFICATIONS:
 *       25-Jul-1997, EB, first
 *
 ***********************************************************************/

/* standard include files */
#include <stdio.h>

/* ups specific include files */
#include "ups_memory.h"
#include "ups_utils.h"
#include "ups_error.h"

/*
 * Definition of public variables.
 */

/*
 * Declaration of private functions.
 */

/*
 * Definition of global variables.
 */

/*
 * Definition of public functions.
 */

int main(void)
{
  char *myc, *myc2, *myc3, *myc4, *myc5, *myc6;
  int myc_bytes = 20, myc2_bytes = 30, myc3_bytes = 40, myc4_bytes = 50;
  int myc5_bytes = 60, myc6_bytes = 70;

  upsutl_start_timing();

  printf("\nPrint out the memory list before adding anything, should get nothing\n");
  upsmem_print();

  /* Now malloc a few things */
  myc = (char *)upsmem_malloc(myc_bytes);
  myc2 = (char *)upsmem_malloc(myc2_bytes);
  myc3 = (char *)upsmem_malloc(myc3_bytes);
  myc4 = (char *)upsmem_malloc(myc4_bytes);
  myc5 = (char *)upsmem_malloc(myc5_bytes);
  myc6 = (char *)upsmem_malloc(myc6_bytes);

  printf("\nPrint out the memory list, should have 6 items with sizes %d, %d, %d, %d, %d, %d\n",
	myc6_bytes, myc5_bytes, myc4_bytes, myc3_bytes, myc2_bytes, myc_bytes);
  upsmem_print();

  /* Now increment a few reference counters */
  upsmem_inc_refctr((void *)myc);
  upsmem_inc_refctr((void *)myc3);
  upsmem_inc_refctr((void *)myc3);
  upsmem_inc_refctr((void *)myc3);
  upsmem_inc_refctr((void *)myc5);
  upsmem_inc_refctr((void *)myc5);
  upsmem_inc_refctr((void *)myc5);
  upsmem_inc_refctr((void *)myc5);
  upsmem_inc_refctr((void *)myc5);
  upsmem_inc_refctr((void *)myc6);
  upsmem_inc_refctr((void *)myc6);
  upsmem_inc_refctr((void *)myc6);

  printf("\nIncremented reference counters, should be 3, 5, 0, 3, 0, 1\n");
  upsmem_print();

  /* Decrement the reference counters */
  upsmem_dec_refctr((void *)myc2);
  upsmem_dec_refctr((void *)myc3);
  upsmem_dec_refctr((void *)myc5);
  upsmem_dec_refctr((void *)myc5);
  upsmem_dec_refctr((void *)myc5);

  printf("\nDecremented counters, now have 6 items with reference counters - 3, 2, 0, 2, -1, 1\n");
  upsmem_print();

  /* now free everything */
  upsmem_dec_refctr((void *)myc);
  upsmem_dec_refctr((void *)myc3);
  upsmem_dec_refctr((void *)myc3);
  upsmem_dec_refctr((void *)myc5);
  upsmem_dec_refctr((void *)myc5);
  upsmem_dec_refctr((void *)myc6);
  upsmem_dec_refctr((void *)myc6);
  upsmem_dec_refctr((void *)myc6);
  printf("\nDecremented all counters, now have 6 items with reference counters < 0\n");
  upsmem_print();

  upsmem_free(myc);
  upsmem_free(myc2);
  upsmem_free(myc3);
  upsmem_free(myc4);
  upsmem_free(myc5);
  upsmem_free(myc6);

  printf("\nPrint out empty memory list again\n");
  upsmem_print();

  /* Do an extra free - see if we blow up */
  upsmem_free(myc);

  upsutl_stop_timing();
  upserr_output();


  return 0;
}


