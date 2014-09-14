/*
 * A Maze Solver using the flood fill algorithm. Implemented originally here in Java by
 * Alexander Hadik (www.alexhadik.com) using publicly available resources on the flood
 * fill technique. Implemented in Java for the purposes of rapid development and testing
 * with intention of porting to C for compilation and use on Arduino Nano
 */
import java.io.IOException;
import java.util.Stack;

/*
 * Master Class to contain all data structures and methods necessary to generate and store maze
 * 	-x,y: dimensions of maze
 * 	-xMid, yMid: defined midpoints of maze (off center for maze of even size)
 * 	-maze: 2 dimensional array of integers storing integer values mapping to possible configurations of each maze space
 * 	-distMaze: 2 dimensional array of integers specifying the calculated distance from the current cell to xMid,yMid
 * 	-expMaze: 2 dimensional array of integers modeling how the maze is known to the robot. That is, it starts empty and is filled
 *	in cell by cell as the algorithm explores the maze. 
 * 	-render: uninitialized definition for a render object
 * 	-headings: array of integers specifying possible headings
 */
public class MazeSolver {
	
	private static int x;
	private static int y;
	private static int xMid;
	private static int yMid;
	private static int[][] expMaze;
	private static int[][] distMaze;
	private static int[][] maze;
	static double inf = Double.POSITIVE_INFINITY;
	static int[] headings = new int[]{1,2,4,8};
	static Render render;
 
	/*
	 * CONSTRUCTOR: MazeSolver - takes size of the maze and a precomputed maze as inputs
	 */
	public MazeSolver(int x, int y, int[][] maze) {
		this.x = x;
		this.y = y;
		//floor the division such that xMid of 6x6 maze is 3 (1 indexed)
		this.xMid = (int) Math.floor((double) this.x/2);
		this.yMid = (int) Math.floor((double) this.y/2);
		this.maze = maze;
		distMaze = new int[this.x][this.y];
		expMaze = new int[this.x][this.y];
		this.render = new Render(x, y, maze);
	}

	//Given x,y coordinate (coord), calculate the ideal distance from coord to defined midpoint 
	//xMid,yMid (mid) using the formula dist = abs(yMid - y)+abs(xMid - x). Return value as int
	public static int calcDist(int x, int y) {
		int dist = Math.abs(yMid - y)+Math.abs(xMid - x);
		return dist;
	}
	
	/*
	 * Input: A Tuple representing the current coordinate and an integer representing the robots current heading
	 * Output: A Tuple representing the next coordinate based upon that heading.
	 */
	public static Tuple<Integer, Integer> bearingCoord(Tuple<Integer, Integer> currCoord, Integer heading){
		
		Tuple<Integer,Integer> nextCoord = new Tuple<Integer,Integer>(0,0);
		
		switch(heading) {
		case 1: nextCoord.x=currCoord.x; nextCoord.y=currCoord.y-1; break;
		case 2: nextCoord.x=currCoord.x; nextCoord.y=currCoord.y+1; break;
		case 4: nextCoord.x=currCoord.x+1; nextCoord.y=currCoord.y; break;
		case 8: nextCoord.x=currCoord.x-1; nextCoord.y=currCoord.y; break;
		}
		return nextCoord;
	}

	/*
	 * Input: A Tuple representing the current coordinate and the robots current heading
	 * Output: A Tuple representing the next optimal coordinate for the robot
	 * 
	 * Notes: This function gives preference for the robot staying straight. 
	 * A coordinate other than the heading provided as an argument will be returned only
	 * if turning offers a more optimal score from the distance matrix.
	 */
	public static Tuple<Tuple<Integer,Integer>,Integer> orient(Tuple<Integer, Integer> currCoord, Integer heading){
		
		Tuple<Integer,Integer> leastnext = null;
		double leastNextVal = inf;
		int leastDir = heading;
		//If there is bitwise equivalence between the current heading and the cell's value, then the next cell is accessible
		if ((expMaze[currCoord.x][currCoord.y] & heading) != 0){
			
			//Define a coordinate for the next cell based on this heading and set the leastNextVal to its value
			Tuple<Integer,Integer> leastnextTemp = bearingCoord(currCoord,heading);
			if(checkBounds(leastnextTemp)){
				leastnext = leastnextTemp;
				leastNextVal = distMaze[leastnext.x][leastnext.y];
			}
		}
		
		/*For each of the possible headings, check if they are accessible, 
		 * and if their value is less than the currently set value. If both 
		 * are true, then set the next coord appropriately
		 */
		for(int dir : headings){
			//If this dir is accessible
			if ((expMaze[currCoord.x][currCoord.y] & dir) != 0){
				
				//Define the coordinate for this dir
				Tuple<Integer,Integer> dirCoord = bearingCoord(currCoord, dir);
				
				if(checkBounds(dirCoord)){
					//If this dir is more optimal than continuing straight
					if((double) distMaze[dirCoord.x][dirCoord.y] < leastNextVal){
						//Update the value of leastNext Val
						leastNextVal = (double) distMaze[dirCoord.x][dirCoord.y];
						//Update the value of least next to this dir.
						leastnext = dirCoord;
						leastDir = dir;
					}
				}
			}
			
		}
		return new Tuple<Tuple<Integer,Integer>,Integer>(leastnext,leastDir);
	}
	
	public static Boolean checkBounds(Tuple<Integer,Integer> coord){
		if ((coord.x >= x) || (coord.y >= y) || (coord.x < 0) || (coord.y < 0)){return false;}else{return true;}
	}
	
	/*
	 * Input: Tuple representing coordinates in the maze
	 * Output: A Tuple with a Boolean representing if the minimum coord accessible from the provided coord is one less than the provided coord 
	 */
	public static Tuple<Boolean, Integer> checkNeighs(Tuple<Integer,Integer> coord){
		boolean returnBool = false;

		double minVal = inf;
		Tuple<Integer,Integer> minCoord = null;
		for (int dir : headings){
			//Check that heading is accessible
			if ((expMaze[coord.x][coord.y] & dir) != 0){
				//Get the coordinate of the accessible neighbor
				Tuple<Integer,Integer> neighCoord = bearingCoord(coord, dir);
				//Check the value of the accessible neighbor
				if (checkBounds(neighCoord)){
					//If the neighbor is less than the current recorded minimum value, update the minimum value
					if ((double) distMaze[neighCoord.x][neighCoord.y] < minVal){minVal = (double) distMaze[neighCoord.x][neighCoord.y]; minCoord = neighCoord;}
				}
			}
		}
		//Check that the value of the coord argument is one greater than the minimum value
		if (((int) minVal<distMaze[coord.x][coord.y])||(isEnd(coord))){
		//if (((int) minVal+1)==distMaze[coord.x][coord.y]){
			returnBool = true;
		}
		
		return new Tuple<Boolean,Integer>(returnBool,(int) minVal);
	}
	
	/*
	 * Return a boolean indicating if the coordinate is an identified dead end
	 */
	public static Boolean isDead(Tuple<Integer,Integer> coord){
		Boolean deadEnd = false;
		if (checkBounds(coord)){
			int dist = expMaze[coord.x][coord.y];
			//If this coordinate represents an identified dead end, set return to TRUE
			if((dist == 1)||(dist == 2)||(dist == 4)||(dist == 8)){deadEnd = true;}
		}
		return deadEnd;
	}
	
	public static Boolean isEnd(Tuple<Integer,Integer> coord){
		Boolean End = false;
		if (checkBounds(coord)){
			//If this coordinate represents an identified dead end, set return to TRUE
			if((coord.x == xMid)&&(coord.y == yMid)){End = true;}
		}
		return End;
	}
	
	/*
	 * Input: A Tuple representing the robots current coordinate
	 * Output: Void
	 * Functionality: This function updates the maze to account for learned walls of the maze.
	 */
	public static void floodFillUpdate(Tuple<Integer,Integer> currCoord){
		Stack<Tuple<Integer,Integer>> entries = new Stack<Tuple<Integer,Integer>>();
		//Update the value of the exploratory maze to the value of the generated maze
		expMaze[currCoord.x][currCoord.y]=MazeSolver.maze[currCoord.x][currCoord.y];
		entries.push(currCoord);

		for (int dir : headings){
			//If there's a wall in this direction
			if ((expMaze[currCoord.x][currCoord.y] & dir) == 0){
				Tuple<Integer,Integer> workingCoord = new Tuple<Integer,Integer>(currCoord.x,currCoord.y);
				switch (dir){
					case 1: workingCoord.y=workingCoord.y-1; coordUpdate(workingCoord,2); break;
					case 2: workingCoord.y=workingCoord.y+1; coordUpdate(workingCoord,1); break;
					case 4: workingCoord.x=workingCoord.x+1; coordUpdate(workingCoord,8); break;
					case 8: workingCoord.x=workingCoord.x-1; coordUpdate(workingCoord,4); break;
				}
				//If the working coordinate has been identified as a dead end, add it to the stack for analysis.
				//if(isDead(workingCoord)){entries.push(workingCoord);}
				if(checkBounds(workingCoord)&&(!isEnd(workingCoord))){
					entries.push(workingCoord);
				}
			}
		}
		while(!entries.empty()){
			//Pop an entry from the stack
			Tuple<Integer,Integer> workingEntry = entries.pop();
			Tuple<Boolean,Integer> neighCheck = checkNeighs(workingEntry);
			if (!neighCheck.x){
				distMaze[workingEntry.x][workingEntry.y] = neighCheck.y+1;
				for (int dir : headings){
					if ((expMaze[workingEntry.x][workingEntry.y] & dir) != 0){
						Tuple<Integer,Integer> nextCoord = bearingCoord(workingEntry,dir);
						if(checkBounds(nextCoord)){
							//Push the coordinate onto the entries stack
							if(!isEnd(nextCoord)){
								entries.push(nextCoord);
							}
						}
					}
				}
			}
		}
	}
	
	/*
	 * INPUT: A tuple representing the coordinate to update, and a direction representing the wall to add
	 * OUTPUT: Update to coordinate adding the wall provided as an argument
	 */
	public static void coordUpdate(Tuple<Integer,Integer> coordinate, Integer wallDir){
		if (checkBounds(coordinate)){
			if ((expMaze[coordinate.x][coordinate.y] & wallDir) != 0){
				expMaze[coordinate.x][coordinate.y] = expMaze[coordinate.x][coordinate.y]-wallDir;
			}
		}
	}
	
	public static void floodFill(){
		Tuple<Integer,Integer> currCoord = new Tuple<Integer,Integer>(0,0);
		int heading = 8;
		/*Integer representation of heading
		 * 1 = N
		 * 4 = E
		 * 2 = S
		 * 8 = W
		 */
		for(int i=0;i<2;i++){
			while(distMaze[currCoord.x][currCoord.y] != 0){
				floodFillUpdate(currCoord);
				Tuple<Tuple<Integer,Integer>,Integer> nextHeading = orient(currCoord, heading);
				Tuple<Integer,Integer> nextCoord = nextHeading.x;
				int nextDir = nextHeading.y;
				currCoord = nextCoord;
				heading = nextDir;
				render.track(currCoord, expMaze, distMaze);
				System.out.println();
			}
			currCoord.x=0;
			currCoord.y=0;
		}
		
	}
	
	/*
	 * Function: Fill the distMaze in with most optimistic values
	 */
	public void instantiate() {
		for (int i = 0; i < y; i++) {
			for (int j = 0; j < x; j++) {
				distMaze[j][i] = calcDist(i,j);
				expMaze[j][i] = 15;
				//If this is the left column (0,x)
				if(j==0){
					expMaze[j][i] = 7;
				}
				if(i==0){
					expMaze[j][i] = 14;
				}
				if(j==(x-1)){
					expMaze[j][i] = 11;
				}
				if(i==(y-1)){
					expMaze[j][i] = 13;
				}
				expMaze[0][0] = 6;
				expMaze[x-1][0] = 10;
				expMaze[0][y-1] = 5;
				expMaze[x-1][y-1] = 9;
			}
		}
	}
	
	/*
	 * INPUT: NONE
	 * OUTPUT: A Boolean representing if the maze has been labeled with a shortest path
	 * Function: Given a labeled maze and a starting position, the path is traced 'downstream' to determine if a valid path has been found.
	 */
	public static Tuple<Boolean,Tuple<Integer,Integer>> testShortestPath(Tuple<Integer,Integer> startCoord){
		Tuple<Integer,Integer> currCoord = startCoord;
		Tuple<Boolean,Tuple<Integer,Integer>> testResult = new Tuple<Boolean,Tuple<Integer,Integer>>(false,currCoord);
		while(distMaze[currCoord.x][currCoord.y]!=0){
			Boolean headingFound = false;
			for(int dir : headings){
				//If this heading is open
				if((expMaze[currCoord.x][currCoord.y] & dir) != 0){
					//Get the coordinates of this potential next coordinate
					Tuple<Integer,Integer> headingCoord = bearingCoord(currCoord,dir);
					//If this next coordinate has a value one less than the current coordinate, update the current coordinate value
					if((distMaze[headingCoord.x][headingCoord.y]+1)==distMaze[currCoord.x][currCoord.y]){
						currCoord = headingCoord;
						testResult.y.x=currCoord.x;
						testResult.y.y=currCoord.y;
						headingFound = true;
						break;
					}
				}
			}
			//If no heading was found, break and return false, as well as the coordinates
			if(!headingFound){
				System.out.println("NO HEADING FOUND");
				System.out.println(expMaze[currCoord.x][currCoord.y]);		
				break;
			}
		}
		if(isEnd(currCoord)){testResult.x=true;}
		
		return testResult;
	}

	public static void main(String[] args) throws IOException {
		
		MazeGenerator maze = null;
		int x = 0;
		int y = 0;
		if((args.length > 1)&&(args[0].equals("-m"))){
			maze = new MazeGenerator(args[1]);
			x = maze.x;
			y = maze.y;
		}else{
			x = args.length >= 1 ? (Integer.parseInt(args[0])) : 8;
			y = args.length == 2 ? (Integer.parseInt(args[1])) : 8;
			//Generate a random maze for testing
			maze = new MazeGenerator(x, y);
		}
		
		//Generate a blank maze for exploring
		MazeSolver distMaze = new MazeSolver(x, y, maze.maze);
		render.display();
		distMaze.instantiate();
		floodFill();
		Tuple<Integer,Integer> start = new Tuple<Integer,Integer>(0,0);

		render.dirmark(maze.maze, distMaze.distMaze);
		render.dirmark(expMaze, distMaze.distMaze);
		
	}
	
}
