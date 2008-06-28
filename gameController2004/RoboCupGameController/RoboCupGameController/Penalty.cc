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

#include "Penalty.h"

const PenaltyEntry penaltyEntry[] = {
    { "No penalty",             "none", 0,      0 },
    { "Ball holding (30)",	"BHld",	0,	30 },
    { "Goalie pushing (30)",	"GPus",	0,	30 },
    { "Field Player pushing (30)",	"FPus",	0,	30 },
    { "Illegal defender (30)",	"IDef",	0,	30 },
    { "Illegal defense (0)",    "IDfs", 0,      0 },
    { "Obstruction (30)",	"Obst",	0,	30  },
    { "Req.for Pick-up (30)",	"PkUp",	0,	30 },
    { 0, 0 }
};
