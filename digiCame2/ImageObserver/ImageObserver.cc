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

#include <OPENR/ODataFormats.h>
#include <OPENR/OFbkImage.h>
#include <OPENR/OPENRAPI.h>
#include <OPENR/OSyslog.h>
#include <OPENR/core_macro.h>
#include <string>
#include <cstdio>
#include <cctype>
#include "ImageObserver.h"
#include "BMP.h"
#include "PPM.h"

const char* const DIGICAME_SETTING_FILE = "/MS/DIGICAME.CFG";
const char* const DIGICAME_LOG_FILE = "/MS/DIGICAME.LOG";

const int SETTING_WB = 0;
const int SETTING_GAIN = 1;
const int SETTING_SHUTTER = 2;
const int SETTING_SIZE = 3;

const char* const SETTING_NAME_TABLE[4][3] = {
    {"indoor", "outdoor", "fluorescent"},
    {"low", "mid", "high"},
    {"slow", "mid", "fast"},
    {"large", "extra", "both"}
};

enum ImageSIZE {imageLARGE=1, imageEXTRA, imageBOTH};

using namespace std;

ImageObserver::ImageObserver() : imageObserverState(IOS_IDLE),
                                 fbkID(oprimitiveID_UNDEF),
                                 initSensorIndex(false),
                                 shutterRequest(false),
                                 comm(DCCMD_LED_NORMAL_EYE),
                                 mWbMode(ocamparamWB_FL_MODE),
                                 mGainMode(ocamparamGAIN_MID),
                                 mShutterMode(ocamparamSHUTTER_MID),
                                 mImageSize(imageLARGE)
{
  
}

OStatus
ImageObserver::DoInit(const OSystemEvent& event)
{
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

    LoadSettingFile(DIGICAME_SETTING_FILE);

    OpenPrimitive();
    SetCameraWB(mWbMode);
    SetCameraGain(mGainMode);
    SetCameraShutter(mShutterMode);
    SetCdtVectorDataOfPinkBall();

    return oSUCCESS;
}

OStatus
ImageObserver::DoStart(const OSystemEvent& event)
{
    imageObserverState = IOS_START;

    ENABLE_ALL_SUBJECT;
    ASSERT_READY_TO_ALL_OBSERVER;

    return oSUCCESS;
}    

OStatus
ImageObserver::DoStop(const OSystemEvent& event)
{
    imageObserverState = IOS_IDLE;

    DISABLE_ALL_SUBJECT;
    DEASSERT_READY_TO_ALL_OBSERVER;

    return oSUCCESS;
}

OStatus
ImageObserver::DoDestroy(const OSystemEvent& event)
{
    DELETE_ALL_SUBJECT_AND_OBSERVER;
    return oSUCCESS;
}


// DigiCame Setting File Loader
void
ImageObserver::LoadSettingFile(const char* const fileName)
{

    FILE* fp;
    char buf[512];
    int count = 0;
    typedef struct SETTING {
        char name[512];
        char value[512];
    }SETTING;
    SETTING settings[3];

    try {
        fp = fopen(fileName, "r");
        if (!fp) {
            throw(string("Can't Open SettingFile"));
        }
        do {
            if (!fgets(buf, sizeof(buf), fp)) {
                throw(string("Can't Read SettingFile"));
            }
            if ('#' != buf[0] && !isspace(buf[0])) {
            sscanf(buf, "%s %s", settings[count].name, settings[count].value);
            count++;
            }
        } while(count < 4);
	} catch(string &e) {
		// Error
		if (fp) {
			fclose(fp);
		}
		fp = fopen(DIGICAME_LOG_FILE, "a");
		fprintf(fp, "Error : Settingfile(%s), Message : %s in %s(%d)\n"
			, fileName, e.c_str(), __FILE__, __LINE__);
		fprintf(fp, "*** Shutdown ***\n\n");
		fflush(fp);
		fclose(fp);
		//*(long*)(0xDEADDEAD) = 'E' + 'N' + 'D'; // Shutdown
		OPENR::Shutdown(OBootCondition(obcbPAUSE_SW));	// ”O‚-AÌ‚½‚ß$)B
		return;
	}
	fclose(fp);
    
	for (count = 0; count < 4; count++) {
		// DECODE SETTING
		if (settings[count].name == string("wb")) {
            // WB
            if (settings[count].value == string("indoor")) {
                mWbMode = ocamparamWB_INDOOR_MODE;
            } else if (settings[count].value == string("outdoor")) {
                mWbMode = ocamparamWB_OUTDOOR_MODE;
            } else if (settings[count].value == string("fl")
                       || settings[count].value == string("fluorescent") ) {
                mWbMode = ocamparamWB_FL_MODE;
            } else {
                throw (string("Invalied Setting [WB]"));
            }
        } else if (settings[count].name == string("gain")) {
            // GAIN
            if (settings[count].value == string("low")) {
                mGainMode = ocamparamGAIN_LOW;
            } else if (settings[count].value == string("mid")) {
                mGainMode = ocamparamGAIN_MID;
            } else if (settings[count].value == string("high")) {
                mGainMode = ocamparamGAIN_HIGH;
            } else {
                throw (string("Invalied Setting [GAIN]"));
            }
        } else if (settings[count].name == string("shutter")) {
            // SHUTTER
            if (settings[count].value == string("slow")) {
                mShutterMode = ocamparamSHUTTER_SLOW;
            } else if (settings[count].value == string("mid")) {
                mShutterMode = ocamparamSHUTTER_MID;
            } else if (settings[count].value == string("fast")) {
                mShutterMode = ocamparamSHUTTER_FAST;
            } else {
                throw (string("Invalied Setting [SHUTTER]"));
            }
        } else if (settings[count].name == string("size")) {
            // SHUTTER
            if (settings[count].value == string("large")) {
                mImageSize = imageLARGE;
            } else if (settings[count].value == string("extra")) {
                mImageSize = imageEXTRA;
            } else if (settings[count].value == string("both")) {
                mImageSize = imageBOTH;
            } else {
                throw (string("Invalied Setting [SIZE]"));
            }
        } else {
            throw (string("Invalid Setting Name[Unknown]"));
        }
    } // for
    
	fp = fopen(DIGICAME_LOG_FILE, "a");
	fprintf(fp, "Info : WB:%s GAIN:%s SHUTTER:%s SIZE:%s\n", 
		SETTING_NAME_TABLE[SETTING_WB][mWbMode - 1],
		SETTING_NAME_TABLE[SETTING_GAIN][mGainMode - 1],
		SETTING_NAME_TABLE[SETTING_SHUTTER][mShutterMode - 1],
		SETTING_NAME_TABLE[SETTING_SIZE][mImageSize - 1]);
	fclose(fp);
}

void
ImageObserver::NotifyImage(const ONotifyEvent& event)
{
    static int counter = 0;
    char path[128];
    char buf[512];

    if (imageObserverState == IOS_IDLE) {
        return; // do nothing
    }

    OFbkImageVectorData* fbkImageVectorData 
        = (OFbkImageVectorData*)event.Data(0);

    BMP bmp;
    PPM ppm;

    if (shutterRequest == true) {
      // set camera parameters and formats
      ppm.AddComment(string("CameraVersion:") + VERSION_STRING.c_str());

      sprintf(buf, "Model:%ld; Type:%s", 
          robot, ROBOT_DESIGN_NAME_TABLE[robot]);
      ppm.AddComment(string(buf));

      sprintf(buf, "CameraSettings:%ld,%ld,%ld; WB:%s GAIN:%s SHUTTER:%s", 
          mWbMode, mGainMode, mShutterMode, 
          SETTING_NAME_TABLE[SETTING_WB][mWbMode - 1],
          SETTING_NAME_TABLE[SETTING_GAIN][mGainMode - 1],
          SETTING_NAME_TABLE[SETTING_SHUTTER][mShutterMode - 1]);
      ppm.AddComment(string(buf));
      ppm.AddComment(string("YUV 8:8:8"));

      if (mImageSize == imageLARGE || mImageSize == imageBOTH) {
          // save a layer_h ppm file
          sprintf(path, "/MS/OPEN-R/MW/DATA/P/LAY_H%03d.PPM", counter);
          OSYSPRINT(("LAY_H%03d.PPM ... ", counter));
          //      ppm.SaveYCrCb2RGB(path, fbkImageVectorData, ofbkimageLAYER_H);
          ppm.SaveYCrCb(path, fbkImageVectorData, ofbkimageLAYER_H);
      }
          
      if (mImageSize == imageEXTRA || mImageSize == imageBOTH) {
          // save a reconstructed layer_h ppm file
          sprintf(path, "/MS/OPEN-R/MW/DATA/P/LAY_X%03d.PPM", counter);
          OSYSPRINT(("LAY_X%03d.PPM ... ", counter));
          byte *image;
          int width, height;
          ReconstructImage(fbkImageVectorData, 
                           ofbkimageLAYER_H, &image, &width, &height);
          ppm.SaveYCrCb(path, image, width, height);
      }

      OSYSPRINT(("DONE\n"));
      shutterRequest = false;
      counter++;

      if (counter % 2 == 0) {        // Set Mode Lamp
	  comm = DCCMD_LED_NORMAL_EYE;
      } else {                       // Uunset Mode Lamp
	  comm = DCCMD_LED_MODE_LAMP;
      }
    }
    observer[event.ObsIndex()]->AssertReady(event.SenderID());
}

void
ImageObserver::NotifyBackSwitch(const ONotifyEvent& event)
{
    OSYSPRINT(("ImageObserver::NotifyBackSwich\n"));

    OSensorFrameVectorData* sensorVec = (OSensorFrameVectorData*)event.Data(0);

    if (initSensorIndex == false) {
      InitSensorIndex(sensorVec);
      initSensorIndex = true;
    }

    bool backSwitch=CheckBackSwitch(sensorVec);

    int currentTime;
    OTime time;
    OPENR::GetTime(&time);
    currentTime = time.GetClock();

    shutterRequest = backSwitch;

    const int shutter_interval=3; // ignore chattering
    if (backSwitch == true) {
      do {
	OPENR::GetTime(&time);
      } while (time.GetClock() < currentTime + shutter_interval);
      backSwitch = false;
    }
    observer[event.ObsIndex()]->AssertReady(event.SenderID());
}

void
ImageObserver::Ready(const OReadyEvent& event)
{
    OSYSDEBUG(("ImageObserver::Ready\n"));
  
    subject[sbjBlinkingLED]->SetData(&comm, sizeof(comm));
    subject[sbjBlinkingLED]->NotifyObservers();
}


void
ImageObserver::OpenPrimitive()
{
    // CMOS image sensor
    OStatus result = OPENR::OpenPrimitive(FBK_LOCATOR, &fbkID);
    if (result != oSUCCESS) {
        OSYSLOG1((osyslogERROR, "%s : %s %d",
                  "ImageObserver::OpenPrimitive()",
                  "OPENR::OpenPrimitive() FAILED", result));
    }
}

void
ImageObserver::SetCameraWB(longword wbmode)
{
    OPrimitiveControl_CameraParam *wb;

    // Set white balance
    wb = new OPrimitiveControl_CameraParam(wbmode);

    OStatus result = OPENR::ControlPrimitive(fbkID, oprmreqCAM_SET_WHITE_BALANCE, wb, sizeof(OPrimitiveControl_CameraParam), 0, 0);
    if (result != oSUCCESS) {
        OSYSLOG1((osyslogERROR, "%s : %s %d",
	 	"ImageObserver::SetCameraWB()",
                  "OPENR::ControlPrimitive() FAILED", result));
    }
    free(wb);
}

void
ImageObserver::SetCameraShutter(longword shuttermode)
{
    OPrimitiveControl_CameraParam *shutter;

    // Set shutter speed
    shutter = new OPrimitiveControl_CameraParam(shuttermode);

    OStatus result = OPENR::ControlPrimitive(fbkID, oprmreqCAM_SET_SHUTTER_SPEED, shutter, sizeof(OPrimitiveControl_CameraParam), 0, 0);
    if (result != oSUCCESS) {
        OSYSLOG1((osyslogERROR, "%s : %s %d",
                  "ImageObserver::SetCameraShutter()",
                  "OPENR::ControlPrimitive() FAILED", result));
    }
    free(shutter);
}

void
ImageObserver::SetCameraGain(longword gainmode)
{
    OPrimitiveControl_CameraParam *gain;

    // Set gain
    gain = new OPrimitiveControl_CameraParam(gainmode);

    OStatus result = OPENR::ControlPrimitive(fbkID, oprmreqCAM_SET_GAIN, gain, sizeof(OPrimitiveControl_CameraParam), 0, 0);
    if (result != oSUCCESS) {
        OSYSLOG1((osyslogERROR, "%s : %s %d",
                  "ImageObserver::SetCameraGain()",
                  "OPENR::ControlPrimitive() FAILED", result));
    }
    free(gain);
}

void
ImageObserver::InitSensorIndex(OSensorFrameVectorData* sensorVec)
{
    OStatus result;
    OPrimitiveID sensorID;
    
    // back switch
    if (robot == RDT_ERS210) { /* for ERS-210 */
        result = OPENR::OpenPrimitive(BACK_SW_LOCATOR, &sensorID);
    } else if (robot == RDT_ERS7) { /* for ERS-7 */
        result = OPENR::OpenPrimitive(BACK_SW_REARE_LOCATOR, &sensorID);
//    result = OPENR::OpenPrimitive(BACK_SW_MIDDLE_LOCATOR, &sensorID);
//    result = OPENR::OpenPrimitive(BACK_SW_FRONT_LOCATOR, &sensorID);
    } else { /* for ERS-220 and others */
        OSYSLOG1((osyslogERROR, "%s : %s",
                  "ImageObserver::InitSensorIndex()",
                  "unsupported robot type"));
        result = oFAIL;
    }
    if (result != oSUCCESS) {
        OSYSLOG1((osyslogERROR, "%s : %s %d",
                  "ImageObserver::InitSensorIndex()",
                  "OPENR::OpenPrimitive() FAILED", result));
    }
    for (int j = 0; j < sensorVec->vectorInfo.numData; j++) {
      OSensorFrameInfo* info = sensorVec->GetInfo(j);
      if (info->primitiveID == sensorID) {
	bswidx = j;
        if (robot == RDT_ERS210) { /* for ERS-210 */
	    OSYSPRINT(("Back switch is [%2d] %s\n",
	    	       bswidx, BACK_SW_LOCATOR));
        } else if (robot == RDT_ERS7) { /* for ERS-7 */
	    OSYSPRINT(("Back switch is [%2d] %s\n",
		       bswidx, BACK_SW_REARE_LOCATOR));
//		       bswidx, BACK_SW_MIDDLE_LOCATOR));
//		       bswidx, BACK_SW_FRONT_LOCATOR));
        } else { /* for ERS-220 and others */
            OSYSLOG1((osyslogERROR, "%s : %s",
                      "ImageObserver::InitSensorIndex()",
                      "unsupported robot type"));
        }
	break;
      }
    }
}


void
ImageObserver::SetCdtVectorDataOfPinkBall()
{
    OStatus result;
    MemoryRegionID  cdtVecID;
    OCdtVectorData* cdtVec;
    OCdtInfo*       cdt;

    result = OPENR::NewCdtVectorData(&cdtVecID, &cdtVec);
    if (result != oSUCCESS) {
        OSYSLOG1((osyslogERROR, "%s : %s %d",
                  "ImageObserver::SetCdtVectorDataOfPinkBall()",
                  "OPENR::NewCdtVectorData() FAILED", result));
        return;
    }

    cdtVec->SetNumData(1);

    cdt = cdtVec->GetInfo(0);
    cdt->Init(fbkID, ocdtCHANNEL0);

    //
    // cdt->Set(Y_segment, Cr_max,  Cr_min, Cb_max, Cb_min)
    //
    cdt->Set( 0, 230, 150, 190, 120);
    cdt->Set( 1, 230, 150, 190, 120);
    cdt->Set( 2, 230, 150, 190, 120);
    cdt->Set( 3, 230, 150, 190, 120);
    cdt->Set( 4, 230, 150, 190, 120);
    cdt->Set( 5, 230, 150, 190, 120);
    cdt->Set( 6, 230, 150, 190, 120);
    cdt->Set( 7, 230, 150, 190, 120);
    cdt->Set( 8, 230, 150, 190, 120);
    cdt->Set( 9, 230, 150, 190, 120);
    cdt->Set(10, 230, 150, 190, 120);
    cdt->Set(11, 230, 150, 190, 120);
    cdt->Set(12, 230, 150, 190, 120);
    cdt->Set(13, 230, 150, 190, 120);
    cdt->Set(14, 230, 150, 190, 120);
    cdt->Set(15, 230, 150, 190, 120);
    cdt->Set(16, 230, 150, 190, 120);
    cdt->Set(17, 230, 150, 190, 120);
    cdt->Set(18, 230, 150, 190, 120);
    cdt->Set(19, 230, 150, 190, 120);
    cdt->Set(20, 230, 160, 190, 120);
    cdt->Set(21, 230, 160, 190, 120);
    cdt->Set(22, 230, 160, 190, 120);
    cdt->Set(23, 230, 160, 190, 120);
    cdt->Set(24, 230, 160, 190, 120);
    cdt->Set(25, 230, 160, 190, 120);
    cdt->Set(26, 230, 160, 190, 120);
    cdt->Set(27, 230, 160, 190, 120);
    cdt->Set(28, 230, 160, 190, 120);
    cdt->Set(29, 230, 160, 190, 120);
    cdt->Set(30, 230, 160, 190, 120);
    cdt->Set(31, 230, 160, 190, 120);

    result = OPENR::SetCdtVectorData(cdtVecID);
    if (result != oSUCCESS) {
        OSYSLOG1((osyslogERROR, "%s : %s %d",
                  "ImageObserver::SetCdtVectorDataOfPinkBall()",
                  "OPENR::SetCdtVectorData() FAILED", result));
    }

    result = OPENR::DeleteCdtVectorData(cdtVecID);
    if (result != oSUCCESS) {
        OSYSLOG1((osyslogERROR, "%s : %s %d",
                  "ImageObserver::SetCdtVectorDataOfPinkBall()",
                  "OPENR::DeleteCdtVectorData() FAILED", result));
    }
}

void
ImageObserver::PrintTagInfo(OFbkImageVectorData* imageVec)
{
    OFbkImageInfo* info = imageVec->GetInfo(ofbkimageLAYER_H);
    byte*          data = imageVec->GetData(ofbkimageLAYER_H);
    OFbkImage yImage(info, data, ofbkimageBAND_Y);
    
    OSYSPRINT(("FrameNumber %x FieldCounter %x ColorFreq ",
               imageVec->GetInfo(0)->frameNumber, yImage.FieldCounter()));

    OSYSPRINT(("%d ", yImage.ColorFrequency(ocdtCHANNEL0)));
    OSYSPRINT(("%d ", yImage.ColorFrequency(ocdtCHANNEL1)));
    OSYSPRINT(("%d ", yImage.ColorFrequency(ocdtCHANNEL2)));
    OSYSPRINT(("%d ", yImage.ColorFrequency(ocdtCHANNEL3)));
    OSYSPRINT(("%d ", yImage.ColorFrequency(ocdtCHANNEL4)));
    OSYSPRINT(("%d ", yImage.ColorFrequency(ocdtCHANNEL5)));
    OSYSPRINT(("%d ", yImage.ColorFrequency(ocdtCHANNEL6)));
    OSYSPRINT(("%d ", yImage.ColorFrequency(ocdtCHANNEL7)));
    OSYSPRINT(("\n"));
}

void
ImageObserver::SaveRawData(char* path,
                           OFbkImageVectorData* imageVec, OFbkImageLayer layer)
{
    OFbkImageInfo* info = imageVec->GetInfo(layer);
    byte*          data = imageVec->GetData(layer);

    size_t size;
    if (layer == ofbkimageLAYER_C) {
        size = info->width * info->height;
    } else {
        size = 3 * info->width * info->height;
    }

    FILE* fp = fopen(path, "w");
    if (fp == 0) return;
    fwrite(data, 1, size, fp);
    fclose(fp);
}

bool
ImageObserver::CheckBackSwitch(OSensorFrameVectorData* sensorVec)
{
    OSensorFrameData* data = sensorVec->GetData(bswidx);

    int value;
    value = data->frame[0].value | data->frame[1].value |
      data->frame[2].value | data->frame[3].value;

    OSYSDEBUG(("BACK SW is | %d\n", value));
    if (value > 0) return true;
    return false;
}

bool
ImageObserver::ReconstructImage(OFbkImageVectorData* imageVec,
                                OFbkImageLayer layer,
                                byte** image, int* width, int* height)
{
    *image = 0;
    *width = *height = 0;
    if (layer == ofbkimageLAYER_C) return false;

    OFbkImageInfo* info = imageVec->GetInfo(layer);
    byte*          data = imageVec->GetData(layer);

    OFbkImage yLLImg(info, data, ofbkimageBAND_Y); // Y_LL
    OFbkImage yLHImg(info, data, ofbkimageBAND_Y_LH);
    OFbkImage yHLImg(info, data, ofbkimageBAND_Y_HL);
    OFbkImage yHHImg(info, data, ofbkimageBAND_Y_HH);

    OFbkImage crImg(info, data, ofbkimageBAND_Cr);
    OFbkImage cbImg(info, data, ofbkimageBAND_Cb);


    int w = yLLImg.Width();
    int h = yLLImg.Height();

    byte* img = (byte*)malloc((2*w*3)*(2*h));
    if (img == 0) return false;

    for (int y = 0; y < h; y++) {

        for (int x = 0; x < w; x++) {
            //
            // yLH, yHL, yHH : offset binary [0, 255] -> signed int [-128, 127]
            //
            int yLL = (int)yLLImg.Pixel(x, y);
            int yLH = (int)yLHImg.Pixel(x, y) - 128;
            int yHL = (int)yHLImg.Pixel(x, y) - 128;
            int yHH = (int)yHHImg.Pixel(x, y) - 128;

            int a = yLL + yLH + yHL + yHH; // ypix11
            int b = 2 * (yLL + yLH);       // ypix11 + ypix01
            int c = 2 * (yLL + yHL);       // ypix11 + ypix10
            int d = 2 * (yLL + yHH);       // ypix11 + ypix00

            byte ypix00 = ClipRange(d - a);
            byte ypix10 = ClipRange(c - a);
            byte ypix01 = ClipRange(b - a);
            byte ypix11 = ClipRange(a);

            byte cb = cbImg.Pixel(x, y);
            byte cr = crImg.Pixel(x, y);

            PutYCbCrPixel(img, 2*w, 2*x,   2*y,   ypix00, cr, cb);
            PutYCbCrPixel(img, 2*w, 2*x+1, 2*y,   ypix10, cr, cb);
            PutYCbCrPixel(img, 2*w, 2*x,   2*y+1, ypix01, cr, cb);
            PutYCbCrPixel(img, 2*w, 2*x+1, 2*y+1, ypix11, cr, cb);
        }

    }

    *image  = img;
    *width  = 2 * w;
    *height = 2 * h;

    return true;
}

void
ImageObserver::FreeImage(byte* image)
{
    if (image != 0) free(image);
}

