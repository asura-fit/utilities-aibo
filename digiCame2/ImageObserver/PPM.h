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

#ifndef PPM_h_DEFINED
#define PPM_h_DEFINED

#include <stdio.h>
#include <string>
#include <vector>
#include <OPENR/ODataFormats.h>

struct PPMHeader {
    slongword width;            // Width of image
    slongword height;           // Height of image
    slongword depth;            // Depth of pixel
};

class PPM {
public:
    PPM() {}
    ~PPM() {}
    
    bool SaveYCrCb(char* path, 
		   OFbkImageVectorData* imageVec, OFbkImageLayer layer);

    bool SaveYCrCb(char* path, byte* image, int width, int height);

    bool SaveYCrCb2RGB(char* path, 
                       OFbkImageVectorData* imageVec, OFbkImageLayer layer);

	void AddComment(const std::string& comment)
		{ mComments.push_back(comment); }
#if 0
    bool SaveLayerC(char* basepath, OFbkImageVectorData* imageVec);
#endif

private:
    void SavePPMHeader(FILE* fp, const PPMHeader& header);
    void YCrCb2RGB(byte y, byte cr, byte cb, byte* r, byte* g, byte* b);
    std::vector<std::string> mComments;

};

#endif // PPM_h_DEFINED
