//
// StdoutHub.cc
//
// $Log: StdoutHub.cc,v $
// Revision 1.2  2002/11/28 17:56:34  isshi
// Merge sdk-firt-release to HEAD
//
// Revision 1.1.2.1  2002/10/18 07:23:28  ken
// AiboTermServ introduced
//
// Revision 1.4  2002/05/20 10:16:42  ken
// bug fix crash
//
// Revision 1.3  2002/03/20 20:35:30  ken
//  Append Log macro
//
//

#ifndef Apertos
#include <OPENR/core_macro.h>
#else // Apertos
#include <OPENR/core_macro.h>
#endif // Apertos

#include "StdoutHub.h"

#include <unistd.h>
#include <fcntl.h>

#ifdef _OPENR_DEBUG
#include <iostream.h>
#endif // _OPENR_DEBUG


StdoutHub::StdoutHub(void) : active_(false) {
  mFD = 0;
}

StdoutHub::~StdoutHub() {}


OStatus
StdoutHub::DoInit(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "StdoutHub::DoInit()" << endl;
#endif // _OPENR_DEBUG

    NewComData;
    SetComData;


    return oSUCCESS;
}


OStatus
StdoutHub::DoStart(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "StdoutHub::DoStart()" << endl;
#endif // _OPENR_DEBUG

    active_ = true;
    ASSERT_TO_ALL_OBSERVER;

    return oSUCCESS;
}


OStatus
StdoutHub::DoStop(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "StdoutHub::DoStop()" << endl;
#endif // _OPENR_DEBUG

    DEASSERT_TO_ALL_OBSERVER;
    active_ = false;

    return oSUCCESS;
}


OStatus
StdoutHub::DoDestroy(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "StdoutHub::DoDestroy()" << endl;
#endif // _OPENR_DEBUG

    DeleteComData;

    return oSUCCESS;
}


void
StdoutHub::Ready(const OReadyEvent& event)
{
#ifdef _OPENR_DEBUG
    cout << "StdoutHub::Ready() ";

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
    } else {
	; // better to ignore
    }

    return;
}


void
StdoutHub::Update(const ONotifyEvent& event)
{
#ifdef _OPENR_DEBUG
    cout << "StdoutHub::Update() received "
	<< event.NumOfData() << " data."<< endl;
#endif

    if (active_) {
      // print all data
      for(int i=0;i<event.NumOfData();i++) {
	RCRegion* region = event.RCData(i);

	char* data = (char *)region->Base();
	long sizeofdata = region->Size();

#ifdef _OPENR_DEBUG
	cout << "length=" << sizeofdata << endl;
#endif
	// print contents
	for(int j=0;j<sizeofdata;j++)
	  cout << *(data + j);
	cout << flush;
      }
      observer[event.ObsIndex()]->AssertReady(event.SenderID());

    } 
      

    return;
}

void
StdoutHub::UpdateToFile(const ONotifyEvent& event)
{
#ifdef _OPENR_DEBUG
    cout << "StdoutHub::Update() received "
	<< event.NumOfData() << " data."<< endl;
#endif

    if (active_) {
      // print all data
      for(int i=0;i<event.NumOfData();i++) {
	RCRegion* region = event.RCData(i);

	char* data = (char *)region->Base();
	long sizeofdata = region->Size();

#ifdef _OPENR_DEBUG
	cout << "length=" << sizeofdata << endl;
#endif
	if (mFD <= 0) {
	  mFD = open("stdOut.pipe", O_WRONLY);
	  if (mFD < 0) {
	    cout << "error open pipe @ Stdout" << endl;
	  }
	}
#ifdef _OPENR_DEBUG
	write(0, "WR:", 3); 
	write(0, data, sizeofdata);
#endif
	write(mFD, data, sizeofdata);
      }
      observer[event.ObsIndex()]->AssertReady(event.SenderID());
    }

    return;
}
