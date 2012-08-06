package com.exnw.browedit.math;


public class Matrix3{
	private float[] data;
	
	public Matrix3(){
		this.data = new float[] 
		                      { 1, 0, 0,
								0, 1, 0,
								0, 0, 1,
		                      };
	}
	
	public Matrix3( Number m11, Number m12, Number m13,	Number m21, Number m22, Number m23,	Number m31, Number m32, Number m33 ){
		this();
		
		this.data[0] = m11.floatValue();
		this.data[1] = m12.floatValue();
		this.data[2] = m13.floatValue();
		
		this.data[3] = m21.floatValue();
		this.data[4] = m22.floatValue();
		this.data[5] = m23.floatValue();
		
		this.data[6] = m31.floatValue();
		this.data[7] = m32.floatValue();
		this.data[8] = m33.floatValue();
	}
	
	public Matrix3( Number[] data ){
		this();
		
		if( data.length < 16 )
			throw new IllegalArgumentException();
		
		for( int i = 0; i < 16; i++ ){
			this.data[i] = data[i].floatValue();
		}
	}
	
	public Matrix3( float[] data )
	{
		this.data = data.clone();
	}	
	

	public float getValue( int row, int column ){
		return this.data[row*4+column];
	}
	
	public float[] getData(){
		return this.data;
	}


	public Matrix3 transpose()
	{
		return new Matrix3(
				data[0], data[3], data[6],
				data[1], data[4], data[7],
				data[2], data[5], data[8]
				);
	}
	
	public Matrix3 invert()
	{
		double[][] dat = new double[][] { 
			{ data[0], data[1], data[2], },
			{ data[3], data[4], data[5], },
			{ data[6], data[7], data[8]  }};
			
		blainvert(dat);
		
		return new Matrix3(
				dat[0][0], dat[0][1], dat[0][2],
				dat[1][0], dat[1][1], dat[1][2],
				dat[2][0], dat[2][1], dat[2][2]			
				);	
	}
	
	
	
	
	  public static final void blainvert(double A[][])
	  {
	    int n = A.length;
	    int row[] = new int[n];
	    int col[] = new int[n];
	    double temp[] = new double[n];
	    int hold , I_pivot , J_pivot;
	    double pivot, abs_pivot;

	    if(A[0].length!=n)
	    {
	      System.out.println("Error in Matrix.invert, inconsistent array sizes.");
	    }
	    // set up row and column interchange vectors
	    for(int k=0; k<n; k++)
	    {
	      row[k] = k ;
	      col[k] = k ;
	    }
	    // begin main reduction loop
	    for(int k=0; k<n; k++)
	    {
	      // find largest element for pivot
	      pivot = A[row[k]][col[k]] ;
	      I_pivot = k;
	      J_pivot = k;
	      for(int i=k; i<n; i++)
	      {
	        for(int j=k; j<n; j++)
	        {
	          abs_pivot = Math.abs(pivot) ;
	          if(Math.abs(A[row[i]][col[j]]) > abs_pivot)
	          {
	            I_pivot = i ;
	            J_pivot = j ;
	            pivot = A[row[i]][col[j]] ;
	          }
	        }
	      }
	      if(Math.abs(pivot) < 1.0E-10)
	      {
	    	  new Exception("Matrix is singular !").printStackTrace();
	        return;
	      }
	      hold = row[k];
	      row[k]= row[I_pivot];
	      row[I_pivot] = hold ;
	      hold = col[k];
	      col[k]= col[J_pivot];
	      col[J_pivot] = hold ;
	       // reduce about pivot
	      A[row[k]][col[k]] = 1.0 / pivot ;
	      for(int j=0; j<n; j++)
	      {
	        if(j != k)
	        {
	          A[row[k]][col[j]] = A[row[k]][col[j]] * A[row[k]][col[k]];
	        }
	      }
	      // inner reduction loop
	      for(int i=0; i<n; i++)
	      {
	        if(k != i)
	        {
	          for(int j=0; j<n; j++)
	          {
	            if( k != j )
	            {
	              A[row[i]][col[j]] = A[row[i]][col[j]] - A[row[i]][col[k]] *
	                                   A[row[k]][col[j]] ;
	            }
	          }
	          A[row[i]][col [k]] = - A[row[i]][col[k]] * A[row[k]][col[k]] ;
	        }
	      }
	    }
	    // end main reduction loop

	    // unscramble rows
	    for(int j=0; j<n; j++)
	    {
	      for(int i=0; i<n; i++)
	      {
	        temp[col[i]] = A[row[i]][j];
	      }
	      for(int i=0; i<n; i++)
	      {
	        A[i][j] = temp[i] ;
	      }
	    }
	    // unscramble columns
	    for(int i=0; i<n; i++)
	    {
	      for(int j=0; j<n; j++)
	      {
	        temp[row[j]] = A[i][col[j]] ;
	      }
	      for(int j=0; j<n; j++)
	      {
	        A[i][j] = temp[j] ;
	      }
	    }
	  } // end invert
	
	
	public Matrix3 clone()
	{
		return new Matrix3(data.clone());
	}


	public String toString()
	{
		return 
				data[0] + ",\t" + data[3] + ",\t" + data[6] + ",\n" +
				data[1] + ",\t" + data[4] + ",\t" + data[7] + ",\n" +
				data[2] + ",\t" + data[5] + ",\t" + data[8] + ",\n";
	}
	

}
