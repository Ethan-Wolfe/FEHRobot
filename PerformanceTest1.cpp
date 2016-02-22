#include <FEHLCD.h>
#include <FEHUtility.h>
#include <FEHMotor.h>

/*
* * * * Constants * * * *
*/
enum DriveDirection {FORWARD = 1, BACKWARD = 0};
enum TurnDirection {RIGHT = 1, LEFT = 0};


/*
* * * * Variables * * * *
*/
//FEHMotor var(FEHMotor::port, float voltage) 
FEHMotor left_wheel(FEHMotor::---, ---);
FEHMotor right_wheel(FEHMotor::---, ---);


/*
* * * * Functions * * * *
*/
void driveStraight(DriveDirection direction, float speed, float seconds) {
	if direction == FORWARD {
	    right_wheel.SetPercent(speed);
	    left_wheel.SetPercent(speed);
	} else {
	    right_wheel.SetPercent(-speed);
	    left_wheel.SetPercent(-speed);
	}
	
	Sleep(seconds);
	
	right_wheel.SetPercent(0);
	left_wheel.SetPercent(0);
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


/*
* * * * Main * * * *
*/
int main(void)
{
	//Clear the proteus screen
    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );
	
	
	
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