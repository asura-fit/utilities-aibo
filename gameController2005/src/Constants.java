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
 * Constants.java
 *
 * Created on 10 January 2005, 14:26
 */

/**
 * This class contains only constants 
 *
 * @author willu@cse.unsw.edu.au shnl327@cse.unsw.edu.au
 */
import java.nio.ByteOrder;

public class Constants {
    
    // TRUE/FALSE - use this instead of builtin true/false to allow for sending
    // of the data in a byte (when converted to a string)
    // used only by the first half flag
    public static final int TRUE  = 1;
    public static final int FALSE = 0;
    
    // number of bytes for an int
    public static final int INT_SIZE = 4;
    
    // number of minutes initially on the clock
    public static final int TIME_MINUTES = 10;
    
    // strings for command line arguments
    public static final String ARG_DEBUG     = "-debug";
    public static final String ARG_PORT      = "-port";
    public static final String ARG_BROADCAST = "-broadcast";
    
    // some networking constants/defaults
    public static final int       NETWORK_DATA_PORT   = 3838;
    public static final String    NETWORK_BROADCAST   = "255.255.255.255";
    public static final int       NETWORK_HEARTBEAT   = 500;   // 500ms
    public static final int       NETWORK_BURST_COUNT = 3;     // burst goes for 3 packets extra
    public static final ByteOrder NETWORK_BYTEORDER = ByteOrder.LITTLE_ENDIAN;
    
    // data structure version
    public static final int STRUCT_VERSION = 4;
    
    // data structure header string, keep this at 4 characters/bytes
    public static final String STRUCT_HEADER = "RGme";
    
    // team colours
    public static final int TEAM_BLUE = 0;
    public static final int TEAM_RED  = 1;
    
    // game states
    public static final int STATE_INITIAL  = 0;
    public static final int STATE_READY    = 1;
    public static final int STATE_SET      = 2;
    public static final int STATE_PLAYING  = 3;
    public static final int STATE_FINISHED = 4;
    
    // penalties
    public static final int PENALTY_NONE             = 0;
    public static final int PENALTY_BALL_HOLDING     = 1;
    public static final int PENALTY_GOALIE_PUSHING   = 2;
    public static final int PENALTY_PLAYER_PUSHING   = 3;
    public static final int PENALTY_ILLEGAL_DEFENDER = 4;
    public static final int PENALTY_ILLEGAL_DEFENSE  = 5;
    public static final int PENALTY_OBSTRUCTION      = 6;
    public static final int PENALTY_REQ_FOR_PICUP    = 7;
    public static final int PENALTY_LEAVING          = 8;    
    public static final int PENALTY_DAMAGE           = 9; 
    
    // long penalty time
    public static final int PENALTY_LONG_TIME  = 30;
    public static final int PENALTY_SHORT_TIME = 0;
    
    // help message
    public static final String HELP = "" +
    "Usage: java -jar GameController [-debug] [-port X] [-broadcast broadcast_address] blue_team_number red_team_number\n" +
    "* The broadcast port will default to 3838 if not specified.\n" +
    "* The broadcast address will default to 255.255.255.255 if not specified.\n" +
    "* The team numbers must be specified for the GameController to work.\n" +
    "* -debug will print debug information to the command line.\n" +
    "eg: 1) java -jar GameController 25 15\n" + 
    "    2) java -jar GameController -debug -port 9900 -broadcast 192.168.0.255 25 15\n";
    
    // debug on/off
    public static boolean debug = false;
    
    public static final String LOG_FILENAME = "GameController.log";
    
    /** Creates a new instance of Constants */
    public Constants() {
    }               
    
}
