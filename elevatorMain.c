
//Requirements
//3 floors
//3 switches in elevator to go to floor
//call button and lights on each floor
//elevator returns to ground floor after a certain period of time

//Plan
//encoder (shares axle with driver motor) count to determine floor
//global timer (T1) that resets when the elevator is sent to a floor

void goToFloor(int floorID);
int awaitInput(); // Return ID of input
void updateFloorDisplay();
int parseInput(int in); //Take input and return floorID to send the elevator to

int encoderValues[3] = {0, 1, 2}; //TODO: Calibrate encoder values for specific floords
int currentFloor;
const int maxWaitLength = 5; //Maximum seconds to wait for input

enum inputID {F0Call, F1Call, F2Call, ElevatorF0, ElevatorF1, ElevatorF2};
enum floorID {F0, F1, F2};

task main() {
	//go to ground floor and reset encoder count
	while(true) { //main control loop
		int input = awaitInput(); //wait for input from the elevator
	}
}

void goToFloor(int floorNumber) {
	ClearTimer(T1);
	switch(floorNumber) {
		//cases to determine if elevator should move up or down
	}
}

int awaitInput() {
	while(time1[T1] < maxWaitLength) {
		//check every switch
		//switches in elevator first, then call buttons
		//if a switch is pressed, return the associated inputID
	}
}

int parseInput(int in) {
	switch(in) {
		case (in == F0Call || in == ElevatorF0):
			return F0;
		case (in == F1Call || in == ElevatorF1):
			return F1;
		case (in == F2Call || in == ElevatorF2):
			return F2;
		default:
		return F0;
	}
}
