package com.exnw.browedit.math;

public class Matrix3{
	private float[] data;
	
	public Matrix3(){
		this.data = new float[16];
	}
	
	public Matrix3( Number m11, Number m12, Number m13,	Number m21, Number m22, Number m23,	Number m31, Number m32, Number m33 ){
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
	
	public Matrix3( Number[] data ){
		this();
		
		if( data.length < 2 )
			throw new IllegalArgumentException();
		
		for( int i = 0; i < 2; i++ ){
			this.data[i] = data[i].floatValue();
		}
	}
	
	public Matrix3( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
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
		this.data[15] = 0.0f;
	}
	
	public float getValue( int row, int column ){
		return this.data[row*4+column];
	}
	
	public float[] getData(){
		return this.data;
	}
}
