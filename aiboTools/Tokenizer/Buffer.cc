/*
  $Log: Buffer.cc,v $
  Revision 1.2  2003/06/09 09:48:07  ken
  add: AiboTools, the integration of AiboProxy and AiboTermServ

  Revision 1.1.2.1  2003/05/21 21:34:49  ken
  add: integrate AiboProxy and AiboTermServ

  Revision 1.2  2002/11/28 17:56:34  isshi
  Merge sdk-firt-release to HEAD

  Revision 1.1.2.1  2002/10/18 07:23:28  ken
  AiboTermServ introduced

  Revision 1.3  2002/03/20 20:34:18  ken

  Append Log macro

*/

#include <MCOOP.h>
#include <OPENR/OPENRAPI.h>
#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "Utility.h"
#include "Buffer.h"
#define ASSERT(v) { \
  if (!(v)) \
    while (1)  { \
      cerr << "ASSERTION FAILED!!" << endl; \
      cerr << "LINE=" << __LINE__ << " FILE=" << __FILE__ << endl; \
    }\
}


#define NEWREGION  0

Buffer::Buffer()
{
  mBufferSize = DEFSIZE;
  mBuffer = my_malloc(mBufferSize);
  mContentSize = 0;
}

Buffer::~Buffer()
{
  my_free(mBuffer);
}

void
Buffer::my_free(void* ptr)
{
#if NEWREGION
  ASSERT(::DeleteRegion(ptr) == oSUCCESS);
#else
  free(mBuffer);
#endif
}


void*
Buffer::my_malloc(long size)
{
  void* buff=NULL;
#if NEWREGION
  ASSERT(::NewRegion(size,&buff) == oSUCCESS);
#else
  buff = malloc(size);
#endif
  return buff;
}

void
Buffer::AllocBuffer(long size)
{
  if (size > mBufferSize) {
    long  newSize;
    void* newBuffer;
    newSize = size + GROWSIZE;
   // Log::info("OLD=%d  NEW=%d",mBufferSize, newSize);

    newBuffer = my_malloc(newSize);
    ASSERT(newBuffer != NULL);
    memcpy(newBuffer, mBuffer, mBufferSize);
    my_free(mBuffer);
    mBuffer = newBuffer;
    mBufferSize = newSize;

  }
}

void
Buffer::write(int ch)
{
  AllocBuffer(mContentSize + 1);
  ((char *)(mBuffer))[mContentSize++] = (char)ch;
}

void
Buffer::writeBytes(void* data, int length)
{
  AllocBuffer(mContentSize + length);
  memcpy((char *)mBuffer + mContentSize, data, length);
  mContentSize += length;
}

void*
Buffer::getBuffer()
{
  return mBuffer;
}

long
Buffer::getSize()
{
  return mContentSize;
}

void
Buffer::reset()
{
  mContentSize = 0;
}

#if 0 
int
main(int argc,char* argv[])
{
  Buffer b;

  for(int i=0;i<1000;i++) {
    for(int ch='0';ch<='9';ch++)
      b.write(ch);
  }
  b.write(0);

}
#endif
