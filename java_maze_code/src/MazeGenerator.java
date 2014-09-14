/*
 * recursive backtracking algorithm
 * shamelessly borrowed from the ruby at
 * http://weblog.jamisbuck.org/2010/12/27/maze-generation-recursive-backtracking
 * 
 * Modified to suit purpose of MicroMau5 Project of Alexander Hadik, Tristan Paine and Kevin Jung of Brown University
 */


import java.io.IOException;
import java.util.Collections;
import java.util.Arrays;
import java.util.HashMap; 

/*
 * Master Class to contain all data structures and methods necessary to generate and store maze
 * 	-x,y: dimensions of maze
 * 	-xMid, yMid: defined midpoints of maze (off center for maze of even size)
 * 	-maze: 2 dimensional array of integers storing integer values mapping to possible configurations of each maze space
 * 	-distMaze: 2 dimensional array of integers specifying the calculated distance from the current cell to xMid,yMid
 * 	-render: uninitialized definition for a render object
 */

public class MazeGenerator {
	int x;
	int y;
	private static int xMid;
	private static int yMid;
	static int[][] maze;
	private static int[][] distMaze;
	static double inf = Double.POSITIVE_INFINITY;
	static Render render;
 
	/*
	 * CONSTRUCTOR: MazeGenerator - take maze dimensions as arguments and generate random maze
	 */
	public MazeGenerator(int x, int y) {
		this.x = x;
		this.y = y;
		MazeGenerator.xMid = (int) Math.floor((double) this.x/2);
		MazeGenerator.yMid = (int) Math.floor((double) this.y/2);
		maze = new int[this.x][this.y];
		this.render = new Render(x, y, maze);
		distMaze = new int[this.x][this.y];
		zeroDistMaze();
		generateMaze(0, 0);
	}
 
	/*
	 * CONSTRUCTOR: MazeGenerator - take filepath as argument and parse stored maze to memory
	 */
	public MazeGenerator(String filePath) throws IOException {
		Parser parsedMaze = new Parser(filePath);
		this.x = parsedMaze.stringMaze.length;
		this.y = parsedMaze.stringMaze[0].length;
		xMid = (int) Math.floor((double) this.x/2);
		yMid = (int) Math.floor((double) this.y/2);
		maze = parsedMaze.stringMaze;
		render = new Render(x, y, maze);
		distMaze = new int[this.x][this.y];
		zeroDistMaze();
	}
	
	//Fill the distMaze with zeros
	public void zeroDistMaze(){
		for(int[] column : distMaze){
			Arrays.fill(column, 0);
		}
	}

	//Given x,y coordinate (coord), calculate the ideal distance from coord to defined midpoint 
	//xMid,yMid (mid) using the formula dist = abs(yMid - y)+abs(xMid - x). Return value as int
	public static int calcDist(int x, int y) {
		int dist = Math.abs(yMid - y)+Math.abs(xMid - x);
		return dist;
	}
	
	//Given an integer distance value (assumed to be less than 100), convert the int to a string
	//and append 0 if necessary to ensure the output string is two characters
	public static String distString(int dist){
		String distVal;
		if (dist > 9){
			distVal = Integer.toString(dist);
		}else{
			distVal = "0"+Integer.toString(dist);
		}
		return distVal;
	}

	
	//Set initial ideal values of the distance matrix (distMaze) while simultaneously reporting
	//these initial values to standard out in a graphical manner
	public void mark() {
		//for every row
		for (int i = 0; i < y; i++) {
			//for every column
			for (int j = 0; j< x; j++) {
				//print the horizontal section (either a wall or an open space)
				System.out.print((maze[j][i] & 1) == 0 ? "+---" : "+   ");
			}
			System.out.println("+");
			//for every column
			for (int j = 0; j < x; j++) {
				//calculate the ideal distance from the current position to the defined xMix,yMid coord
				//and print the resulting value to standard out. Then set that value in distMaze
				String dist = distString(calcDist(i,j));
				System.out.print((maze[j][i] & 8) == 0 ? "|"+dist+" " : " "+dist+" ");
				distMaze[j][i] = calcDist(i,j);
			}
			System.out.println("|");
		}
		// draw the bottom line
		for (int j = 0; j < x; j++) {
			System.out.print("+---");
		}
		System.out.println("+");
	}
	
	//for every cell in the maze matrix, print the structure of the cell, and the value of it's coordinate,
	//so one can visually match coord int values to their structure
	public void dirmark() {
		for (int i = 0; i < y; i++) {
			for (int j = 0; j< x; j++) {
				System.out.print((maze[j][i] & 1) == 0 ? "+---" : "+   ");
			}
			System.out.println("+");
			for (int j = 0; j < x; j++) {
				String coord = distString(maze[j][i]);
				System.out.print((maze[j][i] & 8) == 0 ? "|"+coord+" " : " "+coord+" ");
				distMaze[j][i] = calcDist(i,j);
			}
			System.out.println("|");
		}
		// draw the bottom line
		for (int j = 0; j < x; j++) {
			System.out.print("+---");
		}
		System.out.println("+");
	}
	

	//Recursively generate the maze by filling in values for the maze[][] matrix.
	private void generateMaze(int cx, int cy) {
		DIR[] dirs = DIR.values();
		Collections.shuffle(Arrays.asList(dirs));
		for (DIR dir : dirs) {
			int nx = cx + dir.dx;
			int ny = cy + dir.dy;
			if (between(nx, x) && between(ny, y)
					&& (maze[nx][ny] == 0)) {
				maze[cx][cy] |= dir.bit;
				maze[nx][ny] |= dir.opposite.bit;
				generateMaze(nx, ny);
			}
		}
	}
 
	//test if v is between 0 and an upper bound. Return resulting boolean
	private static boolean between(int v, int upper) {
		return (v >= 0) && (v < upper);
	}
 
	
	private enum DIR {
		N(1, 0, -1), S(2, 0, 1), E(4, 1, 0), W(8, -1, 0);
		private final int bit;
		private final int dx;
		private final int dy;
		private DIR opposite;
 
		// use the static initializer to resolve forward references
		static {
			N.opposite = S;
			S.opposite = N;
			E.opposite = W;
			W.opposite = E;
		}
 
		private DIR(int bit, int dx, int dy) {
			this.bit = bit;
			this.dx = dx;
			this.dy = dy;
		}
	};
 
	//Take as input either two integers to define the size of the maze, or if no arguments
	//are provided, default to size of 8x8
	public static void main(String[] args) {
		int x = args.length >= 1 ? (Integer.parseInt(args[0])) : 8;
		int y = args.length == 2 ? (Integer.parseInt(args[1])) : 8;
		MazeGenerator maze = new MazeGenerator(x, y);
		//render the maze on screen in std out
		render.display();
	}
 
}