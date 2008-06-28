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

#include "TeamInfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


TeamInfo teamInfo;


TeamInfo::TeamInfo()
{
    for (int i = 0; i < NUM_TEAM; i ++) {
        for (int j = 0; j < NUM_ROBOT; j ++) {
            ip_[i][j] = 0;
        }
    }
}


TeamInfo::~TeamInfo()
{
    for (int i = 0; i < NUM_TEAM; i ++) {
        for (int j = 0; j < NUM_ROBOT; j ++) {
            delete[] ip_[i][j];
        }
    }
}


void
TeamInfo::Load()
{
    FILE* fp = fopen("ip.txt", "r");

    if (fp == 0) {
        fprintf(stderr, "TeamInfo : Cannot read ip.txt\n");
        return;
    }

    for (int i = 0; i < NUM_TEAM; i ++) {
        for (int j = 0; j < NUM_ROBOT; j ++) {
            char buf[BUFSIZ];
            if (fgets(buf, BUFSIZ, fp) == NULL)
                goto eof;
            buf[strlen(buf) - 1] = 0;
            delete[] ip_[i][j];
            ip_[i][j] = new char[strlen(buf) + 1];
            strcpy(ip_[i][j], buf);
        }
    }
eof:

    fclose(fp);
}


void
TeamInfo::Save()
{
    FILE* fp = fopen("ip.txt", "w");

    if (fp == 0) {
        fprintf(stderr, "!!!! ERROR TeamInfo : Cannot save ip.txt\n");
        exit(1);
    }

    for (int i = 0; i < NUM_TEAM; i ++) {
        for (int j = 0; j < NUM_ROBOT; j ++) {
            fprintf(fp, "%s\n", ip_[i][j]);
        }
    }

    fclose(fp);
}


const char*
TeamInfo::GetIPAddress(int team, int robot)
{
    assert(team < NUM_TEAM);
    assert(robot < NUM_ROBOT);

    return ip_[team][robot];
}
