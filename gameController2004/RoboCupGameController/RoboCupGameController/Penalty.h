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

#ifndef _Penalty_h_DEFINED
#define _Penalty_h_DEFINED


#include <RoboCupGameControlData.h>


typedef struct {
    char* name;		// name of penalty
    char* shortName;	// short name of penalty
    int maxCount;	// 0 is uncount, other is maximum number of count
    int waitingTime;	// waiting time of penalty [sec]
} PenaltyEntry;

extern const PenaltyEntry penaltyEntry[];


#endif // _Penalty_h_DEFINED
