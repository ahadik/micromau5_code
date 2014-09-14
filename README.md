#MicroMau5
##Flood Fill Approach to the IEEE Micro-Mouse Competion
The IEEE Micr-Mouse competition is a world wide collection of events in which college teams create a robot that can navigate a maze as quickly as possible. Example specifications for the competition can be found at [the IEEE website](http://sites.ieee.org/r1/files/2013/03/2013-Region-1-Micromouse-Competition-Rules.pdf). Every year, the Brown University IEEE Student Organization hosts a regional Micro-Mouse competition. In 2014, Tristan Paine, Kevin Jung and myself, Alexander Hadik, entered with our MicroMau5 robot. While the technical details of the robot are explained on my website [www.alexhadik.com](http://www.alexhadik.com), I am releasing our code and a brief explanation of our approach on this GitHub repository. This code is open for use and modification with due credit given. Any questions, comments, or notes may be passed along to myself at [alexander_hadik@brown.edu](mailto:alexander_hadik@brown.edu).

###The Approach
Our team chose to base our maze solving algorithms on the flood fill approach, which is a common technique in the field of maze solving robots. The flood fill algorithm begins with the initial assumption that there are no walls in the maze, and assigns a distance to each cell that is a 'best guess' at the cell's distance from the goal. With no walls in the center, an initial distance can be easily calculated as:
	
	<xMid, yMid> := coordiante of goal
	<x,y> := current coordinate
	dist := abs(xMid-x)+abs(yMid-y)
	
The general principal, and inspiration for the name, is to model how water might 'flood' to the center of this maze where the goal coordinate is the lowest point at 0. The object then is to assign an accurate 'weight' to each cell that determines that cells 'height' or distance from the goal coordinate. With all cells properly calculated, one can flow from any cell downhill  and be certain to reach the center in the optimal (shortest distance) time. I'll leave the proof of this last claim to you.

A major with this strict implementation is namely that the shortest distance may not be the fastest route for a robot to traverse. We'll touch on this later.

###Flood Fill in English
The flood fill algorithm begins with the agent (the robot) in the corner of the maze, and the target (the goal cell) in the center. The maze is presumed to contain no walls and every cell has been assigned a preliminary weight using the method described above. Before beginning, the agent can fill in some information about the maze, such as the walls that surround its starting cell. After filling in these walls, the agent moves forward one cell and updates its maze in memory with new found walls. The agent then checks if the value of the new cell agrees or disagrees with the current version of the in-memory maze. Namely, the cell must be either one greater or one less than all surrounding cells, and there must be an accessible cell one less than the current cell. The only cell that violates these conditions is the goal cell.

If the agent finds the cell is inconsistent with the maze, it adds this cell to a list of cells to check and update. The agent changes the value of the cell to a value that is consistent and removes the cell from the lis. Then the agent repeats this check, add and change process on the next cell in the list until the list is empty. Once the list is empty, the agent moves to the next most ideal cell, updates its in-memory maze based on newly discovered walls, and repeats the cell verification process.

###Flood Fill in Pseudocode
This algorithm is best implemented with a stack of cells to verify that the agent adds to and pops from. Micro-Mouse mazes are typically 16x16 squares so we'll work with a maze of those dimensions. We also use two mazes each storing integer values. Bit wise equivalences between integers stored in wallmaze are used for the maze construction and integer distance values are used to represent distance to the goal in distmaze.

	distmaze := int[16][16]
	wallmaze := int[16][16]
	goal := <8,8>
	start := <0,0>
	checks := stack of cells to verify
	all cells in wallmaze := empty
	for cell in distmaze
		cell := shortest dist to goal	
	checks.push(start)
	while(start != goal)
		while(stack !empty)
			cellCheck := checks.pop
			if cellCheck.value isn't 1 greater than accessible neighbors
				minVal := minimal value of accessible neighbors of cellCheck
				cellCheck := minVal++
				for neighbor in cellCheck neighbors
					cellCheck.push(neighbor)
		advance to next ideal neighbor
	return ideal path

###Implementation
Included in this repository are two implementations of the flood fill algorithm. The first is a Java implementation found in `/java_maze_code` and the second is a C implementation found in `/arduino_maze_code`.

####Java Implementation
I originally wrote this algorithm in Java because that's what I was best equipped to code with on an airplane without an internet connection. I simply wanted to work out the kinks of the algorithm without worrying about learning C, which I didn't know. This proved to be very helpful and ultimately led to my web visualizer [Mau5Maze](https://github.com/ahadik/Mau5Maze). The implementation consists of four classes:

-	`MazeGenerator`: Recursively generate mazes for testing purposes.
-	`MazeSolver`: Solve created mazes using the Flood Fill approach.
- 	`Parser`: Parse mazes stored in text files into `MazeSolver`
-	`Render`: Print mazes in various states

A Java call to `MazeGenerator` itself will print a random maze to standard out, which can be useful for saving collections of mazes to disk. A call to `MazeSolver` with integer inputs itself instantiates an instance of a `MazeGenerator` to generate a random maze of the specified size. A `Render` instance is instantiated in all cases to print the maze to standard out.

####Arduino Implementation in C
This algorithm was ultimately created for use on our MicroMau5, so I did implement the algorithm in C. The structure and function is mostly the same, however I made a few improvements including using a struct for cells instead of pure integers to allow for more consise integration of arrays tracking distances versus maze formation. This code can be loaded onto an Arduino using the `micromau5/micromau5.ino` file. Also included in this repository are all the libraries and code we used to drive the robot itself, such as the motors, encoders, IR sensors and gyroscope. Again, further detail on these aspects of the project can be found at [my website](http://www.alexhadik.com).