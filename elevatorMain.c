
//Requirements
//3 floors
//3 switches in elevator to go to floor
//call button and lights on each floor
//elevator returns to ground floor after a certain period of time

//Plan
//encoder (shares axle with driver motor) count to determine floor
//global timer (T1) that resets when the elevator is sent to a floor

//Config
//motor that spins the winch is called 'motor'
//encoder is called 'encoder'
//call buttons are called 'callFN', where N [0,2] is the floor that the button is on
//elevator buttons are called 'eFN', where N [0,2] is the floor that the button should send the elevator to
//leds are called 'ledN' where N [0,2] is the horizontal position of the led (0 - 1 - 2)

//Part list
//(1) control board
//(1) 393 motor
//(1) encoder
//(6) bump switches
//(9) leds
// Wire to connect leds

/*
TODO:
-calibration program for encoder values
*/

void goToFloor(int destination); //Sends the elevator to the specified floor
int awaitInput();                //Return ID of input
void updateFloorDisplay();       //Updates the led display on every floor
int parseInput(int in);          //Take inputID and return destination to send the elevator to

const int encoderValues[3] = {0, 1, 2}; //TODO: Calibrate encoder values for specific floors
int currentFloor;                       //Stores the floor # the elevator is on (or has most recently passed)
const int maxWaitLength = 5;            //Maximum seconds to wait for input
const int elevatorSpeed = 20;           //Value from 0 to 127 that denotes the speed the motor that controls the elevator moves at
const int waitAtFloorLength = 2;        //Seconds to wait at the middle floor if the button was pressed before continuing to the next floor

enum inputID {F0Call, F1Call, F2Call, ElevatorF0, ElevatorF1, ElevatorF2};
enum floorID {F0, F1, F2};

task main() {
	//program always starts at ground floor
	currentFloor = 0;
	while(true) {                         //main control loop
		int input = awaitInput();     //wait for input from the elevator
		int dest = parseInput(input); //convert sensor input to destination
		goToFloor(dest);              //send elevator to the destination
	}
}

void goToFloor(int destination) {
	switch(destination) {
		//cases to determine if elevator should move up or down (or not move at all)
		case (destination == currentFloor):                                                                                         //elevator should stay in the same place
			return;
		case (destination < currentFloor):                                                                                          //elevator should move down
			bool passedMiddle = false;                                                                                          //Flag to prevent unnecessary processing by constantly updating leds
			bool stopAtMiddle = false;                                                                                          //Flag to determine if the elevator should stop at the middle floor
			bool middleExists = ((currentFloor - destination) == 2);                                                            //Determines if there is a middle floor
			while(SensorValue[encoder] > encoderValues[destination]) { 
				motor[motor] = -1 * elevatorSpeed;                                                                          //Release the winch until the encoder value reads the value of the desired floor
				stopAtMiddle = (middleExists && (SensorValue[callF1] == 1 || SensorValue[eF1] == 1)) ? true : stopAtMiddle; //Sets the flag to true if either of the buttons for the middle floor is pressed or keeps it the same if no button is pressed
				if(!passedMiddle &&                                                                                         //led has not already been updated
				    middleExists &&                                                                                         //Checks if there is a middle floor
				(SensorValue[encoder] < encoderValues[destination + 1]))                                                    //Checks if the middle floor has been passed 
				{                                                                                                           //The above checks if the elevator has passed the middle floor
					currentFloor = destination + 1;                                                                     //update currentFloor to reflect that the elevator has passed the middle floor
					updateFloorDisplay();                                                                               //update the floor display to show this
					passedMiddle = true;                                                                                //update flag so this statement doesn't unnecessarily run multiple times
					if(stopAtMiddle) {
						motor[motor] = 0;                                                                           //Stop the elevator at the floor
						wait(waitAtFloorLength);                                                                    //Wait for passengers to get on
					}
				}
			}
			motor[motor] = 0;                                                                                                   //Stop the elevator at the floor
			currentFloor = destination;                                                                                         //Current floor is now the destination
			return;
		case (destination > currentFloor):                                                                                          //elevator should move up
			bool passedMiddle = false;                                                                                          //Flag to prevent unnecessary processing by constantly updating leds
			bool stopAtMiddle = false;                                                                                          //Flag to determine if the elevator should stop at the middle floor
			bool middleExists = ((destination - currentFloor) == 2);                                                            //Determines if there is a middle floor
			while(SensorValue[encoder] < encoderValues[destination]) { 
				motor[motor] = elevatorSpeed;                                                                               //Run the motor until the encoder value reads the value of the desired floor                  
				stopAtMiddle = (middleExists && (SensorValue[callF1] == 1 || SensorValue[eF1] == 1)) ? true : stopAtMiddle; //Sets the flag to true if either of the buttons for the middle floor is pressed or keeps it the same if no button is pressed
				if(!passedMiddle &&                                                                                         //led has not already been updated
				    middleExists &&                                                                                         //Checks if there is a middle floor
				(SensorValue[encoder] > encoderValues[destination - 1]))                                                    //Checks if the middle floor has been passed 
				{                                                                                                           //The above checks if the elevator has passed the middle floor
					currentFloor = destination - 1;                                                                     //we have now passed the middle floor
					updateFloorDisplay();                                                                               //update the floor display to show this
					passedMiddle = true;                                                                                //update flag so this statement doesn't unnecessarily run multiple times
					if(stopAtMiddle) {
						motor[motor] = 0;                                                                           //Stop the elevator at the floor
						wait(waitAtFloorLength);                                                                    //Wait for passengers to get on
					}
				}
			}
			motor[motor] = 0;                                                                                                   //Stop the elevator at the floor
			currentFloor = destination;                                                                                         //Current floor is now the destination
			return;
	}
}

int awaitInput() {
	ClearTimer(T1); //clear the timer to start waiting
	while(time1[T1] < maxWaitLength * 1000) {
		//check every switch, starting with the elevator buttons, then the call buttons
		if(SensorValue[eF0] == 1) {           
			return elevatorF0;
		}
		else if(SensorValue[eF1] == 1) {
			return elevatorF1;
		}
		else if(SensorValue[eF2] == 1) {
			return elevatorF2;
		}
		else if(SensorValue[callF0] == 1) {
			return F0call;
		}
		else if(SensorValue[callF1] == 1) {
			return F1call;
		}
		else if(SensorValue[callF2] == 1) {
			return F2call;
		}
	}
	return F0; //If the timer expires and a return statement in the loop has not been triggered, return F0 to send the elevator to the ground floor
}

int parseInput(int in) {
	switch(in) {
		case (in == F0Call || in == ElevatorF0): //User wants to send the elevator to F0
			return F0;
		case (in == F1Call || in == ElevatorF1): //User wants to send the elevator to F1
			return F1;
		case (in == F2Call || in == ElevatorF2): //User wants to send the elevator to F2
			return F2;
		default:
		return F0;
	}
}

void updateFloorDisplay() {
	switch(currentFloor) {
		case currentFloor == F0:  //if the elevator is on ground floor
			turnLEDOn(led0);  
			turnLEDOff(led1); 
			turnLEDOff(led2);
			return;
		case currentFloor == F1:
			turnLEDOff(led0);  
			turnLEDOn(led1); 
			turnLEDOff(led2);
			return;
		case currentFloor == F2:
			turnLEDOff(led0);  
			turnLEDOff(led1); 
			turnLEDOn(led2);
			return;
	}
}
