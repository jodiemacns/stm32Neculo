// -----------------------------------------------------------------------------

/**
 * @file buffer.c
 * @brief Handles the buffer operations.
*/

// -----------------------------------------------------------------------------
#define BUFFER_C
//#include <string.h>
//#include <stdio.h>
#include "buffer.h"

/**
 * @brief initBuffer will initialize the buffer data.
 * The size needs to be to the power of 2 for wrapping.
 *
 * @param pBuffer - Pointer to the buffer descriptor.
 *
 */
int initBuffer(T_BUFFER *pBuf)
{
   pBuf->read = pBuf->write = 0;
   pBuf->nbytes = 0;
   return(0);
}

/**
 * @brief writeBuffer will write to the buffer the data passed.
 *
 * @param pBuffer - Pointer to the buffer descriptor.
 * @param pData   - Pointer to the bytes to be writing.
 * @param length  - Length of the data to write.
 *
 * @return This will return the number of bytes written.
 *         This may not write all bytes if the buffer is does not
 *         have room.
 *
 */
int writeBuffer(T_BUFFER *pBuffer, unsigned char *pData, int length)
{
   int count;

   for(count = 0; count < length; count++)
   {
      if(pBuffer->nbytes <= pBuffer->size)
      {
         b_write(pBuffer, pData[count]);
      } else {
         return(count);
      }
   }

   return(count);
}

/**
 * @brief readBuffer will read from the buffer.
 *        This will read the bytes up to the length
 *        or the number of bytes available.
 *
 * @param pBuffer - Pointer to the buffer descriptor.
 * @param pData   - Pointer to the bytes to be writing.
 * @param length  - Max length of the data to read.
 *
 */
int readBuffer(T_BUFFER *pBuffer, unsigned char *pData, int length)
{
   int count = 0;

   for(count = 0; count < length; count++)
   {
      if(pBuffer->nbytes > 0)
      {
         // Read
         pData[count] = pBuffer->pBuf[pBuffer->read];

         // increment
         pBuffer->read++;
         pBuffer->read &= pBuffer->size;
         pBuffer->nbytes--;
      } else {
         return(count);
      }
   }
   return(count);
}

void b_write(T_BUFFER *pBuffer, unsigned char data)
{
   if(!( ((pBuffer->write + 1) & pBuffer->size) == (pBuffer->read & pBuffer->size)))
   {
      pBuffer->pBuf[pBuffer->write] = data;
      /* Increment */
      pBuffer->write++;
      pBuffer->write &= pBuffer->size;
      pBuffer->nbytes++;
      pBuffer->nbytes &= pBuffer->size;
   }
   return;
}
