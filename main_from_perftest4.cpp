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
        longarm.SetDegree(125);
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
}