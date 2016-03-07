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


    //Setmin and Setmax for Servo
    longarm.SetMin(507);
    longarm.SetMax(2438);
    //Start from CdS Cell


        resetScreen();
    longarm.SetDegree(115);

   while(CDSCell.Value() > .75) {
       Sleep(100);

    }








    //Turn to face switches (northeast -> north)
    //Timing based turn
    setWheelPercent(RIGHTWHEEL, 30);
    Sleep(1.3);
    stopAllWheels();
    Sleep(600);


    //lower long arm
    longarm.SetDegree(40);
    //Drive to swtiches
    float y_switch_bottom = 23.8;
    adjustYLocationRPS(y_switch_bottom, 30.0, NORTH);

    //Raise long arm
    longarm.SetDegree(80);


    //Move backwards and turn (north -> east)
    setWheelPercent(RIGHTWHEEL, -30);
    Sleep(1.0);
    stopAllWheels();
    Sleep(500);
    adjustHeadingRPS(345, 25.0, 1.0);
    Sleep(1.0);
    longarm.SetDegree(50);


    //Drive till you are near ramp (right bump switch) and turn  (east -> north)
    driveStraight(FORWARD, 40.0);
    Sleep(2.0);
    stopAllWheels();
    Sleep(500);
    adjustHeadingRPS(338, 15.0, 0.8); //slight adjustment
    Sleep(500);
    driveStraight(FORWARD, 40.0);
    while (top_right_bump.Value());
    stopAllWheels();

    Sleep(100);
    driveStraight(BACKWARD, 30.0, 0.25);
    adjustHeadingRPS(92, 30.0, 1.1);


    //Drive up ramp
    driveStraight(FORWARD, 90.0);
    int count = 0;
    while (RPS.Y() < 44) { //drive until we get up
        if (count > 100) { //timeout if we arnt going anywhere
            return 0;
        }
    }
    stopAllWheels();
    Sleep(50);
    adjustYLocationRPS(42, 20, NORTH);


    //Turn (north -> west) and drive straight to switches
    adjustHeadingRPS(177, 20.0, 1.0);
    driveStraight(FORWARD, 40.0, 2.3);
    adjustXLocationRPS(10, 30, WEST);


    //Turn (west -> south) and press switch
    turn(LEFT, 60, 0.6);
    Sleep(500);

    //Drive untill borh front bumpswitches are pressed
    driveStraight(FORWARD, 40);
    while(!isFrontAgainstWall()){}
    stopAllWheels();

    //Back up
    driveStraight(BACKWARD, 40, 1.2);

    //Lower Servo
    longarm.SetDegree(15);
    //Drive into switch
    driveStraight(FORWARD,20,.8);
    Sleep(1.0);
    driveStraight(BACKWARD,20,.5);
    longarm.SetDegree(80);
    //END PERFORMANCE TEST (GOING FOR BONUS)-------------------------------
    turn(LEFT, 60, 0.6);
    Sleep (1.0);
    driveStraight(FORWARD, 40, 3.4);
    Sleep (1.0);
    turn(LEFT, 60, .6);
    Sleep(1.0);
    driveStraight(FORWARD, 40, 4.0);






    return 0;

    ////BONUS////
}