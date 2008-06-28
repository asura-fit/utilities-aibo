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

#ifndef _TeamInfo_h_DEFINED
#define _TeamInfo_h_DEFINED


const int NUM_TEAM = 2;
const int NUM_ROBOT = 4;


class TeamInfo {
private:
    char* ip_[NUM_TEAM][NUM_ROBOT];
public:
    TeamInfo();
    ~TeamInfo();
    void Load();
    void Save();
    const char* GetIPAddress(int team, int robot);
};


extern TeamInfo teamInfo;


#endif // _TeamInfo_h_DEFINED
