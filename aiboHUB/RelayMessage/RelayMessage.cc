//
// RelayMessage.cc
//
// Copyright (C) 2002 Sony Corporation
// All Rights Reserved.
//

#ifndef Apertos
#include <OPENR/core_macro.h>
#else // Apertos
#include <OPENR/core_macro.h>
#endif // Apertos

#include "RelayMessage.h"

#ifdef _OPENR_DEBUG
#include <iostream.h>
#endif // _OPENR_DEBUG


RelayMessage::RelayMessage(void) : active_(false) {}

RelayMessage::~RelayMessage() {}


OStatus
RelayMessage::DoInit(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "RelayMessage::DoInit()" << endl;
#endif // _OPENR_DEBUG

    NEW_ALL_SUBJECT_AND_OBSERVER;
    REGISTER_ALL_ENTRY;
    SET_ALL_READY_AND_NOTIFY_ENTRY;

    return oSUCCESS;
}


OStatus
RelayMessage::DoStart(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "RelayMessage::DoStart()" << endl;
#endif // _OPENR_DEBUG

    active_ = true;
    ASSERT_TO_ALL_OBSERVER;

    return oSUCCESS;
}


OStatus
RelayMessage::DoStop(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "RelayMessage::DoStop()" << endl;
#endif // _OPENR_DEBUG

    DEASSERT_TO_ALL_OBSERVER;
    active_ = false;

    return oSUCCESS;
}


OStatus
RelayMessage::DoDestroy(const OSystemEvent&)
{
#ifdef _OPENR_DEBUG
    cout << "RelayMessage::DoDestroy()" << endl;
#endif // _OPENR_DEBUG

    DELETE_ALL_SUBJECT_AND_OBSERVER;

    return oSUCCESS;
}


void
RelayMessage::Ready(const OReadyEvent& event)
{
#ifdef _OPENR_DEBUG
    cout << "RelayMessage::Ready() ";

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
RelayMessage::Update(const ONotifyEvent& event)
{
#ifdef _OPENR_DEBUG
    cout << "RelayMessage::Update() received "
	<< event.NumOfData() << " data."<< endl;
#endif

    if (active_) {
	// do something if necessary
	cout << "Update: receiving " << event.NumOfData() << " datas" << endl;
	for(int i = 0; i < event.NumOfData(); i++) {
		RCRegion* region = event.RCData(i);
		subject[event.ObsIndex()]->SetData(region);
	}
	subject[event.ObsIndex()]->NotifyObservers();
	observer[event.ObsIndex()]->AssertReady(event.SenderID());
    }

    return;
}
