//
// Copyright 2002,2003 Sony Corporation
//
// Permission to use, copy, modify, and redistribute this software is
// hereby granted.
//
// This software is provided "as is" without warranty of any kind,
// either expressed or implied, including but not limited to the
// implied warranties of fitness for a particular purpose.
//

#ifndef _RoboCupGameController_h_DEFINED
#define _RoboCupGameController_h_DEFINED

#include <OPENR/OObject.h>
#include <OPENR/OSubject.h>
#include <OPENR/OObserver.h>
#include "def.h"
#include "RoboCupGameControlData.h"


class RoboCupGameController : public OObject {
  public:
    // OPEN-R Inter-Object Communication
    OSubject*  subject[numOfSubject];
    OObserver* observer[numOfObserver];

  public:
    // public methods
    RoboCupGameController(void);
    ~RoboCupGameController();

    // OPEN-R Object Common Interface
    virtual OStatus DoInit   (const OSystemEvent&);
    virtual OStatus DoStart  (const OSystemEvent&);
    virtual OStatus DoStop   (const OSystemEvent&);
    virtual OStatus DoDestroy(const OSystemEvent&);

    void SendControlData(const RoboCupGameControlData& data, int sbjIndex);

  private:
    // will never be implemented
    RoboCupGameController(const RoboCupGameController&);
    RoboCupGameController& operator=(const RoboCupGameController&);
};


#endif // _RoboCupGameController_h_DEFINED
