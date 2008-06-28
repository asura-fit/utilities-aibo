/*
    Copyright (C) 2005  University Of New South Wales

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

import java.nio.ByteBuffer;
/*
 * RoboCupGameControlData.java
 *
 * Created on 10 January 2005, 12:17
 */

/**
 * This class holds the RoboCupGameControl data structure. Refer to the readme
 * for more information of its structure. The class can convert itself into a 
 * little endian byte array for sending over a network
 * 
 * @author willu@cse.unsw.edu.au shnl327@cse.unsw.edu.au
 */


public class RoboCupGameControlData {
    
    
    // RoboCupGameControlData has two TeamInfos in it, a red and a blue
    private TeamInfo[] teams = new TeamInfo[2];
    
    private int gameState;                                
    private int firstHalf = Constants.TRUE;
    private int estimatedSecs;
    private int kickOffTeam;
    private int dropInTeam;
    private int dropInTime = -1;        // -ve time = first dropin yet to happen   
    private boolean noDropInYet = true; // this will stop the timer from counting drop in time
    
    // each team has 11 fields
    // 2*teams + 8 fields from this class = 30 fields, determines size of byte stream
    private static final int fields = 30;   
    
    
    /** Creates a new instance of RoboCupGameControlData */
    public RoboCupGameControlData() {
    }
    
    public RoboCupGameControlData(int blueNumber, int redNumber) {
        teams[0] = new TeamInfo(Constants.TEAM_BLUE, blueNumber);
        teams[1] = new TeamInfo(Constants.TEAM_RED, redNumber);
        estimatedSecs = 60*Constants.TIME_MINUTES;
    }

        
    // get the object as a byte array
    // do this by converting the team's combined strings and field values and 
    // performing a getBytes on it
    public synchronized byte[] getAsByteArray() {
       
        byte[] array = new byte[fields * Constants.INT_SIZE];
        

        // copy the version number, game state, first half flag, kick off team, and est. seconds
        int offset = 0;
        System.arraycopy(toByteArray(Constants.STRUCT_HEADER), 0, array, offset, 4); offset+=4;
        System.arraycopy(toByteArray(Constants.STRUCT_VERSION), 0, array, offset, Constants.INT_SIZE); offset+=4;
        System.arraycopy(toByteArray(gameState), 0, array, offset, Constants.INT_SIZE); offset+=4;        
        System.arraycopy(toByteArray(firstHalf), 0, array, offset, Constants.INT_SIZE); offset+=4;        
        System.arraycopy(toByteArray(kickOffTeam), 0, array, offset, Constants.INT_SIZE); offset+=4;
        System.arraycopy(toByteArray(estimatedSecs), 0, array, offset, Constants.INT_SIZE); offset+=4;    
        System.arraycopy(toByteArray(dropInTeam), 0, array, offset, Constants.INT_SIZE); offset+=4;    
        System.arraycopy(toByteArray(dropInTime), 0, array, offset, Constants.INT_SIZE); offset+=4;    
        
        // copy the two teams
        System.arraycopy(toByteArray(getTeamNumber(Constants.TEAM_BLUE)), 0, array, offset, Constants.INT_SIZE); offset+=4;
        System.arraycopy(toByteArray(Constants.TEAM_BLUE), 0, array, offset, Constants.INT_SIZE); offset+=4;
        System.arraycopy(toByteArray(getScore(Constants.TEAM_BLUE)), 0, array, offset, Constants.INT_SIZE); offset+=4;
        
        for (int i=0; i<4; i++) {
            System.arraycopy(toByteArray(teams[Constants.TEAM_BLUE].getPlayers()[i].getPenalty()), 
                             0, array, offset, Constants.INT_SIZE); offset+=4;
            System.arraycopy(toByteArray(teams[Constants.TEAM_BLUE].getPlayers()[i].getSecsTillUnpenalised()), 
                             0, array, offset, Constants.INT_SIZE); offset+=4;
        }
        
        System.arraycopy(toByteArray(getTeamNumber(Constants.TEAM_RED)), 0, array, offset, Constants.INT_SIZE); offset+=4;
        System.arraycopy(toByteArray(Constants.TEAM_RED), 0, array, offset, Constants.INT_SIZE); offset+=4;
        System.arraycopy(toByteArray(getScore(Constants.TEAM_RED)), 0, array, offset, Constants.INT_SIZE); offset+=4;
        
        for (int i=0; i<4; i++) {
            System.arraycopy(toByteArray(teams[Constants.TEAM_RED].getPlayers()[i].getPenalty()), 
                             0, array, offset, Constants.INT_SIZE); offset+=4;
            System.arraycopy(toByteArray(teams[Constants.TEAM_RED].getPlayers()[i].getSecsTillUnpenalised()), 
                             0, array, offset, Constants.INT_SIZE); offset+=4;
        }
        
        
        // print out the binary array and debug   
        if (Constants.debug) {
            
            String blueString = teams[0].formString();
            String redString  = teams[1].formString();
            String dataString = "Hr:" + Constants.STRUCT_HEADER + " " + 
                                "Vr:" + Constants.STRUCT_VERSION + " " + 
                                "St:" + gameState + " " + 
                                "1st:" + firstHalf + " " + 
                                "KOTeam:" + kickOffTeam + " " + 
                                "Secs:" + estimatedSecs + " " +
                                "DropT:" + dropInTeam + " " +
                                "DropS:" + dropInTime + " " +                                
                                "(" + blueString + ") (" + redString + ")";
        
            System.out.println(dataString);             
            
            for (int i=0; i<array.length; i++) {
                if (i%4==0) { System.out.print("|"); }
                System.out.print(array[i]);
            }
            System.out.println();        
        }
        
        return array;
    }
    
    
    // this is used by the various data structure classes to 
    // convert their fields into a byte array
    private byte[] toByteArray(int value) {
        
        ByteBuffer buffer = ByteBuffer.allocate(4);
        buffer.order(Constants.NETWORK_BYTEORDER);
        
        buffer.putInt(value);
        byte bytes[] = buffer.array();
        
        return bytes;
    }
    
    
    // fuction is purely to convert header, header is assumed to be 4 bytes long
    private byte[] toByteArray(String string) {
        return string.getBytes();       
    }
    
    
    // update the drop in times
    public synchronized void updateDropInTime() {
        if (!noDropInYet) { this.dropInTime++; }
    }
        
    // change the last team that was favoured
    public synchronized void setDropInTeam(int team) {
        noDropInYet = false;
        this.dropInTeam = team;
        this.dropInTime = 0;        // reset counter on drop in change
    }
    
    // resets the drop in for a new half
    public synchronized void resetDropIn() {
        noDropInYet = true;
        this.dropInTeam = Constants.TEAM_BLUE;
        this.dropInTime = -1;
    }
    
    
    // resets all the penalties of the dogs for a new kick off, etc
    public synchronized void resetPenalties() {
        for (int x=0; x<2; x++) {
            for (int y=0; y<4; y++) {
                teams[x].getPlayers()[y].setPenalty(Constants.PENALTY_NONE);
            }
        }        
    }    
    
    
    // return the specified team (0 = blue, 1 = red)
    public synchronized TeamInfo getTeam(int team) {
        return teams[team];
    }
            
    
    // make the data structure into a certain game state
    public synchronized void setGameState(int gameState) {
        this.gameState = gameState;
    }    
    
    public synchronized int getGameState() {
        return this.gameState;
    }       
       
        
    // get the first half / second half flag
    public synchronized boolean getHalf() {
        return (firstHalf==Constants.TRUE);
    }
    
    // set the first half / second half flag
    public synchronized void setHalf(int firstHalf) {
        this.firstHalf = firstHalf;
    }
    
    // set the first half / second half flag
    public synchronized void setHalf(boolean firstHalf) {
        setHalf(firstHalf?Constants.TRUE:Constants.FALSE);
    }
    
    
    // set the number of estimated seconds remaining in the half
    // the parameter comes from the clock label
    // return 0 seconds when in overtime
    public synchronized void setEstimatedSecs(int time, boolean overTime) {       
        if (overTime == false) {
            this.estimatedSecs = time;
        } else {
            this.estimatedSecs = 0;
        }
    }
     

    // get the kick off team
    public synchronized int getKickOffTeam() {
        return kickOffTeam;
    }
    
    // set the kick off team
    public synchronized void setKickOffTeam(int kickOffTeam) {
        this.kickOffTeam = kickOffTeam;
    }
    
    
    // gets/sets for blue team number
    public synchronized int getTeamNumber(int team) {
        return teams[team].getTeamNumber();
    }
    
    public synchronized void setTeamNumber(int team, int number) {
        teams[team].setTeamNumber(number);
    }
        
    
    // get/set for blue team scores
    public synchronized int getScore(int team) {
        return teams[team].getTeamScore();
    }

    public synchronized void setScore(int team, int score) {
        teams[team].setTeamScore(score);
    }


    // get/set a penalty to a particular player
    public synchronized int getPenalty(int team, int player) {
        return teams[team].getPlayers()[player].getPenalty();
    }
    
    public synchronized void setPenalty(int team, int player, int penalty) {
        teams[team].getPlayers()[player].setPenalty(penalty);
    }
    
    
    // set the penalty time to a particular player
    public synchronized void setSecsTillUnpenalised(int team, int player, int secs) {
        teams[team].getPlayers()[player].setSecsTillUnpenalised(secs);
        
    }
}
