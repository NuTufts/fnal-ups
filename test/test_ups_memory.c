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

  printf("\nPrint out the memory list before adding anything, should get nothing\n");
  umem_print();

  /* Now malloc a few things */
  myc = (char *)umem_malloc(myc_bytes);
  myc2 = (char *)umem_malloc(myc2_bytes);
  myc3 = (char *)umem_malloc(myc3_bytes);
  myc4 = (char *)umem_malloc(myc4_bytes);
  myc5 = (char *)umem_malloc(myc5_bytes);
  myc6 = (char *)umem_malloc(myc6_bytes);

  printf("\nPrint out the memory list, should have 6 items with sizes %d, %d, %d, %d, %d, %d\n",
	myc6_bytes, myc5_bytes, myc4_bytes, myc3_bytes, myc2_bytes, myc_bytes);
  umem_print();

  /* Now increment a few reference counters */
  umem_inc_refctr((void *)myc);
  umem_inc_refctr((void *)myc3);
  umem_inc_refctr((void *)myc3);
  umem_inc_refctr((void *)myc3);
  umem_inc_refctr((void *)myc5);
  umem_inc_refctr((void *)myc5);
  umem_inc_refctr((void *)myc5);
  umem_inc_refctr((void *)myc5);
  umem_inc_refctr((void *)myc5);
  umem_inc_refctr((void *)myc6);
  umem_inc_refctr((void *)myc6);
  umem_inc_refctr((void *)myc6);

  printf("\nIncremented reference counters, should be 3, 5, 0, 3, 0, 1\n");
  umem_print();

  /* Decrement the reference counters */
  umem_dec_refctr((void *)myc2);
  umem_dec_refctr((void *)myc3);
  umem_dec_refctr((void *)myc5);
  umem_dec_refctr((void *)myc5);
  umem_dec_refctr((void *)myc5);

  printf("\nDecremented counters, now have 6 items with reference counters - 3, 2, 0, 2, -1, 1\n");
  umem_print();

  /* now free everything */
  umem_dec_refctr((void *)myc);
  umem_dec_refctr((void *)myc3);
  umem_dec_refctr((void *)myc3);
  umem_dec_refctr((void *)myc5);
  umem_dec_refctr((void *)myc5);
  umem_dec_refctr((void *)myc6);
  umem_dec_refctr((void *)myc6);
  umem_dec_refctr((void *)myc6);
  printf("\nDecremented all counters, now have 6 items with reference counters < 0\n");
  umem_print();

  umem_free(myc);
  umem_free(myc2);
  umem_free(myc3);
  umem_free(myc4);
  umem_free(myc5);
  umem_free(myc6);

  printf("\nPrint out empty memory list again\n");
  umem_print();

  /* Do an extra free - see if we blow up */
  umem_free(myc);

  return 0;
}


