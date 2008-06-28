//
// Tokenizer.h
//
/*
  $Log: Tokenizer.h,v $
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


#ifndef _Tokenizer_h_DEFINED
#define _Tokenizer_h_DEFINED

#include <OPENR/OObject.h>
#ifndef Apertos
#include <OPENR/OSubject.h>
#include <OPENR/OObserver.h>
#else // Apertos
#include <OPENR/OSubject.h>
#include <OPENR/OObserver.h>
#endif // Apertos
#include "def.h"

#include "Buffer.h"

typedef enum CHARTYPE { NIL, NORMAL, PAROPEN, PARCLOSE, RETURN, COMMENT } CHARTYPE;

class Tokenizer : public OObject {
  public:
    // OPEN-R Inter-Object Communication
    OSubject*  subject[numOfSubject];
    OObserver* observer[numOfObserver];
    Buffer     mBuff;
  public:
    // public methods
    Tokenizer(void);
    ~Tokenizer();

    // OPEN-R Object Common Interface
    virtual OStatus DoInit   (const OSystemEvent&);
    virtual OStatus DoStart  (const OSystemEvent&);
    virtual OStatus DoStop   (const OSystemEvent&);
    virtual OStatus DoDestroy(const OSystemEvent&);

    void Update(const ONotifyEvent&);
    void Ready(const OReadyEvent&);
  private:
    // private member
    bool active_;
    
    // setdata available count
    int  mNotifyAvailableCount;

    CHARTYPE mPrevType;
    bool mIsInComment;
    int  mParCount;

    char* mBuffer;
    int   mBufferSize;
    int   mContentSize;
    CHARTYPE getType(char ch);
    void eatchar(char ch);
    void writechar(char ch);
    void flush();
    void reset();
    void delchar();
  private:
    // will never be implemented
    Tokenizer(const Tokenizer&);
    Tokenizer& operator=(const Tokenizer&);
};


#endif // _Tokenizer_h_DEFINED
