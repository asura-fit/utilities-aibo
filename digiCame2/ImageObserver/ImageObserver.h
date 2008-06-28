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

#ifndef ImageObserver_h_DEFINED
#define ImageObserver_h_DEFINED

#include <OPENR/OObject.h>
#include <OPENR/OSubject.h> 
#include <OPENR/OObserver.h>
#include <OPENR/OPrimitiveControl.h>
#include <DigiCame.h>
#include "def.h"
#include <string>

const std::string VERSION_STRING("3.0");

enum ImageObserverState {
    IOS_IDLE,
    IOS_START
};

static const char* const FBK_LOCATOR = "PRM:/r1/c1/c2/c3/i1-FbkImageSensor:F1";

// Back switch of ERS-210
const int BACK_SW    = 26;
static const char* const BACK_SW_LOCATOR = "PRM:/r6/s1-Sensor:s1"; // BACK SWITCH

// Back switches of ERS-7
const int BACK_SW_R  = 31;
const int BACK_SW_M  = 32;
const int BACK_SW_F  = 33;
static const char* const BACK_SW_REARE_LOCATOR = "PRM:/t2-Sensor:t2"; // BACK SENSOR (REAR)
static const char* const BACK_SW_MIDDLE_LOCATOR = "PRM:/t3-Sensor:t3"; // BACK SENSOR (MIDDLE)
static const char* const BACK_SW_FRONT_LOCATOR = "PRM:/t4-Sensor:t4"; // BACK SENSOR (FRONT)

class ImageObserver : public OObject {
public:  
    ImageObserver();
    virtual ~ImageObserver() {}
  
    OSubject*  subject[numOfSubject];
    OObserver* observer[numOfObserver];

    virtual OStatus DoInit   (const OSystemEvent& event);
    virtual OStatus DoStart  (const OSystemEvent& event);
    virtual OStatus DoStop   (const OSystemEvent& event);
    virtual OStatus DoDestroy(const OSystemEvent& event);
    
    void NotifyImage(const ONotifyEvent& event);
    void NotifyBackSwitch(const ONotifyEvent& event);
    void Ready(const OReadyEvent& event);

private:
    void LoadSettingFile(const char* const fileName);
	
    void OpenPrimitive();
    void SetCameraWB(longword wbmode = ocamparamWB_FL_MODE);
    void SetCameraShutter(longword shuttermode = ocamparamSHUTTER_MID);
    void SetCameraGain(longword gainmode = ocamparamGAIN_MID);
    void SetCdtVectorDataOfPinkBall();
    void PrintTagInfo(OFbkImageVectorData* imageVec);
    void SaveRawData(char* path,
                     OFbkImageVectorData* imageVec, OFbkImageLayer layer);
    bool ReconstructImage(OFbkImageVectorData* imageVec,
                          OFbkImageLayer layer,
                          byte** image, int* width, int* height);
    void FreeImage(byte* image);

    void PutYCbCrPixel(byte* img, int w,
                       int x, int y, byte ypix, byte cb, byte cr) {
        byte *ptr = img + 3 * (w * y + x);
        ptr[0] = ypix;
        ptr[1] = cb;
        ptr[2] = cr;
    }

    byte ClipRange(int val) {
        if (val < 0)        { return 0;         }
        else if (val > 255) { return 255;       }
        else                { return (byte)val; }
    }

    void InitSensorIndex(OSensorFrameVectorData* sensorVec);
    bool CheckBackSwitch(OSensorFrameVectorData* sensorVec);

    ImageObserverState  imageObserverState;
    OPrimitiveID        fbkID;
    bool                shutterRequest;

    longword mWbMode;
    longword mGainMode;
    longword mShutterMode;
    longword mImageSize;

    bool initSensorIndex;
    int  bswidx;
    DigiCameCommandType comm;
    RobotDesignType robot;
};

#endif // ImageObserver_h_DEFINED
