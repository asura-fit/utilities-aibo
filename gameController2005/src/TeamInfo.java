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

/*
 * TeamInfo.java
 *
 * Created on 10 January 2005, 12:21
 */

/**
 * TeamInfo is part of the RoboCupGameControlData data structure. The class can 
 * convert itself into a little endian byte array for sending over a network
 *
 * @author willu@cse.unsw.edu.au shnl327@cse.unsw.edu.au
 */
public class TeamInfo {
    
    
    // each team has 4 players, ie, it requires 4 RobotInfos
    private RobotInfo[] players = {new RobotInfo(), new RobotInfo(), 
                                   new RobotInfo(), new RobotInfo()};
    private int teamNumber;
    private int teamColour;
    private int score;
    
    
    // 4 dogs, 2 fields each, plus 3 fields from this class = 11 fields
    private static final int fields = 11;    // # of fields (inc. RobotInfos)
    
    
    /** Creates a new instance of TeamInfo, initialise the 4 robots on the team */
    public TeamInfo() {
    }
    
    // constructor that sets the team colour and number
    public TeamInfo(int teamColour, int teamNumber) {        
        this.teamColour = teamColour;
        this.teamNumber = teamNumber;
    }    
    
    
    // convert the team into a string
    public String formString() {        
        String teamString = "Team#:" + teamNumber + " " +
                            "TeamCol:" + teamColour + " " + 
                            "Score:" + score + " " +
                            "p1:" + players[0].formString() + " " +
                            "p2:" + players[1].formString() + " " +
                            "p3:" + players[2].formString() + " " +
                            "p4:" + players[3].formString();
        return teamString;
    }
           
    
    // get the robots in the team, return as an array
    public RobotInfo[] getPlayers() {
        return players;
    }
    
    
    // get/set the team number
    public void setTeamNumber(int teamNumber) {
        this.teamNumber = teamNumber;
    }
    
    public int getTeamNumber() {
        return this.teamNumber;
    }    
     
    
    // get/set team colour
    public void setTeamColour(int teamColour) {
        this.teamColour = teamColour;
    }
    
    public int getTeamColour() {
        return this.teamColour;
    }   
    
        
    // get/set team score
    public void setTeamScore(int score) {
        this.score = score;
    }
    
    public int getTeamScore() {
        return this.score;
    }    
}
