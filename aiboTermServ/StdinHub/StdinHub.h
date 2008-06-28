//
// StdinHub.h
//
// Copyright (C) 2001 Sony Corporation
// All Rights Reserved.
//

#ifndef _StdinHub_h_DEFINED
#define _StdinHub_h_DEFINED

#include <OPENR/OObject.h>
#ifndef Apertos
#include <OPENR/OSubject.h>
#include <OPENR/OObserver.h>
#else // Apertos
#include <OPENR/OSubject.h>
#include <OPENR/OObserver.h>
#endif // Apertos
#include "def.h"


class StdinHub : public OObject {
  public:
    // OPEN-R Inter-Object Communication
    OSubject*  subject[numOfSubject];
    OObserver* observer[numOfObserver];
    int        mFD;
  public:
    // public methods
    StdinHub(void);
    ~StdinHub();

    // OPEN-R Object Common Interface
    virtual OStatus DoInit   (const OSystemEvent&);
    virtual OStatus DoStart  (const OSystemEvent&);
    virtual OStatus DoStop   (const OSystemEvent&);
    virtual OStatus DoDestroy(const OSystemEvent&);

    void Update(const ONotifyEvent&);
    void Ready(const OReadyEvent&);
    void ReadyReadFromFile(const OReadyEvent&);

  private:
    // private member
    bool active_;

  private:
    // will never be implemented
    StdinHub(const StdinHub&);
    StdinHub& operator=(const StdinHub&);
};


#endif // _StdinHub_h_DEFINED
