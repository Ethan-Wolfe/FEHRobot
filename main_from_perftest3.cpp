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
    LCD.WriteLine("Touch to start");
    while (!LCD.Touch(&touch_x, &touch_y));



    /*
        Performance test 3
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
    while(CDSCell.Value() > .8) {
        LCD.WriteLine (CDSCell.Value());
       Sleep(100);
    }



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
    adjustYLocationRPS(19, 20, SOUTH, 1.0);
    Sleep(1.0);

    //Lower longarm
    longarm.SetDegree(20);
    Sleep(0.5);

    //Adjust heading
    adjustHeadingRPS2(270, 20, 0.8);
    Sleep(250);

    //Drive forward until arm is under dumbbell
    adjustYLocationRPS(15.3, 20, SOUTH, 0.8);
    Sleep(250);

    //Raise arm to pickup dumbbell
    longarm.SetDegree(85);
    Sleep(0.5);

    //Turn (south -> north) to face ramp
    adjustHeadingRPS2(90, 20, 0.8);
    Sleep(250);

    //Get close to ramp
    driveStraight(FORWARD, 30, 1.5);
    adjustYLocationRPS(20, 20, NORTH, 0.8);
    Sleep(250);

    //Adjust heading
    adjustHeadingRPS2(0, 20, 1);
    Sleep(200);

    //Drive up ramp
    goUpRamp();

    //Drive until rps zone
    driveStraight(FORWARD, 40);
    while (RPS.Heading() < 0);
    Sleep(200);

    //Adjust heading
    adjustHeadingRPS2(177, 20, 1.0);

    //Drive till we hit a wall
    driveStraight(FORWARD, 40);
    while (!isFrontAgainstWall());
    stopAllWheels();
    Sleep(250);

    //Backup
    driveStraight(BACKWARD, 30, 1.05);
    Sleep(250);

    //Turn (north)
    turn(RIGHT, 30, 1.35);
    Sleep(1.0);

    //Drop arm
    longarm.SetDegree(38);

    //Back up and leave supplies behind
    driveStraight(BACKWARD, 15, 1.6);




    //BONUS
    longarm.SetDegree(47);
    turn(RIGHT, 30, 1.1);
    driveStraight(FORWARD, 30);
    while (RPS.X() < 22);
    stopAllWheels();
    Sleep(300);
    driveStraight(FORWARD, 15);
    while (RPS.X() < 26.2);
    stopAllWheels();
    Sleep(300);

    adjustHeadingRPS2(90, 25, .9);
    Sleep(300);

    driveStraight(BACKWARD, 40, 5.0);

    return 0;
}