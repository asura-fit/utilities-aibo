//
// RelayMessage.h
//
// Copyright (C) 2002 Sony Corporation
// All Rights Reserved.
//

#ifndef _RelayMessage_h_DEFINED
#define _RelayMessage_h_DEFINED

#include <OPENR/OObject.h>
#ifndef Apertos
#include <OPENR/OSubject.h>
#include <OPENR/OObserver.h>
#else // Apertos
#include <OPENR/OSubject.h>
#include <OPENR/OObserver.h>
#endif // Apertos
#include "def.h"


class RelayMessage : public OObject {
  public:
    // OPEN-R Inter-Object Communication
    OSubject*  subject[numOfSubject];
    OObserver* observer[numOfObserver];

  public:
    // public methods
    RelayMessage(void);
    ~RelayMessage();

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

  private:
    // will never be implemented
    RelayMessage(const RelayMessage&);
    RelayMessage& operator=(const RelayMessage&);
};


#endif // _RelayMessage_h_DEFINED
