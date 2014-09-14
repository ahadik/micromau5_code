/*
 * TUPLE CLASS: Hold two values accessible as values X and Y. Used to pass coordinates around.
 */
public class Tuple<X, Y> { 
	  public X x; 
	  public Y y; 
	  public Tuple(X x, Y y) { 
	    this.x = x; 
	    this.y = y; 
	  } 
}