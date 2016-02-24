#include <FEHLCD.h>
#include <FEHUtility.h>
#include <FEHMotor.h>

#include <string>

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
//FEHMotor var(FEHMotor::port, float voltage) 
FEHMotor left_wheel(FEHMotor::---, ---);
int g_left_wheel_percent = 0;
FEHMotor right_wheel(FEHMotor::---, ---);
int g_right_wheel_percent = 0;

/** BUMP SWITCHES **/
DigitalInputPin bottom_left_bump(FEHIO::---);
DigitalInputPin bottom_right_bump(FEHIO::---);
DigitalInputPin top_left_bump(FEHIO::---);
DigitalInputPin top_right_bump(FEHIO::---);


/*
* * * * Function prototypes * * * *
*/
/** MOTORS **/
void setWheelPercent(WheelID wheel, float percent);
void stopAllWheels();
/** BASIC MOVEMENT **/
void driveStraight(DriveDirection direction, float speed, float seconds);
void driveStraight(DriveDirection direction, float distance);
void turn(TurnDirection direction, float speed, float seconds);
void turn90(TurnDirection direction);
/** DATA ACQUISTITION **/
int getHeading();
/** OTHER **/
void resetSceen();
/** DEBUG **/
char startupTest();
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
	if direction == FORWARD {
		setWheelPercent(RIGHTWHEEL, speed);
		setWheelPercent(LEFTWHEEL, speed);
	} else {
		setWheelPercent(RIGHTWHEEL, -speed);
		setWheelPercent(LEFTWHEEL, -speed);
	}
	
	Sleep(seconds);
	
	stopAllWheels();
}

void turn(TurnDirection direction, float speed, float seconds) {
	if direction == RIGHT {
		setWheelPercent(RIGHTWHEEL, speed);
		setWheelPercent(LEFTWHEEL, -speed);
	} else {
		setWheelPercent(RIGHTWHEEL, -speed);
		setWheelPercent(LEFTWHEEL, speed);
	}
	
	Sleep(seconds);
	
	stopAllWheels();
}

/** DATA ACQUISTITION **/

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


/*
* * * * Main * * * *
*/
int main(void)
{
	//Start off by clearing screen
	LCD.SetFontColor( FEHLCD::White );
	resetScreen();
	
	//Call startup test to make sure everything is okay
	if (startupTest() != 0) {
		while (true) {
			LCD.WriteLine("Startup Test Failure.")
		}
	}
	
	
	
	/*
		Performance test 1
	*/
	
	//Drive straight to get in front of the dumbbell 
	float speed1 = 0;
	float seconds1 = 0;
	driveStraight(FORWARD, speed1, seconds1);
	sleep(10);
	
	//Turn right (facing northeast -> facing east)
	float speed2 = 0;
	float seconds2 = 0;
	turn(RIGHT, speed2, seconds2);
	sleep(10);
	
	//Drive right until we are in front of the ramp
	float speed3 = 0;
	float seconds3 = 0;
	driveStraight(FORWARD, speed3, seconds3);
	sleep(10);
	
	//Turn left to face the ramp (facing east -> facing north)
	float speed4 = 0;
	float seconds4 = 0;
	turn(LEFT, speed4, seconds4);
	sleep(10);
	
	//Drive straight up the ramp
	float speed5 = 0;
	float seconds5 = 0;
	driveStraight(FORWARD, speed5, seconds5);
	sleep(10);

}