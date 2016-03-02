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

void printDebug() {
    //Clear screen
    resetScreen();

	LCD.WriteLine("RPS DATA:");
	LCD.Write("X: ");
	LCD.Write(RPS.X());
	LCD.Write("Y: ");
	LCD.Write(RPS.Y());
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
    while(CDSCell.Value() > .75) {
        LCD.WriteLine(CDSCell.Value());
        Sleep(500);
	}
	


}