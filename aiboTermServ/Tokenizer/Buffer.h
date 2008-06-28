/*
  $Log: Buffer.h,v $
  Revision 1.2  2002/11/28 17:56:34  isshi
  Merge sdk-firt-release to HEAD

  Revision 1.1.2.1  2002/10/18 07:23:28  ken
  AiboTermServ introduced

  Revision 1.3  2002/03/20 20:34:18  ken

  Append Log macro

*/
#ifndef _MY_BUFFER_
#define _MY_BUFFER_


#define DEFSIZE  4096
#define GROWSIZE 4096

class Buffer {
 private:
  void*  mBuffer;
  long   mBufferSize;
  long   mContentSize;
  void*  my_malloc(long size);
  void   my_free(void* ptr);
  void   AllocBuffer(long size);
 public:
  Buffer();
  virtual ~Buffer();
  virtual void write(int ch);
  virtual void writeBytes(void* data, int length);
  virtual void reset();
  // returns raw buffer ptr, so it is invalidated after write* methods.
  // user has to copy for reuse.
  virtual void* getBuffer();
  virtual long  getSize();
};

#endif

