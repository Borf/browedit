package com.exnw.browedit.math;

public class Vector4{
	private float[] data;
	
	public Vector4(){
		this.data = new float[4];
	}
	
	public Vector4( Number x, Number y, Number z, Number w ){
		this();
		
		this.data[0] = x.floatValue();
		this.data[1] = y.floatValue();
		this.data[2] = z.floatValue();
		this.data[3] = w.floatValue();
 	}
	
	public Vector4( Number[] data ){
		this();
		
		if( data.length < 4 )
			throw new IllegalArgumentException();
		
		for( int i = 0; i < 4; i++ ){
			this.data[i] = data[i].floatValue();
		}
	}
	
	public Vector4( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
		this();
		this.read(in);
	}
	
	public void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
		for( int i = 0; i < 4; i++ )
			this.data[i] = in.readFloat();
	}
	
	public float getX(){
		return this.data[0];
	}
	
	public float getY(){
		return this.data[1];
	}
	
	public float getZ(){
		return this.data[2];
	}
	
	public float getW(){
		return this.data[3];
	}
	
	public float[] getData(){
		return this.data;
	}
}
