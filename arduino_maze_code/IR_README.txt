IR LIBRARY README

VARIABLES
	irArray: 3x3 array of integers representing the pin numbers of the IR LEDs and receivers. Order is: front, diagonal, side; right, left. Filled in top to bottom, left to right.

	calibratedArray: 3x2 array of integers representing average values of sensors over ten trials. No input is needed.

FUNCTIONS
	IR(pin ints): Constructor for class. Inputs are:
		-LED input pins; diag, side, front
		-Receiver output pins: front, side, diagonal; left, right
		All inputs are integer values in the order described above

	powerUp(ledPin): function for powering on LED attached to provided pin.

	powerDown(ledPint): function for powering off LED attached to provided pin.

	readSensor(recieverPin): function for reading sensor from pin value provided as single argument. Note: Function cannot be called until 80 microseconds after calling powerUp function.

	getAverage(ledPin, recPin): Given an input of an LED pin and a receiver pin, get the average receiver value over 10 trials.

	calibrateSensors(): For each sensor, power up the associated LED, get its average, and populate the calibratedArray.

USAGE
	This library should be instantiated once. Instantiation calls the calibrateSensors() functon to prepopulate the array. This function should called be called again after every turn of the robot to account for varying light changes.
	
	Once the IR object is created, the dispatcher interrupt will call the readSensor() function which will update the appropriate irArray entry based upon the y,x arguments to readSensor(). The irArray can then be queried at any time with reasonable certainty that it contains the most recent sensor values.

	The dispatcher will use the interreupt to call the readSensor() function for both receivers associated with the two LEDs turned on by the call to powerUp(). The pattern should be as follows; powerUp() is called for a given LED pin. Two LEDs take 80 microseconds to power on fully. After an 80 microsecond pause, the two receivers are read with readSensor() which updates the irArray appropriately. The powerDown function is then called simultaneously with the powerUp function for the next set of LEDs. After 80 microseconds total to allow this simultaneous powering, the next set of receivers are read.