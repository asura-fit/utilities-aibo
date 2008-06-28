//
// Copyright 2002 Sony Corporation 
//
// Permission to use, copy, modify, and redistribute this software for
// non-commercial use is hereby granted.
//
// This software is provided "as is" without warranty of any kind,
// either expressed or implied, including but not limited to the
// implied warranties of fitness for a particular purpose.
//

#ifndef DigiCame_h
#define DigiCame_h

#include <OPENR/OPENR.h>

enum DigiCameCommandType {
    DCCMD_LED_OFF,
    DCCMD_LED_NORMAL_EYE,
    DCCMD_LED_MODE_LAMP
};
const int DigiCameCommandTypeMAX=3;

// Robot design
enum RobotDesignType {
    RDT_UNDEF,
    RDT_ERS210,
    RDT_ERS220, // never used
    RDT_ERS7
};
const char* const ROBOT_DESIGN_NAME_TABLE[4] = {
    "UNKNOWN", "ERS-210", "ERS-220", "ERS-7"
};

#endif // DigiCame_h

