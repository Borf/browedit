package com.exnw.browedit.math;

public class Vector2{
	private float[] data;
	
	public Vector2(){
		this.data = new float[2];
	}
	
	public Vector2( Number x, Number y ){
		this();
		
		this.data[0] = x.floatValue();
		this.data[1] = y.floatValue();
	}
	
	public Vector2( Number[] data ){
		this();
		
		if( data.length < 2 )
			throw new IllegalArgumentException();
		
		for( int i = 0; i < 2; i++ ){
			this.data[i] = data[i].floatValue();
		}
	}
	
	public Vector2( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
		this();
		this.read(in);
	}
	
	public void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
		for( int i = 0; i < 2; i++ )
			this.data[i] = in.readFloat();
	}
	
	public float getX(){
		return this.data[0];
	}
	
	public float getY(){
		return this.data[1];
	}
	
	public float[] getData(){
		return this.data;
	}
}
