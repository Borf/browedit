package com.exnw.browedit.math;

import java.io.Serializable;


public class Matrix4 implements Serializable{
	private float[] data;
	
	public Matrix4(){
		this.data = new float[] 
		                      { 1, 0, 0, 0,
								0, 1, 0, 0,
								0, 0, 1, 0,
								0, 0, 0, 1
		                      };
	}
	
	public Matrix4( Number m11, Number m12, Number m13,	Number m21, Number m22, Number m23,	Number m31, Number m32, Number m33 ){
		this();
		
		this.data[0] = m11.floatValue();
		this.data[1] = m12.floatValue();
		this.data[2] = m13.floatValue();
		this.data[3] = 0.0f;
		
		this.data[4] = m21.floatValue();
		this.data[5] = m22.floatValue();
		this.data[6] = m23.floatValue();
		this.data[7] = 0.0f;
		
		this.data[8] = m31.floatValue();
		this.data[9] = m32.floatValue();
		this.data[10] = m33.floatValue();
		this.data[11] = 0.0f;
		
		this.data[12] = 0.0f;
		this.data[13] = 0.0f;
		this.data[14] = 0.0f;
		this.data[15] = 0.0f;
	}
	
	public Matrix4( Number m11, Number m12, Number m13, Number m14,	Number m21, Number m22, Number m23, Number m24,	Number m31, Number m32, Number m33, Number m34, Number m41, Number m42, Number m43, Number m44 ){
		this();
		
		this.data[0] = m11.floatValue();
		this.data[1] = m12.floatValue();
		this.data[2] = m13.floatValue();
		this.data[3] = m14.floatValue();
		
		this.data[4] = m21.floatValue();
		this.data[5] = m22.floatValue();
		this.data[6] = m23.floatValue();
		this.data[7] = m24.floatValue();
		
		this.data[8] = m31.floatValue();
		this.data[9] = m32.floatValue();
		this.data[10] = m33.floatValue();
		this.data[11] = m34.floatValue();
		
		this.data[12] = m41.floatValue();
		this.data[13] = m42.floatValue();
		this.data[14] = m43.floatValue();
		this.data[15] = m44.floatValue();
	}
	
	public Matrix4( Number[] data ){
		this();
		
		if( data.length < 16 )
			throw new IllegalArgumentException();
		
		for( int i = 0; i < 16; i++ ){
			this.data[i] = data[i].floatValue();
		}
	}
	
	public Matrix4( float[] data )
	{
		this.data = data.clone();
	}	
	public Matrix4( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
		this();
		this.read(in);
	}
	
	public void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
		for( int i = 0; i < 3; i++ )
			this.data[i] = in.readFloat();
		this.data[3] = 0.0f;
		
		for( int i = 4; i < 7; i++ )
			this.data[i] = in.readFloat();
		this.data[7] = 0.0f;
		
		for( int i = 8; i < 11; i++ )
			this.data[i] = in.readFloat();
		this.data[11] = 0.0f;
		
		this.data[12] = 0.0f;
		this.data[13] = 0.0f;
		this.data[14] = 0.0f;
		this.data[15] = 1.0f;
	}
	
	public float getValue( int row, int column ){
		return this.data[row*4+column];
	}
	
	public float[] getData(){
		return this.data;
	}

	public static Matrix4 makeTranslation(float x, float y, float z)
	{
		return new Matrix4(new float[] 
		                             { 	1, 0, 0, 0,
										0, 1, 0, 0,
										0, 0, 1, 0,
										x, y, z, 1
              });
	}

	public Matrix4 mult(Matrix4 other)
	{
		float[] newData = new float[16];
		newData[0] = data[0]*other.data[0] + data[4]*other.data[1] + data[8]*other.data[2] + data[12]*other.data[3];
		newData[4] = data[0]*other.data[4] + data[4]*other.data[5] + data[8]*other.data[6] + data[12]*other.data[7];
		newData[8] = data[0]*other.data[8] + data[4]*other.data[9] + data[8]*other.data[10]+ data[12]*other.data[11];
		newData[12]= data[0]*other.data[12]+ data[4]*other.data[13]+ data[8]*other.data[14]+ data[12]*other.data[15];

		newData[1] = data[1]*other.data[0] + data[5]*other.data[1] + data[9]*other.data[2] + data[13]*other.data[3];
		newData[5] = data[1]*other.data[4] + data[5]*other.data[5] + data[9]*other.data[6] + data[13]*other.data[7];
		newData[9] = data[1]*other.data[8] + data[5]*other.data[9] + data[9]*other.data[10]+ data[13]*other.data[11];
		newData[13]= data[1]*other.data[12]+ data[5]*other.data[13]+ data[9]*other.data[14]+ data[13]*other.data[15];

		newData[2] = data[2]*other.data[0] + data[6]*other.data[1] + data[10]*other.data[2] + data[14]*other.data[3];
		newData[6] = data[2]*other.data[4] + data[6]*other.data[5] + data[10]*other.data[6] + data[14]*other.data[7];
		newData[10]= data[2]*other.data[8] + data[6]*other.data[9] + data[10]*other.data[10]+ data[14]*other.data[11];
		newData[14]= data[2]*other.data[12]+ data[6]*other.data[13]+ data[10]*other.data[14]+ data[14]*other.data[15];

		newData[3] = data[3]*other.data[0] + data[7]*other.data[1] + data[11]*other.data[2] + data[15]*other.data[3];
		newData[7] = data[3]*other.data[4] + data[7]*other.data[5] + data[11]*other.data[6] + data[15]*other.data[7];
		newData[11]= data[3]*other.data[8] + data[7]*other.data[9] + data[11]*other.data[10]+ data[15]*other.data[11];
		newData[15]= data[3]*other.data[12]+ data[7]*other.data[13]+ data[11]*other.data[14]+ data[15]*other.data[15];
		return new Matrix4(newData);
	}

	public static Matrix4 makeRotation(float angle, float x, float y, float z)
	{
		float len = x*x+y*y+z*z;
		if(len > 1)
		{
			len = (float)Math.sqrt(len);
			x /= len;
			y /= len;
			z /= len;
		}
		float c = (float)Math.cos(angle/(180/Math.PI));
		float s = (float)Math.sin(angle/(180/Math.PI));

		float[] values = new float[16];
		values[0] = x*x*(1-c)+c;
		values[4] = x*y*(1-c)-z*s;
		values[8] = x*z*(1-c)+y*s;
		values[12] = 0;

		values[1] = y*x*(1-c)+z*s;
		values[5] = y*y*(1-c)+c;
		values[9] = y*z*(1-c)-x*s;
		values[13] = 0;

		values[2] = z*x*(1-c)-y*s;
		values[6] = z*y*(1-c)+x*s;
		values[10]= z*z*(1-c)+c;
		values[14] = 0;

		values[3] = 0;
		values[7] = 0;
		values[11]= 0;
		values[15]= 1;

		return new Matrix4(values);
	}

	public static Matrix4 makeScale(float x, float y, float z)
	{
		return new Matrix4(new float[] 
		                             { 	x, 0, 0, 0,
										0, y, 0, 0,
										0, 0, z, 0,
										0, 0, 0, 1
              });
	}

	public Vector3 mult(Vector3 other)
	{
		return new Vector3(	data[0]*other.getX() + data[4]*other.getY()+data[8]*other.getZ() + data[12],
							data[1]*other.getX() + data[5]*other.getY()+data[9]*other.getZ() + data[13],
							data[2]*other.getX() + data[6]*other.getY()+data[10]*other.getZ()+ data[14]);
	}
	
	
	public Matrix4 clone()
	{
		return new Matrix4(data.clone());
	}

	public static Matrix4 makePerspective(float fov, float aspect, float near, float far)
	{
		float f = (float)(1 / Math.tan(fov/180*Math.PI / 2));
		return new Matrix4(new float[]
		{
			f/aspect,		0,		0,						0,
			0,				f,		0,						0,
			0,				0,		(far+near)/(near-far),	-1,
			0,				0,		(2*far+near)/(near-far),0
		});
	}
	
	
	public static Matrix4 makeFrustum(float left, float right, float bottom, float top, float nearVal, float farVal)
	{
		float A = (right+left) / (right - left);
		float B = (top+bottom)/(top-bottom);
		float C = -(farVal+nearVal)/(farVal-nearVal);
		float D = -(2*farVal*nearVal)/(farVal-nearVal);
		
		
		
		return new Matrix4(new float[]
		{
			(2*nearVal)/(right-left),	0,								A,				0,
			0,							(2*nearVal)/(top-bottom),		B,				0,
			0,							0,								C,				-1,
			0,							0,								D,				0
		});
	}
	
	
	public String toString()
	{
		return 
				data[0] + ",\t" + data[4] + ",\t" + data[8] + ",\t" + data[12] + ",\n" +
				data[1] + ",\t" + data[5] + ",\t" + data[9] + ",\t" + data[13] + ",\n" +
				data[2] + ",\t" + data[6] + ",\t" + data[10] + ",\t" + data[14] + ",\n" +
				data[3] + ",\t" + data[7] + ",\t" + data[11] + ",\t" + data[15] + ",\n";
	}

	
	public Matrix4 invert()
	{
		double[][] dat = new double[][] { 
			{ data[0], data[1], data[2], data[3], },
			{ data[4], data[5], data[6], data[7], },
			{ data[8], data[9], data[10], data[11], },
			{ data[12], data[13], data[14], data[15], }
		};
			
		blainvert(dat);
		
		return new Matrix4(
				dat[0][0], dat[0][1], dat[0][2], dat[0][3],
				dat[1][0], dat[1][1], dat[1][2], dat[1][3],
				dat[2][0], dat[2][1], dat[2][2], dat[2][3],		
				dat[3][0], dat[3][1], dat[3][2], dat[3][3]		
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
	
	

}
