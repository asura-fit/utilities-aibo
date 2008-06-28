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

#include <OPENR/core_macro.h>
#include <iostream>
#include "SamplePlayer.h"


SamplePlayer::SamplePlayer()
{
}

OStatus
SamplePlayer::DoInit(const OSystemEvent&)
{
    NEW_ALL_SUBJECT_AND_OBSERVER;
    REGISTER_ALL_ENTRY;
    SET_ALL_READY_AND_NOTIFY_ENTRY;
    return oSUCCESS;
}

OStatus
SamplePlayer::DoStart(const OSystemEvent&)
{
    ENABLE_ALL_SUBJECT;
    ASSERT_READY_TO_ALL_OBSERVER;
    return oSUCCESS;
}

OStatus
SamplePlayer::DoStop(const OSystemEvent&)
{
    DISABLE_ALL_SUBJECT;
    DEASSERT_READY_TO_ALL_OBSERVER;
    return oSUCCESS;
}

OStatus
SamplePlayer::DoDestroy(const OSystemEvent&)
{
    DELETE_ALL_SUBJECT_AND_OBSERVER;
    return oSUCCESS;
}

void
SamplePlayer::UpdateGameControl(const ONotifyEvent& event)
{
  cout << "Update" << endl;
    data_ = *static_cast<const RoboCupGameControlData*>(event.Data(0));

    cout << "State   : " << static_cast<int>(data_.state) << endl
         << "Team    : " << static_cast<int>(data_.teamColor) << endl
         << "Kickoff : " << static_cast<int>(data_.kickoff) << endl
         << "Score   : " << data_.ownScore
                << " - " << data_.opponentScore << endl
         << "Penalty : " << static_cast<int>(data_.penalty) << endl
         << endl;

    observer[event.ObsIndex()]->AssertReady(event.SenderID());
}
