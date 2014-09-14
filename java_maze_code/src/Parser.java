import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;

public class Parser {
	
	//Instantiate a String to hold the path to the maze
	String filePath;
	int[][] stringMaze;
	int index = 0;
	
	/*
	 * INPUT: A string of spaces
	 * OUTPUT: An integer representing the number of cells those spaces represent
	 */
	public Integer spreadAmount(String spread){
		int spreadLength = spread.length()+1;
		int numCells = spreadLength/4;
		return numCells;
	}
	
	public String[] splitStringEvery(String s, int interval) {
	    int arrayLength = (int) Math.ceil(((s.length() / (double)interval)));
	    String[] result = new String[arrayLength];

	    int j = 0;
	    int lastIndex = result.length - 1;
	    for (int i = 0; i < lastIndex; i++) {
	        result[i] = s.substring(j, j + interval);
	        j += interval;
	    } //Add the last bit
	    result[lastIndex] = s.substring(j);

	    return result;
	}
	
	public void popRow(String[] firstArray, String[] secondArray,int row){
		for(int i=0;i<firstArray.length-1;i++){
			//If there is a wall to the west, subtract 8 
			if(Character.toString(secondArray[i].charAt(0)).equals("|")){
				stringMaze[i][row] = stringMaze[i][row]-8; 
			}
			//If there is a northern wall, subtract 1
			if(Character.toString(firstArray[i].charAt(1)).equals("-")){
				stringMaze[i][row] = stringMaze[i][row]-1;
				//If this isn't the first row, set a southern wall for the element above
				if(row!=0){
					stringMaze[i][row-1] = stringMaze[i][row-1]-2;
				}
			}
			//If there is a wall to the east, subtract 4
			if(Character.toString(secondArray[i+1].charAt(0)).equals("|")){
				stringMaze[i][row] = stringMaze[i][row]-4; 
			}
			
		}
	}
	
	//Create a constructor for Parser class that sets the value of filePath
	Parser(String filepath) throws IOException{
		this.filePath = filepath;
		
		//Define a FileReader object from the filePath
		FileReader input = new FileReader(this.filePath);
		BufferedReader bufRead = new BufferedReader(input);		
		
		//Read in the first two lines to initialize them
		String firstLine = bufRead.readLine();
		String secondLine = bufRead.readLine();
		String[] firstSplit = splitStringEvery(firstLine,4);
		String[] secondSplit = splitStringEvery(secondLine,4);
		
		//Instantiate the stringMaze matrix based upon the length of the provided first line;
		int length = firstSplit.length-1;
		stringMaze = new int[length][length];
		//Fill each element of the empty maze to have no walls.
		for(int[] row : stringMaze){
			Arrays.fill(row, 15);
		}
		
		popRow(firstSplit, secondSplit, index);
		index++;
		firstLine = bufRead.readLine();
		secondLine = bufRead.readLine();
		
		//For all lines in the maze file
		while (secondLine != null){
			
			//Read the first line of the maze
			firstSplit = splitStringEvery(firstLine,4);
			secondSplit = splitStringEvery(secondLine,4);
			popRow(firstSplit, secondSplit, index);
			firstLine = bufRead.readLine();
			secondLine = bufRead.readLine();
			//Update the index by one
			index++;
			
		}
	}
}
