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
//
// Modified at University Bremen
// for RoboCup 2004 - Martin Fritsche, March 2004

#include <OPENR/core_macro.h>
#include "RoboCupGameController.h"
#include "GUI.h"
#include "TeamInfo.h"


RoboCupGameController::RoboCupGameController() {}


RoboCupGameController::~RoboCupGameController() {}


OStatus
RoboCupGameController::DoInit(const OSystemEvent&)
{
    NewComData;
    SetComData;

    return oSUCCESS;
}


OStatus
RoboCupGameController::DoStart(const OSystemEvent&)
{
    teamInfo.Load();
    GuiInit();

    return oSUCCESS;
}


OStatus
RoboCupGameController::DoStop(const OSystemEvent&)
{
    return oSUCCESS;
}


OStatus
RoboCupGameController::DoDestroy(const OSystemEvent&)
{
    DeleteComData;

    return oSUCCESS;
}

void
RoboCupGameController::SendControlData(const RoboCupGameControlData& data, int sbjIndex)
{
    RoboCupGameControlData data2 = data;
    subject[sbjIndex]->SetData(&data2, sizeof(data2));
    subject[sbjIndex]->NotifyObservers();
}
