package com.exnw.browedit.renderutils;

import com.exnw.browedit.math.Matrix4;
import com.exnw.browedit.math.Vector2;
import com.exnw.browedit.math.Vector3;

public class Uniform
{
	Shader shader;
	int location;
	
	int 	valueInt;
	boolean valueBool;
	float	valueFloat;
	Matrix4 valueMatrix;
	Vector3 valueVector3;
	Vector2 valueVector2;
	
	public Uniform(Shader shader, int location)
	{
		this.shader = shader;
	}

	public void set(Matrix4 matrix)
	{
		valueMatrix = matrix;
		shader.gl.glUniformMatrix4fv(location, 4*4, false, matrix.getData(), 0);
	}
	public void set(int value)
	{
		valueInt = value;
		shader.gl.glUniform1i(location, value);
	}
	public void set(float value)
	{
		valueFloat = value;
		shader.gl.glUniform1f(location, value);
	}
	public void set(Vector3 value)
	{
		valueVector3 = value;
		shader.gl.glUniform3f(location, value.getX(), value.getY(), value.getZ());
	}
	public void set(Vector2 value)
	{
		valueVector2 = value;
		shader.gl.glUniform2f(location, value.getX(), value.getY());
	}
	public void set(boolean value)
	{
		valueBool = value;
		shader.gl.glUniform1i(location, value ? 1 : 0);
	}
	
}