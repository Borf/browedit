package com.exnw.browedit.renderutils;

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

	public void set(Matrix4 matrix)
	{
		valueMatrix4 = matrix;
		shader.gl.glUniformMatrix4fv(location, 1, false, matrix.getData(), 0);
	}
	public void set(Matrix3 matrix)
	{
		valueMatrix3 = matrix;
		shader.gl.glUniformMatrix3fv(location, 1, false, matrix.getData(), 0);
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