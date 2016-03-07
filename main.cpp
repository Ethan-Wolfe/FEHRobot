#include <FEHLCD.h>
#include <FEHServo.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>

//#include <string>

/*
* * * * Constants * * * *
*/
enum WheelID {LEFTWHEEL = 0, RIGHTWHEEL = 1};

enum DriveDirection {FORWARD = 1, BACKWARD = 0};
enum TurnDirection {RIGHT = 1, LEFT = 0};

enum FaceDirection {NORTH = 0, SOUTH = 1, EAST = 2, WEST = 3};

/*
* * * * Variables * * * *
*/
/** RUNTIME DATA **/
int seconds = 0;

/** WHEELS **/
//FEHMotor var(FEHMotor::port, float voltage);
FEHMotor left_wheel(FEHMotor::Motor1, 7.2);   /** CHANGE THIS **/
int g_left_wheel_percent = 0;
FEHMotor right_wheel(FEHMotor::Motor0, 7.2);    /** CHANGE THIS **/
int g_right_wheel_percent = 0;

/** BUMP SWITCHES **/
DigitalInputPin bottom_left_bump(FEHIO::P0_2);   /** CHANGE THIS **/
DigitalInputPin bottom_right_bump(FEHIO::P0_3);    /** CHANGE THIS **/
DigitalInputPin top_left_bump(FEHIO::P2_0);   /** CHANGE THIS **/
DigitalInputPin top_right_bump(FEHIO::P3_0);   /** CHANGE THIS **/

/** CDS CELL **/
AnalogInputPin CDSCell(FEHIO::P0_0);   /** CHANGE THIS **/


/*
* * * * Function prototypes * * * *
*/
/** MOTORS **/
void setWheelPercent(WheelID wheel, float percent);
void stopAllWheels();
/** BASIC MOVEMENT **/
void driveStraight(DriveDirection direction, float speed, float seconds);
void driveStraight(DriveDirection direction, float speed);
void turn(TurnDirection direction, float speed, float seconds);
void turn(TurnDirection direction, float speed);
/** ADVANCED MOVEMENT **/
void adjustHeadingRPS(float heading, float motorPercent);
void adjustXLocationRPS(float x_coordinate, float motorPercent, FaceDirection dirFacing);
void adjustYLocationRPS(float y_coordinate, float motorPercent, FaceDirection dirFacing);
void driveStraightRPS(DriveDirection direction, float speed, float distance); //NOT IMPLEMENTED
void turn90RPS(TurnDirection direction); //NOT IMPLEMENTED
/** DATA ACQUISTITION **/
int getHeading(); //NOT IMPLEMENTED
bool isFrontAgainstWall();
bool isBackAgainstWall();
/** OTHER **/
void resetSceen();
/** DEBUG **/
char startupTest(); //NOT IMPLEMENTED
void printDebug();


/*
* * * * Functions * * * *
*/
/** MOTORS **/
void setWheelPercent(WheelID wheel, float percent) {
    switch (wheel) {
      case LEFTWHEEL:
        left_wheel.SetPercent(percent);
        g_left_wheel_percent = percent;
        break;
      case RIGHTWHEEL:
        right_wheel.SetPercent(percent);
        g_right_wheel_percent = percent;
        break;
      default:
        LCD.WriteLine("setWheelPercent ERROR");
    }
}
void stopAllWheels() {
    setWheelPercent(RIGHTWHEEL, 0);
    setWheelPercent(LEFTWHEEL, 0);
}


/** BASIC MOVEMENT **/
void driveStraight(DriveDirection direction, float speed, float seconds) {
    if (direction == FORWARD) {
        setWheelPercent(RIGHTWHEEL, speed);
        setWheelPercent(LEFTWHEEL, speed-1);
    } else {
        setWheelPercent(RIGHTWHEEL, -speed);
        setWheelPercent(LEFTWHEEL, -speed+1);
    }

    Sleep(seconds);

    stopAllWheels();
}
void driveStraight(DriveDirection direction, float speed) {
    if (direction == FORWARD) {
        setWheelPercent(RIGHTWHEEL, speed);
        setWheelPercent(LEFTWHEEL, speed-1);
    } else {
        setWheelPercent(RIGHTWHEEL, -speed);
        setWheelPercent(LEFTWHEEL, -speed+1);
    }
}

void turn(TurnDirection direction, float speed, float seconds) {
    if (direction == RIGHT) {
        setWheelPercent(RIGHTWHEEL, -speed);
        setWheelPercent(LEFTWHEEL, speed+1);
    } else {
        setWheelPercent(RIGHTWHEEL, speed);
        setWheelPercent(LEFTWHEEL, -speed-1);
    }

    Sleep(seconds);

    stopAllWheels();
}
void turn(TurnDirection direction, float speed) {
    if (direction == RIGHT) {
        setWheelPercent(RIGHTWHEEL, -speed);
        setWheelPercent(LEFTWHEEL, speed+1);
    } else {
        setWheelPercent(RIGHTWHEEL, speed);
        setWheelPercent(LEFTWHEEL, -speed-1);
    }
}

/** ADVANCED MOVEMENT **/
void adjustHeadingRPS(float heading, float motorPercent, float tolerance) {
    //Turn speed failsafe
    //float encoderCounts = 3;
    int loopCount = 0;

    //See how far we are away from desired heading
    float difference = heading - RPS.Heading();
    if (difference < 0) difference *= -1; //Absolute value of difference
    //float tolerance = 0.8;
    while (difference > tolerance) {
        printDebug();

        //Find what direction we should turn to
        bool turnRight;
        if (RPS.Heading() < heading) {
            //Less than desired -> Increase degrees
            turnRight = false;
        } else {
            //Greater than desired -> Decrease degrees
            turnRight = true;
        }
        if (difference > 180) {
            turnRight = !turnRight;
        }

        //Turn
        if (turnRight) {
            turn(RIGHT, motorPercent);
        } else {
            turn(LEFT, motorPercent);
        }

        //Recalculate difference
        difference = heading - RPS.Heading();
        if (difference < 0) difference *= -1;

        //Failsafe
        loopCount++;
        if (loopCount > 150) {
            motorPercent *= .8;
            //encoderCounts *= .8;
            loopCount = 0;
        }
        if (motorPercent <= 4) {
            //Things have gone wrong
            break;
        }

    }
}

void adjustXLocationRPS(float x_coordinate, float motorPercent, FaceDirection dirFacing) {
    //Facing east
    if (dirFacing == EAST) {
        while(RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1)
        {
            printDebug();

            //We are in front facing away -> move backwards
            if(RPS.X() > x_coordinate)
            {
                //pulse the motors for a short duration in the correct direction
                driveStraight(BACKWARD, motorPercent, 0.5);
            }
            //We are behind facing towards -> move forwards
            else if(RPS.X() < x_coordinate)
            {
                //pulse the motors for a short duration in the correct direction
                driveStraight(FORWARD, motorPercent, 0.5);
            }
        }
    }

    //Facing west
    if (dirFacing == WEST) {
        while(RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1)
        {
            printDebug();

            //We are in front facing towards -> move backwards
            if(RPS.X() > x_coordinate)
            {
                //pulse the motors for a short duration in the correct direction
                driveStraight(FORWARD, motorPercent, 0.5);
            }
            //We are behind facing away -> move forwards
            else if(RPS.X() < x_coordinate)
            {
                //pulse the motors for a short duration in the correct direction
                driveStraight(BACKWARD, motorPercent, 0.5);
            }
        }
    }
}

void adjustYLocationRPS(float y_coordinate, float motorPercent, FaceDirection dirFacing) {
    //Facing north
    if (dirFacing == NORTH) {
        while(RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1)
        {
            printDebug();

            //We are in front facing away -> move backwards
            if(RPS.Y() > y_coordinate)
            {
                //pulse the motors for a short duration in the correct direction
                driveStraight(BACKWARD, motorPercent, 0.5);
            }
            //We are in behind facing towards -> move forwards
            else if(RPS.Y() < y_coordinate)
            {
                //pulse the motors for a short duration in the correct direction
                driveStraight(FORWARD, motorPercent, 0.5);
            }
        }
    }

    //Facing south
    if (dirFacing == SOUTH) {
        while(RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1)
        {
            printDebug();

            //We are in front facing towards -> move forwards
            if(RPS.Y() > y_coordinate)
            {
                //pulse the motors for a short duration in the correct direction
                driveStraight(FORWARD, motorPercent, 0.5);
            }
            //We are in behind facing away -> move backwards
            else if(RPS.Y() < y_coordinate)
            {
                //pulse the motors for a short duration in the correct direction
                driveStraight(BACKWARD, motorPercent, 0.5);
            }
        }
    }
}

/** DATA ACQUISTITION **/
bool isFrontAgainstWall() {
    return top_left_bump.Value()==false && top_right_bump.Value()==false;
}
bool isBackAgainstWall() {
    return bottom_left_bump.Value()==false && bottom_right_bump.Value()==false;
}

/** OTHER **/
void resetScreen() {
    LCD.Clear( FEHLCD::Black );
}

/** DEBUG **/
char startupTest() {
    //Test to make sure everything is reading normal values
    //Return 0 if everything is okay
    return 0;
}

void printDebug() {
    //Clear screen
    //resetScreen();

    //Print RPS data
    /*
    LCD.WriteLine("RPS DATA:");
    LCD.Write("X: ");
    LCD.WriteLine(RPS.X());
    LCD.Write("Y: ");
    LCD.WriteLine(RPS.Y());
    LCD.Write("Heading: ");
    LCD.WriteLine(RPS.Heading());
    */
    LCD.WriteRC("RPS DATA",0,0);
    LCD.WriteRC(RPS.X(),1,0); //update the x coordinate
    LCD.WriteRC(RPS.Y(),2,0); //update the y coordinate
    LCD.WriteRC(RPS.Heading(),3,0); //update the heading

    //Print error list
}



/*
* * * * Main * * * *
*/
int main(void)
{
    //Start off by clearing screen
    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );

    //Initialize RPS
    RPS.InitializeTouchMenu();

    //Call startup test to make sure everything is okay
    if (startupTest() != 0) {
        while (true) {
            LCD.WriteLine("Startup Test Failure.");
            Sleep(50);
        }
    }

    //Wait to start
    float touch_x, touch_y;
    LCD.WriteLine("Touch to start");
    while (!LCD.Touch(&touch_x, &touch_y));

    /*
        Performance test 2
    */

    /* FUNCTIONS TO USE:
    void driveStraight(FORWARD or BACKWARD, speed, seconds);  //drive for a number of seconds
    void driveStraight(FORWARD or BACKWARD, speed);  //turn until stopAllWheels();
    void stopAllWheels();
    void turn(LEFT or RIGHT, speed, seconds);    //turn for a number of seconds (turns both wheels)
    void turn(LEFT or RIGHT, speed);    //turn until stopAllWheels(); (turns both wheels)
    bool isFrontAgainstWall();   //returns true or false
    bool isBackAgainstWall();    //returns true or false
    void setWheelPercent(LEFTWHEEL or RIGHTWHEEL, percent);   //move an individual wheel
    void adjustHeadingRPS(float heading, float motorPercent);
    void adjustYLocationRPS(float y_coordinate, float motorPercent, NORTH or SOUTH or EAST or WEST);
    void adjustXLocationRPS(float x_coordinate, float motorPercent, NORTH or SOUTH or EAST or WEST);
    */

    //Start from CdS Cell
    /*
    while(CDSCell.Value() > .75) {
        LCD.WriteLine(CDSCell.Value());
        Sleep(500);
    }*/
	

    resetScreen();
	

    //Turn to face switches (northeast -> north)
	//Timing based turn
    setWheelPercent(RIGHTWHEEL, 30);
    Sleep(1.2);
    stopAllWheels();
    Sleep(500);
	

    //Drive to swtiches
	adjustYLocationRPS(y_coordinate, 30.0, NORTH);
	
	
    //Move backwards and turn (north -> east)
    driveStraight(BACKWARD, 30.0, 2.0);
    adjustHeadingRPS(0, 20.0, 0.8);


    //Drive till you are near ramp and turn  (east -> north)
	adjustXLocationRPS(X_coordinate, 30.0, EAST);
    turn(LEFT, 10.0, 1.0);
    adjustHeadingRPS(90, 10.0, 0.8);


    //Drive up ramp
    driveStraight(FORWARD, 75.0, 3.0);


    //Turn (north -> west) and drive straight to switches
    adjustHeadingRPS(180, 10.0, 1.0);
   	adjustXLocationRPS(X_coordinate, 30.0, WEST);


    //Turn (west -> south) and press swtich
	adjustHeadingRPS(270, 10.0, 1.0);


	return 0;
	
	////BONUS////
}
