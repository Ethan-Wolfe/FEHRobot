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