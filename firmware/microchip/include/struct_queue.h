/* struct_queue.h
 *************************************************************************
 * Filename:        struct_queue.h
 * Dependancies:    generic.h
 * Processor:       Any
 * Hardware:        Any
 * Assembler:       NA
 * Linker:          Any
 * Company:         Microchip Technology, Inc.
 *************************************************************************
 * Software License Agreement:
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the Company) for its PICmicro Microcontroller is intended and
 * supplied to you, the Company's customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *************************************************************************
 * File Description:
 * 
 * This file defines primative operations and data structures that provide
 * a queue abstraction for same-sized structs of data.
 *
 * !!!! IMPORTANT:  The caller is responsible for copying the data. !!!!
 *
 * StructQueue Operations:
 * 
 *      StructQueueInit       - Initializes a queue and makes it empty.
 *      StructQueueAdd        - Adds an item to a queue.
 *      StructQueueRemove     - Removes an item from a queue.
 *      StructQueueIsFull     - Checks to see if a queue is full.
 *      StructQueueIsNotFull  - Checks to see if a queue is not full.
 *      StructQueueIsEmpty    - Checks to see if a queue is empty.
 *      StructQueueIsNotEmpty - Checks to see if a queue is not empty.
 *      StructQueueCount      - Provides the count of items a queue.
 *
 * Usage:
 *
 *      Code using the struct queue operations must define and allocate a
 *      struct-queue structure.  A struct-queue structure must have the 
 *      following members.  
 *
 *      head        - Contains the index to the current head item
 *      tail        - Contains the incex to the current tail item
 *      count       - Contains the current number of items in the queue
 *      buffer[]    - Array of queue items
 *
 *      Example:
 *
 *          // This structure defines the items the queue will hold.
 *          typedef struct _my_structure
 *          {
 *              //... Define any desired number of elements
 *          } QUEUE_ITEM;
 *
 *          // This defines how many items the queue will hold.
 *          #define SIZE_OF_MY_QUEUE    8
 *
 *          // This is the queue data structure.
 *          typedef struct _my_queue
 *          {
 *              int         head;
 *              int         tail;
 *              int         count;
 *              QUEUE_ITEM  buffer[SIZE_OF_MY_QUEUE];
 *          } MY_QUEUE;
 *
 *      Then, allocate as many queues of this type as desired.
 *
 *      Example:
 *
 *          MY_QUEUE my_queue1;
 *          MY_QUEUE my_queue2;
 *          //...
 *
 *      Before a queue can be used, it must be initialized using the 
 *      StructQueueInit operation.
 *
 *      Example:
 *
 *          StructQueueInit(&my_queue1, SIZE_OF_MY_QUEUE);
 *
 *      Before peforming an operation that will change the state of a
 *      queue (StructQueueAdd or StructQueueRemove), it is necessary to
 *      check the state of the queue to make sure it is safe to perform
 *      the desired task.  (See examples, below.)
 *
 * IMPORTANT:
 *
 *      The caller is responsible for copying the data.  The StructQueueAdd
 *      and StructQueueRemove operations will only provide pointers to the
 *      item in the array.  They do not copy any data.
 *
 *      Example:
 *
 *          QUEUE_ITEM *p_item;
 *
 *          // Remove an item 
 *          if (StructQueueIsNotFull(&my_queue1, SIZE_OF_MY_QUEUE))
 *          {
 *              p_item = StructQueueAdd(&my_queue1, SIZE_OF_MY_QUEUE);
 *              p_item-><member1> = <value1>;
 *              p_item-><member2> = <value2>;
 *              p_item-><member3> = <value3>;
 *              //...
 *          }
 *
 *      Were <member1> through <member3> are members of the QUEUE_ITEM 
 *      structure and <value1> through <value3> are data values appropriate
 *      to that member.
 *
 * IMPORTANT:
 *
 *      The struct queue operations are not atomic and they must be used 
 *      in combination to be effective.  The caller is responsible for 
 *      guarding sequences of operations to ensure that they will execute
 *      atomically, meaning that they will not be interrupted mid-sequence
 *      by code that will also modify the queue.  For example, if the queue
 *      is used to synchronize data between a main thread of execution and
 *      an Interrupt Service Routine (ISR).
 *
 *      Example Thread Sequence:
 *
 *          QUEUE_ITEM *p_item;
 *
 *          // Ensure ISR cannot execute by masking appropriate interrupt.
 *
 *          // Remove an item 
 *          if (StructQueueIsNotFull(&my_queue1, SIZE_OF_MY_QUEUE))
 *          {
 *              p_item = StructQueueAdd(&my_queue1, SIZE_OF_MY_QUEUE);
 *              p_item-><member1> = <value1>;
 *              p_item-><member2> = <value2>;
 *              p_item-><member3> = <value3>;
 *              //...
 *          }
 *
 *          // Unmask the interrupt, allowing the ISR to execute when ready.
 *
 *      Example ISR Sequence:
 *
 *          QUEUE_ITEM *p_item;
 *
 *          // Add an item 
 *          if (StructQueueIsNotFull(&my_queue1, SIZE_OF_MY_QUEUE))
 *          {
 *              p_item = StructQueueAdd(&my_queue1, SIZE_OF_MY_QUEUE);
 *              p_item-><member1> = <value1>;
 *              p_item-><member2> = <value2>;
 *              p_item-><member3> = <value3>;
 *              //...
 *          }
 *
 *      The ISR seqeunce generally does not need to be guarded since it 
 *      cannot execute during the guarded sequence if its interrupt has
 *      been disabled.
 *
 * Notes:
 *
 *      It is an error to enqueue an item into a full queue and it will 
 *      result in an access violation.  before performing a StructQueueAdd
 *      operation, the caller must first use one or more of the other 
 *      operations to determine if the queue has room.
 *
 *      It is an error to dequeue an item from an empty queue and it will
 *      result in an access violation.  Before performing a 
 *      StructQueueRemove operation, the caller must first perform one or 
 *      more of the other primatives to ensure that the queue has an item
 *      to remove.
 *
 *      A particular type of queue may only contain one type of item 
 *      unless a union type is used for the queue's buffer array.
 *
 *      The struct-queue primative operations are defined as typless 
 *      macros.  Thus, they will behave the same for any type of buffer 
 *      array as long as the queue structure has the same member names.
 *
 * Change History:
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Bud Caldwell         9/26/2007    Original Implementation.
 *************************************************************************/

#ifndef STRUCT_QUEUE_H
#define STRUCT_QUEUE_H


/* StructQueueInit
 *************************************************************************
 * Precondition:    None
 *
 * Input:           q   Pointer to the queue data structure
 *
 *                  N   Number of elements in the queue data buffer array
 *
 * Output:          None
 *
 * Returns:         zero (0)
 *
 * Side Effects:    The queue structure has been initialized and is ready 
 *                  to use.
 *
 * Overview:        This operation initializes a queue and makes it empty.
 *
 * Note:            This operation is implemented with a macro that 
 *                  supports queues of any type of data items.
 *************************************************************************/
 
#define StructQueueInit(q,N) (  (q)->head  = (N), \
                                (q)->tail  = (N), \
                                (q)->count =  0   )


/* StructQueueAdd
 *************************************************************************
 * Precondition:    The queue must have been initialized and must not 
 *                  currently be full.
 *
 * Input:           q   Pointer to the queue data structure
 *
 *                  N   Number of elements in the queue data buffer array
 *
 * Output:          None
 *
 * Returns:         The address of the new item in the queue.
 *
 * Side Effects:    The item has been added to the queue.
 *
 *                  IMPORTANT!  No data has been copied to the item.
 *
 * Overview:        This operation adds (enqueues) a new item into the 
 *                  queue data buffer and updates the head index,
 *                  handling buffer wrap correctly.
 *
 * Notes:           The caller must first ensure that the queue is not 
 *                  full by performing one of the other operations (such 
 *                  as "StructQueueIsNotFull") before performing this 
 *                  operation.  Adding an item into a full queue will 
 *                  cause an access violation.
 *
 *                  This operation is implemented with a macro that 
 *                  supports queues of any type of data items.
 *************************************************************************/

#define StructQueueAdd(q,N) ( (q)->count++,            \
                              ( ((q)->head < (N-1)) ?  \
                                  ((q)->head++)     :  \
                                  ((q)->head=0)     ), \
                              &(q)->buffer[(q)->head]  )


/* StructQueueRemove
 *************************************************************************
 * Precondition:    The queue must have been initialized and not currently
 *                  be empty.
 *
 * Input:           q   Pointer to the queue data structure
 *
 *                  N   Number of elements in the queue data buffer array
 *
 * Output:          None
 *
 * Returns:         The item removed.
 *
 * Side Effects:    The item has been removed from the queue.
 *
 *                  IMPORTANT!  No data has been copied from the item.
 *
 * Overview:        This routine removes (dequeues) an item from the 
 *                  queue data buffer and updates the tail index,
 *                  handling buffer wrap correctly.
 *
 * Notes:           The caller must first ensure that the queue is not
 *                  empty by calling one or more of the other operations
 *                  (such as "StructQueueIsNotEmpty") before performing this
 *                  operation.  Dequeueing an item from an empty queue
 *                  will cause an access violation.
 *
 *                  This operation is implemented with a macro that 
 *                  supports queues of any type of data items.
 *************************************************************************/

#define StructQueueRemove(q,N) ( (q)->count--,            \
                                 ( ((q)->tail < (N-1)) ?  \
                                     ((q)->tail++)     :  \
                                     ((q)->tail=0)     ), \
                                 &(q)->buffer[(q)->tail]  )


/* StructQueuePeekTail
 *************************************************************************
 * Precondition:    The queue must have been initialized and not currently
 *                  be empty.
 *
 * Input:           q   Pointer to the queue data structure
 *
 *                  N   Number of elements in the queue data buffer array
 *
 * Output:          None
 *
 * Returns:         The item at the tail of the queue.
 *
 * Side Effects:    None
 *
 *                  IMPORTANT!  No data has been copied from the item.
 *
 * Overview:        This routine provides access to an item in the 
 *                  queue data buffer at the tail index position,
 *                  handling buffer wrap correctly.
 *
 * Notes:           The caller must first ensure that the queue is not
 *                  empty by calling one or more of the other operations
 *                  (such as "StructQueueIsNotEmpty") before performing this
 *                  operation.
 *
 *                  This operation is implemented with a macro that 
 *                  supports queues of any type of data items.
 *************************************************************************/

#define StructQueuePeekTail(q,N) ( ((q)->tail < (N-1))         ?  \
                                     &(q)->buffer[(q)->tail+1] :  \
                                     &(q)->buffer[0]              )


/* StructQueueIsFull
 *************************************************************************
 * Precondition:    The queue must be initialized.
 *
 * Input:           q   Pointer to the queue data structure
 *
 *                  N   Number of elements in the queue data buffer array
 *
 * Output:          None
 *
 * Returns:         TRUE if the queue is full, FALSE otherwise.
 *
 * Side Effects:    None
 *
 * Overview:        This routine checks to see if the queue is full.
 *
 * Note:            This operation is implemented with a macro that 
 *                  supports queues of any type of data items.
 *************************************************************************/

#define StructQueueIsFull(q,N) ( (q)->count >= N )


/* StructQueueIsNotFull
 *************************************************************************
 * Precondition:    The queue must be initialized.
 *
 * Input:           q   Pointer to the queue data structure
 *
 *                  N   Number of elements in the queue data buffer array
 *
 * Output:          None
 *
 * Returns:         FALSE if the queue is full, TRUE otherwise.
 *
 * Side Effects:    None
 *
 * Overview:        This routine checks to see if the queue is full.
 *
 * Note:            This operation is implemented with a macro that 
 *                  supports queues of any type of data items.
 *************************************************************************/

#define StructQueueIsNotFull(q,N) ( (q)->count < N )


/* StructQueueIsEmpty
 *************************************************************************
 * Precondition:    The queue must be initialized.
 *
 * Input:           q   Pointer to the queue data structure
 *
 *                  N   Number of elements in the queue data buffer array
 *
 * Output:          None
 *
 * Returns:         TRUE if the queue is empty, FALSE otherwise.
 *
 * Side Effects:    None
 *
 * Overview:        This routine checks to see if the queue is empty.
 *
 * Note:            This operation is implemented with a macro that 
 *                  supports queues of any type of data items.
 *************************************************************************/

#define StructQueueIsEmpty(q,N) ( (q)->count == 0 )


/* StructQueueIsNotEmpty
 *************************************************************************
 * Precondition:    The queue must be initialized.
 *
 * Input:           q   Pointer to the queue data structure
 *
 *                  N   Number of elements in the queue data buffer array
 *
 * Output:          None
 *
 * Returns:         FALSE if the queue is empty, TRUE otherwise.
 *
 * Side Effects:    None
 *
 * Overview:        This routine checks to see if the queue is not empty.
 *
 * Note:            This operation is implemented with a macro that 
 *                  supports queues of any type of data items.
 *************************************************************************/

#define StructQueueIsNotEmpty(q,N) ( (q)->count != 0 )


/* StructQueueSpaceAvailable
 *************************************************************************
 * Precondition:    The queue must be initialized.
 *
 * Input:           q   Pointer to the queue data structure
 *
 *                  N   Number of elements in the queue data buffer array
 *
 * Output:          None
 *
 * Returns:         TRUE if the queue has the indicated number of slots
                    available, FALSE otherwise.
 *
 * Side Effects:    None
 *
 * Overview:        This routine checks to see if the queue has at least
 *                  the specified number of slots free.
 *
 * Note:            This operation is implemented with a macro that 
 *                  supports queues of any type of data items.
 *************************************************************************/

#define StructQueueSpaceAvailable(c,q,N) ( ((q)->count + c) <= N )


/* StructQueueCount
 *************************************************************************
 * Precondition:    The queue must be initialized.
 *
 * Input:           q   Pointer to the queue data structure
 *
 *                  N   Number of elements in the queue data buffer array
 *
 * Output:          None
 *
 * Returns:         The number of items in the queue.
 *
 * Side Effects:    None
 *
 * Overview:        This routine provides the number of items in the queue.
 *
 * Note:            This operation is implemented with a macro that 
 *                  supports queues of any type of data items.
 *************************************************************************/

#define StructQueueCount(q,N) ( (q)->count )


#endif // STRUCT_QUEUE_H
/*************************************************************************
 * EOF struct_queue.c
 */

