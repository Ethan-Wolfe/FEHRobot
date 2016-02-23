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
int left_wheel_percent = 0;
FEHMotor right_wheel(FEHMotor::---, ---);
int right_wheel_percent = 0;


/*
* * * * Function prototypes * * * *
*/
/** MOTORS **/
void setWheel(WheelID wheel, float percent);
void stopAllWheels();
/** MOVEMENT **/
void driveStraight(DriveDirection direction, float speed, float seconds);
void turn(TurnDirection direction, float speed, float seconds);
/** DATA ACQUISTITION **/
/** OTHER **/
void resetSceen();
/** DEBUG **/
void printDebug();


/*
* * * * Functions * * * *
*/
/** MOTORS **/
void setWheel(WheelID wheel, float percent) {
	switch (wheel) {
	  case LEFTWHEEL:
	    left_wheel.SetPercent(percent);
		left_wheel_percent = percent;
	    break;
	  case RIGHTWHEEL:
	    right_wheel.SetPercent(percent);
		right_wheel_percent = percent;
	    break;
	  default:
	  	LCD.WriteLine("setWheel ERROR");
	}
}
void stopAllWheels() {
	setWheel(RIGHTWHEEL, 0);
	setWheel(LEFTWHEEL, 0);
}


/** MOVEMENT **/
void driveStraight(DriveDirection direction, float speed, float seconds) {
	if direction == FORWARD {
		setWheel(RIGHTWHEEL, speed);
		setWheel(LEFTWHEEL, speed);
	} else {
		setWheel(RIGHTWHEEL, -speed);
		setWheel(LEFTWHEEL, -speed);
	}
	
	Sleep(seconds);
	
	stopAllWheels();
}

void turn(TurnDirection direction, float speed, float seconds) {
	if direction == RIGHT {
	    right_wheel.SetPercent(speed);
	    left_wheel.SetPercent(-speed);
	} else {
	    right_wheel.SetPercent(-speed);
	    left_wheel.SetPercent(speed);
	}
	
	Sleep(seconds);
	
	right_wheel.SetPercent(0);
	left_wheel.SetPercent(0);
}

/** DATA ACQUISTITION **/

/** OTHER **/
void resetSceen() {
    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );
}

/** DEBUG **/
void printDebug() {
	//Clear screen
	resetScreen()
	
	//Declaring display content
	int length = 2;
	std::string debugString = {
		"Motor_Left: "+std::to_string(left_wheel_percent),
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
	resetScreen();
	
	
	
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