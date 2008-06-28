
        RoboCup 2003 Sony Legged Robot League - Game control package

                                                                2004.03.12


These samples describe:
  1) How to connect the robots only using "HOSTGW.CFG" and "CONNECT.CFG".
       - without any additional object(mediator) on the host PC.
  2) The GameController.
       - The data definition of the messages at game start/stop, etc
       - Which port number should be reserved for the GameController.
       - A sample object in AIBO that receives its messages.


1) InterRobotComm
  - This directory will be used for each team playing the game.
    The only thing you can do with this directory is to edit the
    two files: "HOSTGW.CFG" and "CONNECT.CFG".
  - If you want to connect the service "Object.Service.Data.S"
    in robot A(IP-address=192.168.117.100) and the service
    "Object.Service.Data.O" in robot B(IP-address=192.168.117.101),
    the "HOSTGW.CFG" will be
    ----------
    # Service Name            port    robot
    RobotA.Service.Data.S     2000    192.168.117.100
    RobotB.Service.Data.S     2000    192.168.117.101
    RobotA.Service.Data.O     2001    192.168.117.100
    RobotB.Service.Data.O     2001    192.168.117.101
    ----------

    and the "CONNECT.CFG" will be
    ----------
    RobotA.Service.Data.S     RobotB.Service.Data.O
    RobotB.Service.Data.S     RobotA.Service.Data.O
    ----------

    assuming that the "MS/OPEN-R/MW/CONF/ROBOTGW.CFG" on the robot be
    ----------
    # Service Name                              port     parameters if any
    Object.Service.Data.O                       2000
    Object.Service.Data.S                       2001

    # Game Controller
    RoboCup.Control.RoboCupGameControlData.S    1070
    ----------

  - Please test with your objects.


2) RoboCupGameController
 - This directory will give you an idea, how the games go on.
   Please refer to the header files in the 'include' directory.
 - Every time the operator of the GameController presses a button
   on the Controller, a message will be sent to the specified 
   robot(s).
 - In order to receive theses messages, the robots must listen at
   port #1070. They should react to the messages as described in 
   the rules:
     Ready    : The robots have to go (back) to the start position 
                and wait there. They can check the 'Kick-off' flag 
                to decide the position. By comparing the new score
                with the previous one, they can determine the 
                reason for the kick-off-restart (win a goal/lose a
                goal/game stuck)
     Set      : Stop walking and wait for kick-off. All robots not
                at legal positions will be placed manually by the
                assistant referees. Heads and tails are allowed 
                moving.
     Playing  : Start playing (from Set or Penalized).
     Penalized: Stop moving. The robot will be picked-up. When it 
                is returned to the field, it will receive Playing.
     Finished : The game (half) is over.

 - Note that there may be a case in which a wrong message is sent to 
   the robots, because of an operator error. In such a case, the 
   "correct" message will be sent again as soon as possible...

 - For the receiver sample inside the robot, please refer to 
   "SamplePlayer'.

 - To run RoboCupGameController, there are two RP directories.
   One [RP/host/] is for the real game, another [RP2/host/] is
   just an example in which all objects (including SamplePlayer) run 
   on the PC (but the player is alone).
 - At the RP/host/ directory, RoboCupIPConfigurator can be used to 
   edit HOSTGW.CFG. This is a simple executable independent from 
   OPEN-R. Run it before start-rp-openr.


ChangeLog:

2004-04-07
 - Robot numbers are now 1-4
 - Loading resets state "finished" to "initial"
 - Switching team color or kickoff resets states after "ready" to "ready"
 - Label of "obstruction" button shows correct time
 - Penalty time is counted down
 - Team color and kickoff are loaded from a file

2004-03-12
 - Follow latest rules, converted to GTK2.

2003-07-04
 - RoboCupGameController/RoboCupGameController/Penalty.cc: follow latest rules
 - RoboCupGameController/RoboCupIPConfigurator/Gui.cc: enrich UI

2003-05-12
 - include/RoboCupGameControlData.h:
 - RoboCupGameController/RoboCupGameController/Penalty.cc:
 - RoboCupGameController/GUI.cc:
   add "request for pick-up" penalty.
   remove unused buttons ("all member" and "ping").
   fix logging bug.

2003-04-17
 - RoboCupGameController/GUI.cc: player's number is continuous between teams
 - RoboCupGameController/Penalty.cc: Obstruction penalty time 30 -> 0 sec

2003-02-25
 - RoboCupGameController/RP2/host/: a full PC example directory [new]
 - RoboCupGameController/SamplePlayer/host/Makefile: changed INSTALLDIR
 - README.txt: added a few lines

2003-02-18
 - Ported to `Remote Processing OPEN-R' from `OPEN-R Linux Environment'.

2002-06-13
 - RoboCupManager: improvement usability (renew user interface)
 - RoboCupIPConfigurator: support null IP address (don't try to connect)

2002-06-07
 - RoboCupManager: add logging feature (>> game.log)
 - RoboCupManager: penalty counting not only 'illigal defender'
   but also all penalties

2002-05-14
 - Initial version.
