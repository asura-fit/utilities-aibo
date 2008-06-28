
//
// Tokenizer.cc
//
/*
  $Log: Tokenizer.cc,v $
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


#ifndef Apertos
#include <OPENR/core_macro.h>
#else // Apertos
#include <OPENR/core_macro.h>
#endif // Apertos

#include "Tokenizer.h"
#include "Buffer.h"

#ifdef _OPENR_DEBUG
#include <iostream.h>
#endif // _OPENR_DEBUG

#include <stdlib.h>

Tokenizer::Tokenizer(void) : active_(false) {
  mPrevType = RETURN;
  mIsInComment = false;
  mParCount = 0;
  mBufferSize = 1024;
  mBuffer = (char *)malloc(mBufferSize);
  mContentSize = 0;
  mNotifyAvailableCount = 1;
}

Tokenizer::~Tokenizer() {}


OStatus
Tokenizer::DoInit(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "Tokenizer::DoInit()" << endl;
#endif // _OPENR_DEBUG

    NewComData;
    SetComData;

    return oSUCCESS;
}


OStatus
Tokenizer::DoStart(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "Tokenizer::DoStart()" << endl;
#endif // _OPENR_DEBUG

    active_ = true;
    ASSERT_TO_ALL_OBSERVER;

    return oSUCCESS;
}


OStatus
Tokenizer::DoStop(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "Tokenizer::DoStop()" << endl;
#endif // _OPENR_DEBUG

    DEASSERT_TO_ALL_OBSERVER;
    active_ = false;

    return oSUCCESS;
}


OStatus
Tokenizer::DoDestroy(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "Tokenizer::DoDestroy()" << endl;
#endif // _OPENR_DEBUG

    DeleteComData;

    return oSUCCESS;
}


void
Tokenizer::Ready(const OReadyEvent& event)
{
#ifdef _OPENR_DEBUG
    cout << "Tokenizer::Ready() ";

    if (event.IsAssert()) {
	cout << "ASSERT_READY";
    } else if (event.IsDeassert()) {
	cout << "DEASSERT_READY";
    } else {
	cout << "UNKNOWN COMMAND";
    }
    cout << endl;
#endif // _OPENR_DEBUG

    if (active_) {
	; // do something if necessary

	if (mBuff.getSize() > 0) {
	  mBuff.write('\0');
	  subject[sbjStdout]->SetData(mBuff.getBuffer(),mBuff.getSize());
	  subject[sbjStdout]->NotifyObservers();
	  mBuff.reset();
	}
	mNotifyAvailableCount = 1;

    } else {
	; // better to ignore
    }

    return;
}


void
Tokenizer::Update(const ONotifyEvent& event)
{
#ifdef _OPENR_DEBUG
    cout << "Tokenizer::Update() received "
	<< event.NumOfData() << " data."<< endl;
#endif

    if (active_) {
      for(int i=0;i<event.NumOfData();i++) {
	RCRegion* region = event.RCData(i);
	char* data = (char *)region->Base();
	long sizeofdata = region->Size();
	// eat chars for tokenization
	for(int j=0;j<sizeofdata;j++)
	  eatchar(data[j]);
      }

      if (mNotifyAvailableCount > 0 && mBuff.getSize() > 0) {
	mBuff.write('\0');
	subject[sbjStdout]->SetData(mBuff.getBuffer(),mBuff.getSize());
	subject[sbjStdout]->NotifyObservers();
	mBuff.reset();
	mNotifyAvailableCount--;
      }

      observer[event.ObsIndex()]->AssertReady(event.SenderID());
    }

    return;
}

void
Tokenizer::eatchar(char ch)
{
  CHARTYPE type = getType(ch);
  //  printf("ch=%3d \\r=%3d \\n=%3d\n", ch, '\r', '\n');
  if (mIsInComment) {
    // comment mode
    if (type == RETURN || type == NIL)
      mIsInComment = false;
    // do nothing
  } else {
    // not comment mode
    if (type == RETURN || type == NIL) {
      if (mParCount == 0)
	flush();
      else
	writechar('\n'); // there are pending ')'s. wait for it.
    } else {
      if (type == COMMENT) {
	mIsInComment = true;
      } else {
	writechar(ch);
	if (type == PAROPEN)
	  mParCount++;
	if (type == PARCLOSE) {
	  mParCount--;
	  if (mParCount < 0) {
	    cout << "Tokenizer error too much )" << endl;
	    delchar();
	    mParCount = 0;
	  }
	} // for PARCLOSE
      }
    }
  } // not comment mode

  mPrevType = type;
}

CHARTYPE
Tokenizer::getType(char ch)
{
  switch(ch) {
  case '\0':
    return NIL;
  case '(':
    return PAROPEN;
  case ')':
    return PARCLOSE;
  case '\r':
  case '\n':
    return RETURN;
  case ';':
    return COMMENT;
  default:
    return NORMAL;
  }
}

void
Tokenizer::writechar(char ch)
{
  mBuffer[mContentSize++] = ch;
  if (mBufferSize <= mContentSize) {
    mBufferSize = mBufferSize * 2;
    mBuffer = (char *)realloc(mBuffer, mBufferSize);
  }
}

void
Tokenizer::flush()
{

  // terminate an S-expression
  writechar('\n');
  //writechar('\0');
  //  printf("FLUSH->%s<--\n",mBuffer);
  mBuff.writeBytes(mBuffer,mContentSize);
  /*
  subject[sbjStdout]->SetData(mBuffer,mContentSize);
  subject[sbjStdout]->NotifyObservers();
  */
  reset();
}

void
Tokenizer::reset()
{
  // reset
  mContentSize = 0;
  mPrevType = RETURN;
  mParCount = 0;
  mIsInComment = false;
}

void
Tokenizer::delchar()
{
  if (mContentSize > 0)
    mContentSize--;
}
