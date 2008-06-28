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

#include <OPENR/OFbkImage.h>
#include <OPENR/OSyslog.h>
#include "PPM.h"

bool 
PPM::SaveYCrCb(char* path,
	       OFbkImageVectorData* imageVec, OFbkImageLayer layer)
{
    if (layer == ofbkimageLAYER_C) return false;

    byte rgb[3];
    OFbkImageInfo* info = imageVec->GetInfo(layer);
    byte*          data = imageVec->GetData(layer);
    
    OFbkImage yImage(info, data, ofbkimageBAND_Y);
    OFbkImage crImage(info, data, ofbkimageBAND_Cr);
    OFbkImage cbImage(info, data, ofbkimageBAND_Cb);

    PPMHeader     header;

    header.width     = yImage.Width();
    header.height    = yImage.Height();
    header.depth     = 255;

    FILE* fp = fopen(path, "w");
    if (fp == 0) return false;

    SavePPMHeader(fp, header);

    for (int y = 0; y < header.height; y++) {
        for (int x = 0; x < header.width; x++) {

	    rgb[0] = yImage.Pixel(x, y);
	    rgb[1] = crImage.Pixel(x, y);
	    rgb[2] = cbImage.Pixel(x, y);
            fwrite(rgb, 1, 3, fp);
        }
    }

    fclose(fp);
    return true;
}

bool
PPM::SaveYCrCb(char* path, byte* image, int width, int height)
{
    byte rgb[3];
    byte*          data = image;
    
    PPMHeader     header;

    header.width     = width;
    header.height    = height;
    header.depth     = 255;

    FILE* fp = fopen(path, "w");
    if (fp == 0) return false;

    SavePPMHeader(fp, header);

    fwrite(data, header.width * 3, header.height, fp);

    fclose(fp);
    return true;
}

bool 
PPM::SaveYCrCb2RGB(char* path,
                   OFbkImageVectorData* imageVec, OFbkImageLayer layer)
{
    if (layer == ofbkimageLAYER_C) return false;

    byte rgb[3];
    OFbkImageInfo* info = imageVec->GetInfo(layer);
    byte*          data = imageVec->GetData(layer);
    
    OFbkImage yImage(info, data, ofbkimageBAND_Y);
    OFbkImage crImage(info, data, ofbkimageBAND_Cr);
    OFbkImage cbImage(info, data, ofbkimageBAND_Cb);

    PPMHeader     header;

    header.width     = yImage.Width();
    header.height    = yImage.Height();
    header.depth     = 255;

    FILE* fp = fopen(path, "w");
    if (fp == 0) return false;

    SavePPMHeader(fp, header);

    for (int y = 0; y < header.height; y++) {
        for (int x = 0; x < header.width; x++) {
            YCrCb2RGB(yImage.Pixel(x, y),
                      crImage.Pixel(x, y),
                      cbImage.Pixel(x, y),
                      &rgb[0], &rgb[1], &rgb[2]);
            fwrite(rgb, 1, 3, fp);
        }
    }

    fclose(fp);
    return true;
}

#if 0
bool 
PPM::SaveLayerC(char* basepath, OFbkImageVectorData* imageVec)
{
    byte rgb[3];
    char path[128];
    OFbkImageInfo* info = imageVec->GetInfo(ofbkimageLAYER_C);
    byte*          data = imageVec->GetData(ofbkimageLAYER_C);
    
    OFbkImage cdtImage(info, data, ofbkimageBAND_CDT);

    PPMHeader     header;
    PPMInfoHeader infoheader;

    infoheader.width     = cdtImage.Width();
    infoheader.height    = cdtImage.Height();
    infoheader.imagesize = cdtImage.Width() * cdtImage.Height() * 3;

    header.size = infoheader.imagesize + 54;

    for (int i = 0; i < ocdtNUM_CHANNELS; i++) {

        byte plane = 0x01 << i;
        sprintf(path, "%s%d.PPM", basepath, i);
        
        FILE* fp = fopen(path, "w");
        if (fp == 0) return false;

        SavePPMHeader(fp, header);
        SavePPMInfoHeader(fp, infoheader);

        for (int y = infoheader.height - 1 ; y >= 0; y--) {
            for (int x = 0; x < infoheader.width; x++) {
                if (cdtImage.Pixel(x, y) & plane) {
                    rgb[0] = rgb[1] = rgb[2] = 255;
                } else {
                    rgb[0] = rgb[1] = rgb[2] = 0;
                }
                fwrite(rgb, 1, 3, fp);
            }
        }

        fclose(fp);
    }

    return true;
}
#endif
               
void
PPM::SavePPMHeader(FILE* fp, const PPMHeader& header)
{
    fprintf(fp, "P6\n");
    std::vector<std::string>::iterator iter;
    for(iter = mComments.begin(); iter != mComments.end(); iter++) {
		fprintf(fp, "%s%s\n", "#", iter->c_str());
    }
    fprintf(fp, "%d %d\n", header.width, header.height);
    fprintf(fp, "%d\n", header.depth);
}

void
PPM::YCrCb2RGB(byte y, byte cr, byte cb, byte* r, byte* g, byte* b)
{
    // offset binary [0, 255] -> signed char [-128, 127]
    sbyte scr = (sbyte)(cr ^ 0x80);
    sbyte scb = (sbyte)(cb ^ 0x80);

    double Y  = (double)y / 255.0;   //  0.0 <= Y  <= 1.0
    double Cr = (double)scr / 128.0; // -1.0 <= Cr <  1.0
    double Cb = (double)scb / 128.0; // -1.0 <= Cb <  1.0

    double R = 255.0 * (Y + Cb);
    double G = 255.0 * (Y - 0.51*Cb - 0.19*Cr);
    double B = 255.0 * (Y + Cr);

    if (R > 255.0) {
        *r = 255;
    } else if (R < 0.0) {
        *r = 0;
    } else {
        *r = (byte)R;
    }

    if (G > 255.0) {
        *g = 255;
    } else if (G < 0.0) {
        *g = 0;
    } else {
        *g = (byte)G;
    }

    if (B > 255.0) {
        *b = 255;
    } else if (B < 0.0) {
        *b = 0;
    } else {
        *b = (byte)B;
    }
}
