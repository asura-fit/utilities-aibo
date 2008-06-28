/*
 * Copyright 2002,2003 Sony Corporation
 *
 * Permission to use, copy, modify, and redistribute this software is
 * hereby granted.
 *
 * This software is provided "as is" without warranty of any kind,
 * either expressed or implied, including but not limited to the
 * implied warranties of fitness for a particular purpose.
 *
 *
 * Modified at University Bremen
 * for RoboCup 2004 - Martin Fritsche, March 2004
 */

#ifndef _RoboCupGameControlData_h_DEFINED
#define _RoboCupGameControlData_h_DEFINED


#ifdef __cplusplus
extern "C" {
#endif


/* Game state */

typedef char RState;

const RState ROBOCUP_STATE_INITIAL   = 0x00; /* initial state             */
const RState ROBOCUP_STATE_READY     = 0x01; /* position for kickoff      */
const RState ROBOCUP_STATE_SET       = 0x02; /* stop and wait for kickoff */
const RState ROBOCUP_STATE_PLAYING   = 0x03; /* play game                 */
const RState ROBOCUP_STATE_PENALIZED = 0x04; /* penalized, do not move    */
const RState ROBOCUP_STATE_FINISHED  = 0x05; /* game set                  */



/* Kickoff */

typedef char RKickOff;

const RKickOff ROBOCUP_KICKOFF_INVALID  = 0x00;  /* sent in all states but "ready" and "set" */
const RKickOff ROBOCUP_KICKOFF_OWN      = 0x01;
const RKickOff ROBOCUP_KICKOFF_OPPONENT = 0x02;



/* Color */

typedef char RTeamColor;

const RTeamColor ROBOCUP_TEAMCOLOR_RED  = 0x00; 
const RTeamColor ROBOCUP_TEAMCOLOR_BLUE = 0x01;



/* Penalty */

typedef char RPenalty;
const int NUM_PENALTY = 8;                      /* used by GameController */

const RPenalty PENALTY_NONE                = 0; /* sent in all states but "penalized" */
const RPenalty PENALTY_BALL_HOLDING        = 1;
const RPenalty PENALTY_KEEPER_CHARGE       = 2;
const RPenalty PENALTY_FIELD_PLAYER_CHARGE = 3;
const RPenalty PENALTY_ILLEGAL_DEFENDER    = 4;
const RPenalty PENALTY_ILLEGAL_DEFENSE     = 5;
const RPenalty PENALTY_OBSTRUCTION         = 6;
const RPenalty PENALTY_REQ_FOR_PICKUP      = 7;



/* Data type definition */

typedef struct {
    RState      state;                  /* current state of player */
    RTeamColor  teamColor;              /* team color              */
    RKickOff    kickoff;                /* kickoff information     */
    short       ownScore;               /* score of own team       */
    short       opponentScore;          /* score of opponent team  */
    RPenalty    penalty;                /* current penalty, only valid in "penalized" */
} RoboCupGameControlData;


#ifdef __cplusplus
}
#endif


#endif /* _RoboCupGameControlData_h_DEFINED */

