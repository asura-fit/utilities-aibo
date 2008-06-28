//
// StdinHub.cc
//
// Copyright (C) 2001 Sony Corporation
// All Rights Reserved.
//

#ifndef Apertos
#include <OPENR/core_macro.h>
#else // Apertos
#include <OPENR/core_macro.h>
#endif // Apertos

#include "StdinHub.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef _OPENR_DEBUG
#include <iostream.h>
#endif // _OPENR_DEBUG


StdinHub::StdinHub(void) : active_(false) {
  mFD = 0;
}

StdinHub::~StdinHub() {}


OStatus
StdinHub::DoInit(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "StdinHub::DoInit()" << endl;
#endif // _OPENR_DEBUG

    NewComData;
    SetComData;

    return oSUCCESS;
}


OStatus
StdinHub::DoStart(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "StdinHub::DoStart()" << endl;
#endif // _OPENR_DEBUG

    active_ = true;
    ASSERT_TO_ALL_OBSERVER;

    return oSUCCESS;
}


OStatus
StdinHub::DoStop(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "StdinHub::DoStop()" << endl;
#endif // _OPENR_DEBUG

    DEASSERT_TO_ALL_OBSERVER;
    active_ = false;

    return oSUCCESS;
}


OStatus
StdinHub::DoDestroy(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "StdinHub::DoDestroy()" << endl;
#endif // _OPENR_DEBUG

    DeleteComData;

    return oSUCCESS;
}


#define BUFFERSIZE (1024*10) // 10kbytes

void
StdinHub::Ready(const OReadyEvent& event)
{
#ifdef _OPENR_DEBUG
    cout << "StdinHub::Ready() ";

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
        // read a line from stdin
	char buffer[BUFFERSIZE];
	fgets(buffer, BUFFERSIZE, stdin);

	//	cout << "<" << buffer << ">" << endl;
	// set data
	// this includes "\0"
	// subject[sbjStdout]->SetData(buffer, strlen(buffer)+1);
	// Intentionally, I ommit "\0" due to clean line.
	subject[sbjStdout]->SetData(buffer, strlen(buffer));

	// kick data
	subject[sbjStdout]->NotifyObservers();
    } else {
      ; // better to ignore
    }

    return;
}


void
StdinHub::ReadyReadFromFile(const OReadyEvent& event)
{
#ifdef _OPENR_DEBUG
    cout << "StdinHub::ReadyReadFromFile() ";

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
        // read a line from stdin

      if (event.IsAssert()) {
	char buffer[BUFFERSIZE];
	int  bytesRead;
	if (mFD <= 0) {
	  mFD = open("stdIn.pipe",O_RDONLY);
	
	  if (mFD < 0) {
	    cout << "Error open stdIn.pipe" << endl;
	    system("mknod stdIn.pipe p ; sleep 1;");
	  }
	}
	if (mFD > 0) {
	  bytesRead = read(mFD, buffer, BUFFERSIZE);
	  if (bytesRead == 0) {
	    cout << "fread() returns 0 reopen the pipe" << endl;
	    close(mFD);
	    mFD = open("stdIn.pipe", O_RDONLY);
	  }
#ifdef _OPENR_DEBUG
	  write(0,"RD:", 3);
	  write(0, buffer, bytesRead);
	  printf("SIZE=%4d\n", bytesRead);
#endif // _OPENR_DEBUG

	  // cout << "<" << buffer << ">" << endl;
	  // set data
	  // this includes "\0"
	  // subject[sbjStdout]->SetData(buffer, strlen(buffer)+1);
	  // Intentionally, I ommit "\0" due to let the line clean.
	  subject[sbjStdoutFile]->SetData(buffer, bytesRead);
	  // kick data
	  subject[sbjStdoutFile]->NotifyObservers();
	}
      }

    }
    return;
}

void
StdinHub::Update(const ONotifyEvent& event)
{
#ifdef _OPENR_DEBUG
    cout << "StdinHub::Update() received "
	<< event.NumOfData() << " data."<< endl;
#endif

    if (active_) {
	// do something if necessary
      observer[event.ObsIndex()]->AssertReady();
    }

    return;
}



