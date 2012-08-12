package com.exnw.browedit.math;

import java.io.Serializable;

public class Vector3 implements Serializable{
	private float[] data;
	
	public Vector3(){
		this.data = new float[3];
	}
	
	public Vector3( Number x, Number y, Number z ){
		this();
		
		this.data[0] = x.floatValue();
		this.data[1] = y.floatValue();
		this.data[2] = z.floatValue();
	}
	
	public Vector3( Number[] data ){
		this();
		
		if( data.length < 3 )
			throw new IllegalArgumentException();
		
		for( int i = 0; i < 3; i++ ){
			this.data[i] = data[i].floatValue();
		}
	}
	
	public Vector3( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
		this();
		this.read(in);
	}
	
	public void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
		for( int i = 0; i < 3; i++ )
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
	
	public float[] getData(){
		return this.data;
	}
	
	public float getLength(){
		return (float) Math.sqrt(data[0]*data[0]+data[1]*data[1]+data[2]*data[2]);
	}
	
	public double getVerticalAngle()
	{
		return Math.acos(data[1]);
	}
	
	public double getHorizontalAngle()
	{
		return Math.atan2(data[2], data[0]);
	}
	
	public void rotateBy(float xRotateIncrement, float yRotateIncrement)
	{
		double hAngle = getHorizontalAngle();
		double vAngle = getVerticalAngle();
		hAngle -= xRotateIncrement;
		vAngle += yRotateIncrement;
		vAngle = Math.min(Math.max(vAngle, 0.001f),((Math.PI/2.0)-0.001f));
		data[0] = (float) (Math.cos(hAngle)*Math.sin(vAngle));
		data[1] = (float) (Math.cos(vAngle));
		data[2] = (float) (Math.sin(hAngle)*Math.sin(vAngle));
	}

	public Vector3 add(Vector3 other)
	{
		return new Vector3(data[0]+other.data[0], data[1]+other.data[1], data[2]+other.data[2]);		
	}
	
	public Vector3 sub(Vector3 other)
	{
		return new Vector3(data[0]-other.data[0], data[1]-other.data[1], data[2]-other.data[2]);		
	}
	
	public Vector3 cross(Vector3 other)
	{
		return new Vector3(
			data[1] * other.data[2] - data[2] * other.data[1],
			data[2] * other.data[0] - data[0] * other.data[2],
			data[0] * other.data[1] - data[1] * other.data[0]);
	}

	public float dot(Vector3 other)
	{
		return 	data[0] * other.data[0] + 
				data[1] * other.data[1] +
				data[2] * other.data[2];
	}
	
	public Vector3 normalized()
	{
		return div(getLength());
	}
	
	private Vector3 div(float length)
	{
		return new Vector3(data[0]/length, data[1]/length, data[2]/length);
	}

	public String toString()
	{
		return "Vector3(" + data[0] + "," + data[1] + "," + data[2] + ") ";
	}
}
