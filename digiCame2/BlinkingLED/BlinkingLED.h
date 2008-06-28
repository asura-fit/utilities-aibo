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

#ifndef BlinkingLED_h_DEFINED
#define BlinkingLED_h_DEFINED

#include <OPENR/OObject.h>
#include <OPENR/OSubject.h>
#include <OPENR/OObserver.h>
#include <DigiCame.h>
#include "def.h"

enum BlinkingLEDState {
    BLS_IDLE,
    BLS_START
};

static const char* const LED_LOCATOR_210[] = {
    "PRM:/r1/c1/c2/c3/l1-LED2:l1",
    "PRM:/r1/c1/c2/c3/l2-LED2:l2",
    "PRM:/r1/c1/c2/c3/l3-LED2:l3",
    "PRM:/r1/c1/c2/c3/l4-LED2:l4",
    "PRM:/r1/c1/c2/c3/l5-LED2:l5",
    "PRM:/r1/c1/c2/c3/l6-LED2:l6",
    "PRM:/r1/c1/c2/c3/l7-LED2:l7",
};   

static const char* const LED_LOCATOR_7_HEAD[] = {
  "PRM:/r1/c1/c2/c3/l1-LED2:l1", // Head light (color)
  "PRM:/r1/c1/c2/c3/l2-LED2:l2", // Head light (white)
  "PRM:/r1/c1/c2/c3/l3-LED2:l3", // Mode Indicator (red)
  "PRM:/r1/c1/c2/c3/l4-LED2:l4", // Mode indicator (green)
  "PRM:/r1/c1/c2/c3/l5-LED2:l5", // Mode Indicator (blue)
  //  "PRM:/r1/c1/c2/c3/l6-LED2:l6", // Wireless light
};   

static const char* const LED_LOCATOR_7_FACE[] = {
  "PRM:/r1/c1/c2/c3/la-LED3:la", // Face light 1
  "PRM:/r1/c1/c2/c3/lb-LED3:lb", // Face light 2
  "PRM:/r1/c1/c2/c3/lc-LED3:lc", // Face light 3
  "PRM:/r1/c1/c2/c3/ld-LED3:ld", // Face light 4
  "PRM:/r1/c1/c2/c3/le-LED3:le", // Face light 5
  "PRM:/r1/c1/c2/c3/lf-LED3:lf", // Face light 6
  "PRM:/r1/c1/c2/c3/lg-LED3:lg", // Face light 7
  "PRM:/r1/c1/c2/c3/lh-LED3:lh", // Face light 8
  "PRM:/r1/c1/c2/c3/li-LED3:li", // Face light 9
  "PRM:/r1/c1/c2/c3/lj-LED3:lj", // Face light 10
  "PRM:/r1/c1/c2/c3/lk-LED3:lk", // Face light 11
  "PRM:/r1/c1/c2/c3/ll-LED3:ll", // Face light 12
  "PRM:/r1/c1/c2/c3/lm-LED3:lm", // Face light 13
  "PRM:/r1/c1/c2/c3/ln-LED3:ln", // Face light 14
};   

static const char* const LED_LOCATOR_7_BACK[] = {
  "PRM:/lu-LED3:lu",             // Back light (front, color)
  "PRM:/lv-LED3:lv",             // Back light (front, white)
  "PRM:/lw-LED3:lw",             // Back light (middle, color)
  "PRM:/lx-LED3:lx",             // Back light (middle, white)
  "PRM:/ly-LED3:ly",             // Back light (rear, color)
  "PRM:/lz-LED3:lz",             // Back light (rear, white)
};   

class BlinkingLED : public OObject {
public:
    BlinkingLED();
    virtual ~BlinkingLED() {}

    OSubject*  subject[numOfSubject];
    OObserver* observer[numOfObserver];

    virtual OStatus DoInit   (const OSystemEvent& event);
    virtual OStatus DoStart  (const OSystemEvent& event);
    virtual OStatus DoStop   (const OSystemEvent& event);
    virtual OStatus DoDestroy(const OSystemEvent& event);

    void Ready(const OReadyEvent& event);
    void Notify(const ONotifyEvent& event);

private:
    void BlinkingLED210();
    void BlinkingLED7Head();
    void OpenPrimitives();
    void OpenPrimitives210();
    void OpenPrimitives7Head();
    void NewCommandVectorData();
    void NewCommandVectorData210();
    void NewCommandVectorData7Head();
    void BlinkLED();
    void SetLED();

    static const size_t NUM_COMMAND_VECTOR = 2;
    static const size_t NUM_LEDS_210       = 7;
    static const size_t NUM_LEDS_7_HEAD    = 5;
    static const size_t NUM_LEDS_7_FACE    = 14;
    static const size_t NUM_LEDS_7_BACK    = 6;

    BlinkingLEDState  blinkingLEDState;
    OPrimitiveID      ledID_210[NUM_LEDS_210];
    OPrimitiveID      ledID_7_HEAD[NUM_LEDS_7_HEAD];
    RCRegion*         region[NUM_COMMAND_VECTOR * DigiCameCommandTypeMAX];
    DigiCameCommandType currentCommand;

    RobotDesignType robot;
};

#endif // BlinkingLED_h_DEFINED
