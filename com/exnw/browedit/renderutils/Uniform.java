package com.exnw.browedit.renderutils;

import javax.media.opengl.GL4;

import com.exnw.browedit.math.Matrix3;
import com.exnw.browedit.math.Matrix4;
import com.exnw.browedit.math.Vector2;
import com.exnw.browedit.math.Vector3;

public class Uniform
{
	String name;
	Shader shader;
	int location;
	
	public int 	valueInt;
	public boolean valueBool;
	public float	valueFloat;
	public Matrix4 valueMatrix4;
	public Matrix3 valueMatrix3;
	public Vector3 valueVector3;
	public Vector2 valueVector2;
	
	public Uniform(Shader shader, int location, String name)
	{
		this.shader = shader;
		this.location = location;
		this.name = name;
	}

	public void set(Matrix4 matrix, GL4 gl)
	{
		valueMatrix4 = matrix;
		gl.glUniformMatrix4fv(location, 1, false, matrix.getData(), 0);
	}
	public void set(Matrix3 matrix, GL4 gl)
	{
		valueMatrix3 = matrix;
		gl.glUniformMatrix3fv(location, 1, false, matrix.getData(), 0);
	}
	public void set(int value, GL4 gl)
	{
		valueInt = value;
		gl.glUniform1i(location, value);
	}
	public void set(float value, GL4 gl)
	{
		valueFloat = value;
		gl.glUniform1f(location, value);
	}
	public void set(Vector3 value, GL4 gl)
	{
		valueVector3 = value;
		gl.glUniform3f(location, value.getX(), value.getY(), value.getZ());
	}
	public void set(Vector2 value, GL4 gl)
	{
		valueVector2 = value;
		gl.glUniform2f(location, value.getX(), value.getY());
	}
	public void set(boolean value, GL4 gl)
	{
		valueBool = value;
		gl.glUniform1i(location, value ? 1 : 0);
	}
	
}