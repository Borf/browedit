package com.exnw.browedit.math;

public class Matrix4{
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
		                             { 	1, 0, 0, x,
										0, 1, 0, y,
										0, 0, 1, z,
										0, 0, 0, 1
              });
	}

	public void mult(Matrix4 other)
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
		data = newData;
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
	
	
}
