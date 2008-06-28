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
 * RobotInfo.java
 *
 * Created on 10 January 2005, 12:22
 */

/**
 * RobotInfo is part of the RoboCupGameControlData data structure. The class can 
 * convert itself into a little endian byte array  for sending over a network
 *
 * @author willu@cse.unsw.edu.au shnl327@cse.unsw.edu.au
 */
public class RobotInfo {
    
    private int penalty;                    // the penalty state the robot is in (see Constants.java)
    private int secsTillUnpenalise;         // estimated seconds till unpenalised
    
    
    /** Creates a new instance of RobotInfo */
    public RobotInfo() {
    }
    
    
    // convert into a string (for data structure serialisation)
    public String formString() {
        String player = new String(penalty + "," + secsTillUnpenalise);
        return player;
    }
       
    
    // get/set for penalty state
    public void setPenalty(int penalty) {
        this.penalty = penalty;
    }
    
    public int getPenalty() {
        return this.penalty;
    }    
    
    
    // get/set for secsTillUnpenalise
    public int getSecsTillUnpenalised() {
        return this.secsTillUnpenalise;
    }
    
    public void setSecsTillUnpenalised(int secsTillUnpenalise) {
        this.secsTillUnpenalise = secsTillUnpenalise;
    }
    
}
