
//Requirements
//3 floors
//3 switches in elevator to go to floor
//call button and lights on each floor
//elevator returns to ground floor after a certain period of time

//Plan
//encoder (shares axle with driver motor) count to determine floor
//global timer (T1) that resets when the elevator is sent to a floor

//Config schema
//motor that spins the winch is called 'motor'
//encoder is called 'encoder'
//call buttons are called 'callFN', where N [0,2] is the floor that the button is on
//elevator buttons are called 'eFN', where N [0,2] is the floor that the button should send the elevator to
//leds are called 'ledNK' where N [0,2] is the floor that the led is on and K [0,2] is the horizontal position of the led with 0 on the left and 2 on the right

//Part list
//(1) control board
//(1) 393 motor
//(1) encoder
//(6) bump switches
//(9) leds

/*
TODO:
finish awaitInput()
figure out ports (we will likely have problems otherwise)
finish goToFloor()
-implement routine to stop at the middle floor if the call button was pressed after we started moving but before we arrived
*/

void goToFloor(int destination); //Sends the elevator to the specified floor
int awaitInput(); // Return ID of input
void updateFloorDisplay(); //Updates the led display on every floor
int parseInput(int in); //Take inputID and return destination to send the elevator to

const int encoderValues[3] = {0, 1, 2}; //TODO: Calibrate encoder values for specific floords
int currentFloor;
const int maxWaitLength = 5; //Maximum seconds to wait for input
const int elevatorSpeed = 20; //Value from 0 to 127 that denotes the speed the motor that controls the elevator moves at

enum inputID {F0Call, F1Call, F2Call, ElevatorF0, ElevatorF1, ElevatorF2};
enum floorID {F0, F1, F2};

task main() {
	//program always starts at ground floor
	currentFloor = 0;
	while(true) { //main control loop
		int input = awaitInput(); //wait for input from the elevator
		int dest = parseInput(input); //convert sensor input to destination
		goToFloor(dest); //send elevator to the destination
	}
}

void goToFloor(int destination) {
	switch(destination) {
		//cases to determine if elevator should move up or down (or not move at all)
		case (destination == currentFloor): //elevator should stay in the same place
			return;
		case (destination < currentFloor): //elevator should move down
			bool passedMiddle = false;  //Flag to prevent unnecessary processing by constantly updating leds
			while(SensorValue[encoder] < encoderValues[destination]) { 
				motor[motor] = -1 * elevatorSpeed; //Release the winch until the encoder value reads the value of the desired floor                  
				if(!passedMiddle && //led has not already been updated
				(currentFloor - destination == 2) && //Checks if there is a middle floor
				(SensorValue[encoder] < encoderValues[destination + 1]))//Checks if the middle floor has been passed 
				{ //Check if the elevator has passed the middle floor
					currentFloor = destination + 1; //we have now passed the middle floor
					updateFloorDisplay(); //update the floor display to show this
					passedMiddle = true; //update flag so this statement doesn't unnecessarily run multiple times
				}
			}
			motor[motor] = 0; //Stop the elevator at the floor
			currentFloor = destination; //Current floor is now the destination
			return;
		case (destination > currentFloor): //elevator should move up
	}
}

int awaitInput() {
	ClearTimer(T1); //clear the timer to start waiting
	while(time1[T1] < maxWaitLength * 1000) {
		//check every switch
		//switches in elevator first, then call buttons
		if(SensorValue[eF0] == 1) { //The button inside the elevator for F0 has been pressed
			return elevatorF0;
		}
		
		//if a switch is pressed, return the associated inputID
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
			turnLEDOn(led00);  
			turnLEDOff(led01); 
			turnLEDOff(led02);

			turnLEDOn(led10);  
			turnLEDOff(led11); 
			turnLEDOff(led12);

			turnLEDOn(led20);  
			turnLEDOff(led21); 
			turnLEDOff(led22);
			return;
		case currentFloor == F1:
			turnLEDOff(led00);  
			turnLEDOn(led01); 
			turnLEDOff(led02);

			turnLEDOff(led10);  
			turnLEDOn(led11); 
			turnLEDOff(led12);

			turnLEDOff(led20);  
			turnLEDOn(led21); 
			turnLEDOff(led22);
			return;
		case currentFloor == F2:
			turnLEDOff(led00);  
			turnLEDOff(led01); 
			turnLEDOn(led02);

			turnLEDOff(led10);  
			turnLEDOff(led11); 
			turnLEDOn(led12);
			
			turnLEDOff(led20);  
			turnLEDOff(led21); 
			turnLEDOn(led22);
			return;
	}
}
