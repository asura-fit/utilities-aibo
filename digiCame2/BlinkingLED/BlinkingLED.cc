// Copyright 2002 Sony Corporation 
//
// Permission to use, copy, modify, and redistribute this software for
// non-commercial use is hereby granted.
//
// This software is provided "as is" without warranty of any kind,
// either expressed or implied, including but not limited to the
// implied warranties of fitness for a particular purpose.
//

#include <OPENR/OPENRAPI.h>
#include <OPENR/OSyslog.h>
#include <OPENR/core_macro.h>
#include "BlinkingLED.h"

BlinkingLED::BlinkingLED() : blinkingLEDState(BLS_IDLE),
			     currentCommand(DCCMD_LED_OFF)
{
    /* for ERS-210 */
    for (int i = 0; i < NUM_LEDS_210; i++) {
        ledID_210[i] = oprimitiveID_UNDEF;
    }
    /* for ERS-7 */
    for (int i = 0; i < NUM_LEDS_7_HEAD; i++) {
        ledID_7_HEAD[i] = oprimitiveID_UNDEF;
    }
    /* for ERS-210 and ERS-7 */
    for (int i = 0; i < NUM_COMMAND_VECTOR * DigiCameCommandTypeMAX; i++) {
        region[i] = 0;
    }
}

OStatus
BlinkingLED::DoInit(const OSystemEvent& event)
{
    OSYSDEBUG(("BlinkingLED::DoInit()\n"));

    NEW_ALL_SUBJECT_AND_OBSERVER;
    REGISTER_ALL_ENTRY;
    SET_ALL_READY_AND_NOTIFY_ENTRY;

    char design[orobotdesignNAME_MAX+1];
    OStatus result = OPENR::GetRobotDesign(design);
    if (result != oSUCCESS) {
        OSYSLOG1((osyslogERROR, "%s : %s %d",
                  "BlinkingLED::DoInit()",
                  "OPENR::GetRobotDesign() FAILED", result));
    }

    if (!strcmp(design, "ERS-210")) {
        robot = RDT_ERS210;
    } else if (!strcmp(design, "ERS-220")) {
        robot = RDT_ERS220;
    } else if (!strcmp(design, "ERS-7")) {
        robot = RDT_ERS7;
    } else {
        robot = RDT_UNDEF;
        OSYSLOG1((osyslogERROR,
                  "BlinkingLED::DoInit() UNKNOWN ROBOT DESIGN"));
    }

    OpenPrimitives();
    NewCommandVectorData();
    OStatus st = OPENR::SetMotorPower(opowerON);
    OSYSDEBUG(("OPENR::SetMotorPower(opowerON) %d\n", st));
    
    return oSUCCESS;
}

OStatus
BlinkingLED::DoStart(const OSystemEvent& event)
{
    OSYSDEBUG(("BlinkingLED::DoStart()\n"));

    blinkingLEDState = BLS_START;

    ENABLE_ALL_SUBJECT;
    ASSERT_READY_TO_ALL_OBSERVER;

    return oSUCCESS;
}

OStatus
BlinkingLED::DoStop(const OSystemEvent& event)
{
    OSYSDEBUG(("BlinkingLED::DoStop()\n"));

    blinkingLEDState = BLS_IDLE;

    DISABLE_ALL_SUBJECT;    
    DEASSERT_READY_TO_ALL_OBSERVER;

    return oSUCCESS;
}

OStatus
BlinkingLED::DoDestroy(const OSystemEvent& event)
{
    DELETE_ALL_SUBJECT_AND_OBSERVER;
    return oSUCCESS;
}

void
BlinkingLED::Ready(const OReadyEvent& event)
{
    // OSYSDEBUG(("BlinkingLED::Ready()\n"));
    SetLED();
}

void
BlinkingLED::Notify(const ONotifyEvent& event)
{
    DigiCameCommandType *comm = (DigiCameCommandType *)event.Data(0);

    // OSYSDEBUG(("BlinkingLED::Notify() %d\n", *comm));

    if (*comm == DCCMD_LED_OFF) {
      // OSYSDEBUG(("DCCMD_LED_OFF\n"));
    } else if (*comm == DCCMD_LED_NORMAL_EYE) {
      // OSYSDEBUG(("DCCMD_LED_NORMAL_EYE\n"));
    } else if (*comm == DCCMD_LED_MODE_LAMP) {
      // OSYSDEBUG(("DCCMD_LED_MODE_LAMP\n"));
    }
    currentCommand = *comm;
    observer[event.ObsIndex()]->AssertReady(event.SenderID());
}

void
BlinkingLED::OpenPrimitives()
{
    if (robot == RDT_ERS210) { /* for ERS-210 */
        OpenPrimitives210();
    } else if (robot == RDT_ERS7) { /* for ERS-7 */
        OpenPrimitives7Head();
    } else { /* for ERS-220 and others */
        OSYSLOG1((osyslogERROR, "%s : %s",
                  "BlinkingLED::OpenPrimitives()",
                  "unsupported robot type"));
    }
}

void
BlinkingLED::OpenPrimitives210()
{
    for (int i = 0; i < NUM_LEDS_210; i++) {
        OStatus result = OPENR::OpenPrimitive(LED_LOCATOR_210[i], &ledID_210[i]);
        if (result != oSUCCESS) {
            OSYSLOG1((osyslogERROR, "%s : %s %d",
                     "BlinkingLED::OpenPrimitives()",
                     "OPENR::OpenPrimitive() FAILED", result));
        }
    }
}

void
BlinkingLED::OpenPrimitives7Head()
{
    for (int i = 0; i < NUM_LEDS_7_HEAD; i++) {
        OStatus result = OPENR::OpenPrimitive(LED_LOCATOR_7_HEAD[i], &ledID_7_HEAD[i]);
        if (result != oSUCCESS) {
            OSYSLOG1((osyslogERROR, "%s : %s %d",
                     "BlinkingLED::OpenPrimitives()",
                     "OPENR::OpenPrimitive() FAILED", result));
    }
  }
}

void
BlinkingLED::NewCommandVectorData()
{
    if (robot == RDT_ERS210) { /* for ERS-210 */
        NewCommandVectorData210();
    } else if (robot == RDT_ERS7) { /* for ERS-7 */
        NewCommandVectorData7Head();
    } else { /* for ERS-220 and others */
        OSYSLOG1((osyslogERROR, "%s : %s",
                  "BlinkingLED::NewCommandVectorData()",
                  "unsupported robot type"));
    }
}

void
BlinkingLED::NewCommandVectorData210()
{
    OStatus result;
    MemoryRegionID      cmdVecDataID;
    OCommandVectorData* cmdVecData;

    for (int i = 0; i < NUM_COMMAND_VECTOR * DigiCameCommandTypeMAX; i++) {

        result = OPENR::NewCommandVectorData(NUM_LEDS_210, 
                                             &cmdVecDataID, &cmdVecData);
        if (result != oSUCCESS) {
            OSYSLOG1((osyslogERROR, "%s : %s %d",
                      "BlinkingLED::NewCommandVectorData()",
                      "OPENR::NewCommandVectorData() FAILED", result));
        }

        region[i] = new RCRegion(cmdVecData->vectorInfo.memRegionID,
                                 cmdVecData->vectorInfo.offset,
                                 (void*)cmdVecData,
                                 cmdVecData->vectorInfo.totalSize);

        cmdVecData->SetNumData(NUM_LEDS_210);

        for (int j = 0; j < NUM_LEDS_210; j++) {
            OCommandInfo* info = cmdVecData->GetInfo(j);
            info->Set(odataLED_COMMAND2, ledID_210[j], 1);
            OCommandData* data = cmdVecData->GetData(j);
            OLEDCommandValue2* val = (OLEDCommandValue2*)data->value;

	    if (i < 2) {               // for DCCMD_LED_OFF
	        val[0].led = oledOFF;
	    } else if (i < 4) {        // for DCCMD_LED_NORMAL_EYE
  	        if ((j==1) || (j==4)) {
	            val[0].led = oledON;
	        } else {
	            val[0].led = oledOFF;
		}
	    } else {                   // for DCCMD_LED_MODE_LAMP
	        if ((j==1) || (j==4) || (j==6)) {	      
	            val[0].led = oledON;
	        } else {
	            val[0].led = oledOFF;
		}
	    }
            val[0].period = 16; // 8ms * 64 = 512ms
	    OSYSDEBUG(("ledID[%d] %d\n", j, val[0].led));
        }
    }
}
void
BlinkingLED::NewCommandVectorData7Head()
{
    OStatus result;
    MemoryRegionID      cmdVecDataID;
    OCommandVectorData* cmdVecData;

    for (int i = 0; i < NUM_COMMAND_VECTOR * DigiCameCommandTypeMAX; i++) {

        result = OPENR::NewCommandVectorData(NUM_LEDS_7_HEAD, 
                                             &cmdVecDataID, &cmdVecData);
        if (result != oSUCCESS) {
            OSYSLOG1((osyslogERROR, "%s : %s %d",
                      "BlinkingLED::NewCommandVectorData()",
                      "OPENR::NewCommandVectorData() FAILED", result));
        }

        region[i] = new RCRegion(cmdVecData->vectorInfo.memRegionID,
                                 cmdVecData->vectorInfo.offset,
                                 (void*)cmdVecData,
                                 cmdVecData->vectorInfo.totalSize);

        cmdVecData->SetNumData(NUM_LEDS_7_HEAD);

        for (int j = 0; j < NUM_LEDS_7_HEAD; j++) {
            OCommandInfo* info = cmdVecData->GetInfo(j);
            info->Set(odataLED_COMMAND2, ledID_7_HEAD[j], 1);
            OCommandData* data = cmdVecData->GetData(j);
            OLEDCommandValue2* val = (OLEDCommandValue2*)data->value;

	    if (i < 2) {               // for DCCMD_LED_OFF
	        val[0].led = oledOFF;
	    } else if (i < 4) {        // for DCCMD_LED_NORMAL_EYE (green)
  	        if (j==3) {
	            val[0].led = oledON;
	        } else {
	            val[0].led = oledOFF;
		}
	    } else {                   // for DCCMD_LED_MODE_LAMP (green and white)
	        if ((j==3) || (j==1)) {	      
	            val[0].led = oledON;
	        } else {
	            val[0].led = oledOFF;
		}
	    }
            val[0].period = 16; // 8ms * 64 = 512ms
	    OSYSDEBUG(("ledID[%d] %d\n", j, val[0].led));
        }
    }
}

void
BlinkingLED::BlinkLED()
{
    static int index = -1;

    if (index == -1) { // BlinkLED() is called first time.
        index = 0;
        subject[sbjBlink]->SetData(region[index]);
        index++;
    } 

    subject[sbjBlink]->SetData(region[index]);
    subject[sbjBlink]->NotifyObservers();

    index++;
    index = index % NUM_COMMAND_VECTOR;
    OSYSDEBUG(("index %d\n", index));
}

void
BlinkingLED::SetLED()
{
    static int index = -1;
    int n=currentCommand;

    // OSYSDEBUG(("n %d, index %d\n", n, index));
    n = n % DigiCameCommandTypeMAX;

    if (index == -1) { // SetLED() is called first time.
        index = 0;
        subject[sbjBlink]->SetData(region[n * NUM_COMMAND_VECTOR + index]);
        index++;
    } 

    subject[sbjBlink]->SetData(region[n * NUM_COMMAND_VECTOR + index]);
    subject[sbjBlink]->NotifyObservers();

    index++;
    index = index % NUM_COMMAND_VECTOR;
}
