/**
 * @file buffer.h
 * @brief Handles the buffer operations.
*/

// -----------------------------------------------------------------------------
#ifndef BUFFER_H
#define BUFFER_H

//******************************************************************************
// Macros
#define BUFFER_REALLY_SMALL   0x03
#define BUFFER_SMALL          0x0f
#define BUFFER_MEDIUM         0x1f
#define BUFFER_LARGE          0x3f

//******************************************************************************
// Typedefs and structures

/**
 * @brief T_BUFFER holds the information of the buffer.
 */
typedef struct
{
   int read;              ///< read will hold the read index buffer.
   int write;             ///< write holds the write index.
   int size;              ///< The size of the buffer.
   int nbytes;            ///< The number of bytes in the buffer.
   unsigned char *pBuf;   ///< Pointer to the buffer.
}T_BUFFER;

void b_write(T_BUFFER *pBuffer, unsigned char data);

/**
 * @brief Macro for writing a single character to the buffer.
 */
#define buff_write_char(pBuffer, data)                                               \
   if(!( ((pBuffer->write + 1) & pBuffer->size) == (pBuffer->read & pBuffer->size))) \
   {                                                                                 \
      pBuffer->pBuf[pBuffer->write] = data;                                          \
      /* Increment */                                                                \
      pBuffer->write++;                                                              \
      pBuffer->write &= pBuffer->size;                                               \
      pBuffer->nbytes++;                                                             \
      pBuffer->nbytes &= pBuffer->size;                                              \
   }

//******************************************************************************
// Functions
int initBuffer(T_BUFFER *pBuf);
int writeBuffer(T_BUFFER *pBuffer, unsigned char *pData, int length);
int readBuffer(T_BUFFER *pBuffer, unsigned char *pData, int length);

#endif
