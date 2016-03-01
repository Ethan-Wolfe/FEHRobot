#include <FEHLCD.h>
#include <FEHServo.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>

//#include <string>

/*
* * * * Constants * * * *
*/
enum WheelID {LEFTWHEEL = 0, RIGHTWHEEL = 1};

enum DriveDirection {FORWARD = 1, BACKWARD = 0};
enum TurnDirection {RIGHT = 1, LEFT = 0};


/*
* * * * Variables * * * *
*/
/** WHEELS **/
//FEHMotor var(FEHMotor::port, float voltage);
FEHMotor left_wheel(FEHMotor::Motor1, 7.2);   /** CHANGE THIS **/
int g_left_wheel_percent = 0;
FEHMotor right_wheel(FEHMotor::Motor0, 7.2);    /** CHANGE THIS **/
int g_right_wheel_percent = 0;

/** BUMP SWITCHES **/
DigitalInputPin bottom_left_bump(FEHIO::P0_0);   /** CHANGE THIS **/
DigitalInputPin bottom_right_bump(FEHIO::P1_2);    /** CHANGE THIS **/
DigitalInputPin top_left_bump(FEHIO::P2_0);   /** CHANGE THIS **/
DigitalInputPin top_right_bump(FEHIO::P3_0);   /** CHANGE THIS **/

/** CDS CELL **/
AnalogInputPin CDSCell(FEHIO::P1_3);   /** CHANGE THIS **/


/*
* * * * Function prototypes * * * *
*/
/** MOTORS **/
void setWheelPercent(WheelID wheel, float percent);
void stopAllWheels();
/** BASIC MOVEMENT **/
void driveStraight(DriveDirection direction, float speed, float seconds);
void driveStraight(DriveDirection direction, float speed, float distance); //NOT IMPLEMENTED
void driveStraight(DriveDirection direction, float speed);
void turn(TurnDirection direction, float speed, float seconds);
void turn(TurnDirection direction, float speed);
void turn90(TurnDirection direction); //NOT IMPLEMENTED
/** DATA ACQUISTITION **/
int getHeading(); //NOT IMPLEMENTED
bool isFrontAgainstWall();
bool isBackAgainstWall();
/** OTHER **/
void resetSceen();
/** DEBUG **/
char startupTest(); //NOT IMPLEMENTED
void printDebug(); //NOT IMPLEMENTED


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

/** DATA ACQUISTITION **/
bool isFrontAgainstWall() {
    return top_left_bump.Value()==false && top_right_bump.Value()==false;
}
bool isBackAgainstWall() {
    return bottom_left_bump.Value()==false && bottom_right_bump.Value()==false;
}

/** OTHER **/
void resetSceen() {
    LCD.Clear( FEHLCD::Black );
}

/** DEBUG **/
char startupTest() {
    //Test to make sure everything is reading normal values
    //Return 0 if everything is okay
    return 0;
}

/*
void printDebug() {
    //Clear screen
    resetScreen();

    //Declaring display content
    int length = 2;
    std::string debugString [length] = {
        "Motor_Left: "+std::to_string(g_left_wheel_percent),
        "Motor_Right: "+std::to_string(right_wheel_percent)
    };

    //Display
    for (int i=0; i<length; i++) {
        LCD.WriteLine(debugString[i]);
    }
}
*/




/*
* * * * Main * * * *
*/
int main(void)
{
    //Start off by clearing screen
    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );

    //Call startup test to make sure everything is okay
    /*
    if (startupTest() != 0) {
        while (true) {
            LCD.WriteLine("Startup Test Failure.");
        }
    }
    */


    /*
        Performance test 1 (big ramp)
    */

    /* FUNCTIONS TO USE:
    driveStraight(FORWARD or BACKWARD, speed, seconds);  //drive for a number of seconds
    driveStraight(FORWARD or BACKWARD, speed);  //turn until stopAllWheels();
    stopAllWheels();
    turn(LEFT or RIGHT, speed, seconds);    //turn for a number of seconds (turns both wheels)
    turn(LEFT or RIGHT, speed);    //turn until stopAllWheels(); (turns both wheels)
    isFrontAgainstWall();   //returns true or false
    isBackAgainstWall();    //returns true or false
    setWheelPercent(LEFTWHEEL or RIGHTWHEEL, percent);   //move an individual wheel
    */

    //Start from CdS Cell
    while(CDSCell.Value() > .75)   /** CHANGE THIS **/
{
        LCD.WriteLine(CDSCell.Value());
        Sleep(500);
}
    //Drive straight to get in front of the dumbbell
    float speed1 = 60;  /** CHANGE THIS **/
    float seconds1 = .6;   /** CHANGE THIS **/
    driveStraight(FORWARD, speed1, seconds1);
    Sleep(400);



    //Turn right (facing northeast -> facing east)
    float speed2 = 60;   /** CHANGE THIS **/
    float seconds2 = 0.37;   /** CHANGE THIS **/
    turn(RIGHT, speed2, seconds2);
    Sleep(400);

    //Drive right until we are hit right wall
    float speed3 = 60;   /** CHANGE THIS **/
    //float seconds3 = 0;
    driveStraight(FORWARD, speed3);
    while (!isFrontAgainstWall());
    stopAllWheels();
    Sleep(400);

    float speed6 = 60;  /** CHANGE THIS **/
    float seconds6 = .3;   /** CHANGE THIS **/
    driveStraight(BACKWARD, speed6, seconds6);
    Sleep(400);




    //Turn left to face the ramp (facing east -> facing north)
    float speed4 = 60;   /** CHANGE THIS **/
    float seconds4 = 0.6;   /** CHANGE THIS **/
    turn(LEFT, speed4, seconds4);
    Sleep(1000);

    //Drive straight up the ramp
    float speed5 = 75;   /** CHANGE THIS **/
    float seconds5 = 5.0;   /** CHANGE THIS **/
    driveStraight(FORWARD, speed5, seconds5);
    Sleep(400);
}