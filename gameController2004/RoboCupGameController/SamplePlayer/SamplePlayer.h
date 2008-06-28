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

#ifndef SamplePlayer_h_DEFINED
#define SamplePlayer_h_DEFINED

#include <OPENR/OObject.h>
#include <OPENR/OSubject.h>
#include <OPENR/OObserver.h>
#include "def.h"
#include "RoboCupGameControlData.h"

class SamplePlayer : public OObject {
public:
    SamplePlayer(void);
    virtual ~SamplePlayer() {}

    OSubject*  subject[numOfSubject];
    OObserver* observer[numOfObserver];

    virtual OStatus DoInit   (const OSystemEvent&);
    virtual OStatus DoStart  (const OSystemEvent&);
    virtual OStatus DoStop   (const OSystemEvent&);
    virtual OStatus DoDestroy(const OSystemEvent&);

    void UpdateGameControl(const ONotifyEvent&);

private:
    RoboCupGameControlData data_;
};


#endif // SamplePlayer_h_DEFINED
