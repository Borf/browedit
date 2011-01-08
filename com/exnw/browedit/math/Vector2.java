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

	public double getAngle()
	{
		return Math.atan2(data[1], data[0]);
	}
	
	public void rotateBy(float rotateIncrement)
	{
		float len = getLength();
		double angle = getAngle();
		angle += rotateIncrement;
		data[0] = (float) (len*Math.cos(angle));
		data[1] = (float) (len*Math.sin(angle));
	}

	private float getLength()
	{
		return (float) Math.sqrt(data[0]*data[0]+data[1]*data[1]);
	}
}
