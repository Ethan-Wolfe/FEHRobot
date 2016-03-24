#include <FEHLCD.h>
#include <FEHServo.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>
#include <FEHServo.h>
#include <FEHBuzzer.h>

/*
* * * * Constants * * * *
*/
enum WheelID {LEFTWHEEL = 0, RIGHTWHEEL = 1};

enum DriveDirection {FORWARD = 1, BACKWARD = 0};
enum TurnDirection {RIGHT = 1, LEFT = 0};

enum FaceDirection {NORTH = 0, SOUTH = 1, EAST = 2, WEST = 3};

enum LightColor {cNONE = 0, cRED = 1, cBLUE = 2};

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
DigitalInputPin bottom_left_bump(FEHIO::P2_2);   /** CHANGE THIS **/
DigitalInputPin bottom_right_bump(FEHIO::P2_0);    /** CHANGE THIS **/
DigitalInputPin top_left_bump(FEHIO::P2_4);   /** CHANGE THIS **/
DigitalInputPin top_right_bump(FEHIO::P2_6);   /** CHANGE THIS **/

/** CDS CELL **/
AnalogInputPin CDSCell(FEHIO::P0_0);   /** CHANGE THIS **/

/** SERVO ARM **/
FEHServo longarm(FEHServo::Servo0);


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
void adjustHeadingRPS2(float heading, float motorPercent, float tolerance);
void adjustXLocationRPS(float x_coordinate, float motorPercent, FaceDirection dirFacing);
void adjustYLocationRPS(float y_coordinate, float motorPercent, FaceDirection dirFacing);
void driveStraightRPS(DriveDirection direction, float speed, float distance); //NOT IMPLEMENTED
void turn90RPS(TurnDirection direction); //NOT IMPLEMENTED
void goUpRamp();
/** DATA ACQUISTITION **/
int getHeading(); //NOT IMPLEMENTED
bool isFrontAgainstWall();
bool isBackAgainstWall();
LightColor getLightColor();
/** OTHER **/
void resetSceen();
/** DEBUG **/
char startupTest(); //NOT IMPLEMENTED
void printDebug();
void checkPorts();


/*
* * * * Functions * * * *
*/
/** MOTORS **/
void setWheelPercent(WheelID wheel, float percent) {
    switch (wheel) {
      case LEFTWHEEL:
        left_wheel.SetPercent(percent*1.06);
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
    left_wheel.SetPercent(0);
    g_left_wheel_percent = 0;
    right_wheel.SetPercent(0);
    g_right_wheel_percent = 0;
}


/** BASIC MOVEMENT **/
void driveStraight(DriveDirection direction, float speed, float seconds) {
    if (direction == FORWARD) {
        setWheelPercent(RIGHTWHEEL, speed);
        setWheelPercent(LEFTWHEEL, speed-1);
    } else {
        setWheelPercent(RIGHTWHEEL, -speed-1);
        setWheelPercent(LEFTWHEEL, -speed);
    }

    Sleep(seconds);

    stopAllWheels();
}
void driveStraight(DriveDirection direction, float speed) {
    if (direction == FORWARD) {
        setWheelPercent(RIGHTWHEEL, speed);
        setWheelPercent(LEFTWHEEL, speed-1);
    } else {
        setWheelPercent(RIGHTWHEEL, -speed-1);
        setWheelPercent(LEFTWHEEL, -speed);
    }
}

void turn(TurnDirection direction, float speed, float seconds) {
    if (direction == RIGHT) {
        setWheelPercent(RIGHTWHEEL, -speed);
        setWheelPercent(LEFTWHEEL, speed);
    } else {
        setWheelPercent(RIGHTWHEEL, speed);
        setWheelPercent(LEFTWHEEL, -speed);
    }

    Sleep(seconds);

    stopAllWheels();
}
void turn(TurnDirection direction, float speed) {
    if (direction == RIGHT) {
        setWheelPercent(RIGHTWHEEL, -speed);
        setWheelPercent(LEFTWHEEL, speed);
    } else {
        setWheelPercent(RIGHTWHEEL, speed);
        setWheelPercent(LEFTWHEEL, -speed);
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
            turn(RIGHT, motorPercent, .1);
        } else {
            turn(LEFT, motorPercent, .1);
        }

        Sleep(50);

        //Recalculate difference
        difference = heading - RPS.Heading();
        if (difference < 0) difference *= -1;

        //Failsafe
        loopCount++;
        if (loopCount > 400) {
            motorPercent *= .8;
            //encoderCounts *= .8;
            loopCount = 0;
        }
        if (motorPercent <= 4) {
            //Things have gone wrong
            break;
        }

    }
    stopAllWheels();
    printDebug();
}

void adjustHeadingRPS2(float heading, float motorPercent, float tolerance) {
    //Timeout
    int timeoutCount = 0;
    int widdleCount = 0;
    bool didTimeout = false;
    bool didError = false;
    float minSpd = 15;
    //float lastHeading = RPS.Heading();

    //Make sure we arnt going with too low of a motor power
    if (motorPercent <= minSpd) {
        motorPercent = minSpd;
    }

    //See how far we are away from desired heading
    float difference = heading - RPS.Heading();
    if (difference < 0) difference *= -1; //Absolute value of difference
    while (difference > tolerance && !didTimeout && !didError) {
        printDebug();

        //Check if we are inside the RPS zone
        if (RPS.Heading() < 0) {
            didError = true;
            break;
        }

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
            turn(RIGHT, motorPercent, .1);
        } else {
            turn(LEFT, motorPercent, .1);
        }

        Sleep(50);

        //Recalculate difference
        difference = heading - RPS.Heading();
        if (difference < 0) difference *= -1;

        //Timeout
        if (timeoutCount > 50) {
            didTimeout = true;
        } else {
            timeoutCount++;
        }

        if (difference < 30) {
            widdleCount++;
            if (widdleCount > 5) {
                didTimeout = true;
            }
        }
    }

    //Stop all the wheels and wait a few seconds to give them time to stop
    stopAllWheels();
    printDebug();
    Sleep(100);

    //Recalculate difference and recall method if we are not close enough
    if (!didError && motorPercent != minSpd) {
        difference = heading - RPS.Heading();
        if (difference < 0) difference *= -1;
        if (difference > tolerance) {
            adjustHeadingRPS2(heading, minSpd, tolerance);
        }
    }
}

void adjustXLocationRPS(float x_coordinate, float motorPercent, FaceDirection dirFacing, float tolerance) {
    //Facing east
    if (dirFacing == EAST) {
        while(RPS.X() < x_coordinate - tolerance || RPS.X() > x_coordinate + tolerance)
        {
            printDebug();

            //We are in front facing away -> move backwards
            if(RPS.X() > x_coordinate)
            {
                //pulse the motors for a short duration in the correct direction
                driveStraight(BACKWARD, motorPercent, 0.1);
            }
            //We are behind facing towards -> move forwards
            else if(RPS.X() < x_coordinate)
            {
                //pulse the motors for a short duration in the correct direction
                driveStraight(FORWARD, motorPercent, 0.1);
            }
        }
    }

    //Facing west
    if (dirFacing == WEST) {
        while(RPS.X() < x_coordinate - tolerance || RPS.X() > x_coordinate + tolerance)
        {
            printDebug();

            //We are in front facing towards -> move backwards
            if(RPS.X() > x_coordinate)
            {
                //pulse the motors for a short duration in the correct direction
                driveStraight(FORWARD, motorPercent, 0.1);
            }
            //We are behind facing away -> move forwards
            else if(RPS.X() < x_coordinate)
            {
                //pulse the motors for a short duration in the correct direction
                driveStraight(BACKWARD, motorPercent, 0.1);
            }
        }
    }
}

void adjustYLocationRPS(float y_coordinate, float motorPercent, FaceDirection dirFacing, float tolerance) {
    //Facing north
    if (dirFacing == NORTH) {
        while(RPS.Y() < y_coordinate - tolerance || RPS.Y() > y_coordinate + tolerance)
        {
            printDebug();

            //We are in front facing away -> move backwards
            if(RPS.Y() > y_coordinate)
            {
                //pulse the motors for a short duration in the correct direction
                driveStraight(BACKWARD, motorPercent, 0.1);
            }
            //We are in behind facing towards -> move forwards
            else if(RPS.Y() < y_coordinate)
            {
                //pulse the motors for a short duration in the correct direction
                driveStraight(FORWARD, motorPercent, 0.1);
            }

            Sleep(50);
        }
    }

    //Facing south
    if (dirFacing == SOUTH) {
        while(RPS.Y() < y_coordinate - tolerance || RPS.Y() > y_coordinate + tolerance)
        {
            printDebug();

            //We are in front facing towards -> move forwards
            if(RPS.Y() > y_coordinate)
            {
                //pulse the motors for a short duration in the correct direction
                driveStraight(FORWARD, motorPercent, 0.1);
            }
            //We are in behind facing away -> move backwards
            else if(RPS.Y() < y_coordinate)
            {
                //pulse the motors for a short duration in the correct direction
                driveStraight(BACKWARD, motorPercent, 0.1);
            }

            Sleep(50);
        }
    }
}

void goUpRamp() {
    //Drive to right wall
    driveStraight(FORWARD, 30);
    while (!isFrontAgainstWall());
    stopAllWheels();
    Sleep(100);

    //Pivot
    driveStraight(BACKWARD, 30, 1.5);
    Sleep(.5);
    setWheelPercent(RIGHTWHEEL, 60);
    Sleep (1.1);
    stopAllWheels();
    Sleep(.5);
    driveStraight(BACKWARD, 30);
    while (bottom_left_bump.Value());
    stopAllWheels();
    Sleep(0.5);
    setWheelPercent(RIGHTWHEEL, -20);
    while (bottom_right_bump.Value());
    stopAllWheels();
    Sleep(1.0);

    //Drive up ramp
    driveStraight(FORWARD, 45);
    while (!isFrontAgainstWall());
    stopAllWheels();
    Sleep(0.2);


    driveStraight(BACKWARD, 30, 1.5);
    Sleep(.5);
    setWheelPercent(RIGHTWHEEL, 60);
    Sleep (1.1);
    stopAllWheels();
    Sleep(.5);
    driveStraight(BACKWARD, 30);
    while (!isBackAgainstWall());
    stopAllWheels();
    Sleep(1.0);

    driveStraight(FORWARD, 30, 2.5);
    stopAllWheels();
}

/** DATA ACQUISTITION **/
bool isFrontAgainstWall() {
    return top_left_bump.Value()==false && top_right_bump.Value()==false;
}
bool isBackAgainstWall() {
    return bottom_left_bump.Value()==false && bottom_right_bump.Value()==false;
}
LightColor getLightColor() {
    if (CDSCell.Value() <= 1.1) {
        return cRED;
    } else if (CDSCell.Value() > 1.1 && CDSCell.Value() <= 1.8) {
        return cBLUE;
    } else {
        return cNONE;
    }
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
    LCD.WriteRC("RPS DATA",0,0);
    LCD.WriteRC(RPS.X(),1,0); //update the x coordinate
    LCD.WriteRC(RPS.Y(),2,0); //update the y coordinate
    LCD.WriteRC(RPS.Heading(),3,0); //update the heading

    //Print error list
}

void checkPorts() {
    setWheelPercent(RIGHTWHEEL, 15);
    Sleep(100);
    stopAllWheels();

    Sleep(1.0);

    setWheelPercent(LEFTWHEEL, 15);
    Sleep(100);
    stopAllWheels();

    Sleep(1.0);

    while (true) {
        LCD.WriteRC(top_left_bump.Value(),0,0);
        LCD.WriteRC(top_right_bump.Value(),1,0);
        LCD.WriteRC(bottom_left_bump.Value(),2,0);
        LCD.WriteRC(bottom_right_bump.Value(),3,0);
    }
}

/** TEST FUNCTIONS **/
void driveBackTest() {
    longarm.SetDegree(29);
    Sleep(500);
    driveStraight(FORWARD, 20, 4.0);
}
void lightColorTest() {
    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );
    while (true) {
        LightColor col = getLightColor();
        if (col == cRED) {
            LCD.WriteRC("RED",1,0);
        } else if (col == cBLUE) {
            LCD.WriteRC("BLUE",1,0);
        } else if (col == cNONE) {
            LCD.WriteRC("NONE",1,0);
        }
        Sleep(200);
    }
}


/*
* * * ****************************************************************** Main ************************************************************************* * * *
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

    //Setmin and Setmax for Servo
    longarm.SetMin(550);
    longarm.SetMax(2438);

    //Wait to start
    float touch_x, touch_y;
    LCD.WriteLine("BUILD ID: 22");
    LCD.WriteLine("Touch to start");
    while (!LCD.Touch(&touch_x, &touch_y));


    /*
        Performance test 4
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

    resetScreen();

    //Set initial position for longarm
    longarm.SetDegree(115);

    //Start from cds cell
    while(CDSCell.Value() > 0.9) {
        LCD.WriteLine (CDSCell.Value());
        Sleep(100);
    }


    //**** BEGIN RUN ****//


    //Drive forward slightly to get in front of dumbbell
    driveStraight(FORWARD, 30, 1.0);
    Sleep(100);

    //Turn (northeast -> east) to face east wall
    turn(RIGHT, 30, 0.6);
    adjustHeadingRPS2(0, 20, 1.0);
    Sleep(100);

    //Drive until bump against right wall in front of dumbbell
    driveStraight(FORWARD, 40);
    while (!isFrontAgainstWall());
    stopAllWheels();
    Sleep(100);

    //Backup slighty to get apart from wall then turn (east -> south) to face dumbbell
    driveStraight(BACKWARD, 30, 1.0);
    Sleep(100);
    turn(RIGHT, 30.0, 1.0);
    adjustHeadingRPS2(270, 25, 1.0);
    Sleep(50);

    //Backup
    adjustYLocationRPS(17.5, 20, SOUTH, 0.8);
    Sleep(1.0);

    //Lower longarm to dumbbell height
    longarm.SetDegree(12);
    Sleep(0.5);

    //Adjust heading to face dumbbell
    adjustHeadingRPS2(270, 20, 0.8);
    Sleep(250);

    //Drive forward until arm is under dumbbell
    adjustYLocationRPS(15, 20, SOUTH, 0.8);
    Sleep(250);

    //Raise arm to pickup dumbbell
    longarm.SetDegree(110);
    Sleep(0.5);

    //Get close to ramp
    driveStraight(BACKWARD, 30, 1.5);
    adjustYLocationRPS(25, 20, SOUTH, 0.8);
    Sleep(250);

    //Adjust heading (north -> east)
    adjustHeadingRPS2(269, 20, 0.8);
    Sleep(1.0);

    //Lift up dumbbell
    longarm.SetDegree(140);
    Sleep(0.5);

    //Drive up ramp
    /*
    goUpRamp();
    Sleep(200);
    */
    driveStraight(BACKWARD, 60, 3);
    Sleep(1.0);
    longarm.SetDegree(110);
    Sleep(1.0);
    adjustHeadingRPS2(270, 30, 1.0);
    Sleep(1.0);
    //adjustYLocationRPS(45.5, 30, SOUTH, 1.0);

    //Adjust heading (? -> east)
    adjustHeadingRPS2(0, 20, 1);
    Sleep(200);

    //Adjust x position to be in line with buttons
    adjustXLocationRPS(26.5, 20, EAST, 1.0);
    Sleep(200);

    //Turn to face buttons
    adjustHeadingRPS2(90,20,1);
    Sleep(200);

    //Drive forward to get close to buttons
    driveStraight(FORWARD, 30, 2.5);
    adjustYLocationRPS(62, 30, NORTH, 1.0);
    Sleep(200);
    adjustHeadingRPS2(90, 20, 1.0);
    Sleep(200);

    //Check button color
    LightColor buttonColor;
    buttonColor = getLightColor();
    LCD.WriteRC(CDSCell.Value(),6,0);

    if (buttonColor == cRED) {
        LCD.WriteRC("Red",5,0);

        //Adjust arm to be in line with button
        longarm.SetDegree(135);
        Sleep(1.0);

        //Adjust y position to press button and wait 5 seconds
        driveStraight(FORWARD, 15, 2);
        Sleep(5.5);
    }

    if (buttonColor == cBLUE) {
        LCD.WriteRC("Blue",5,0);

        //Adjust y position to press button and wait 5 seconds
        driveStraight(FORWARD, 15, 2);
        Sleep(5.5);

    }

    if (buttonColor == cNONE) {
        while (true) {
            LCD.WriteLine("NO COLOR FOUND");
        }
    }

    //Backup
    longarm.SetDegree(110);
    driveStraight(BACKWARD, 30, 1.5);
    Sleep(200);
    adjustHeadingRPS2(90, 20, 1.0);

    //Navigate to ramp
    adjustYLocationRPS(42, 30, NORTH, 1.0);

    //Go down ramp
    driveStraight(BACKWARD, 50, 3.0);
    stopAllWheels();
    adjustHeadingRPS2(90, 30, 1.0);
    Sleep(200);
    adjustYLocationRPS(21.5, 30, NORTH, 1.0);

    //Adjust heading and drive to end buton
    adjustHeadingRPS2(190, 30, 0.8);
    Sleep(200);
    driveStraight(FORWARD, 30, 1.5);
    Sleep(200);
    adjustHeadingRPS2(225, 30, 0.8);
    Sleep(200);
    driveStraight(FORWARD, 45);

    return 0;

}

