package com.exnw.browedit.render;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;

import javax.media.opengl.GL;

import com.exnw.browedit.grflib.GrfLib;

public class Shader
{
	int shaderprogram;
	String vsrc;
	String fsrc;
	String vertexShaderFileName;
	String fragmentShaderFileName;
	
	
	public Shader(String vertexShader, String fragmentShader)
	{
		this.vertexShaderFileName = vertexShader;
		this.fragmentShaderFileName = fragmentShader;
		try
		{
			BufferedReader brv = new BufferedReader(new InputStreamReader(GrfLib.openFile(vertexShader)));
			vsrc = "";
			String line;
			while ((line=brv.readLine()) != null) {
			  vsrc += line + "\n";
			}
	
			BufferedReader brf = new BufferedReader(new InputStreamReader(GrfLib.openFile(fragmentShader)));
			fsrc = "";
			while ((line=brf.readLine()) != null) {
			  fsrc += line + "\n";
			}
		}
		catch(IOException e)
		{
			e.printStackTrace();
		}
	}
	
	void compile(GL gl)
	{
		int v = gl.glCreateShader(GL.GL_VERTEX_SHADER);
		int f = gl.glCreateShader(GL.GL_FRAGMENT_SHADER);
		gl.glShaderSource(v, 1, new String[]{vsrc}, IntBuffer.wrap(new int[] { vsrc.length() }));
		gl.glCompileShader(v);
		printShaderInfoLog(gl,v,vertexShaderFileName);
		gl.glShaderSource(f, 1, new String[]{fsrc}, IntBuffer.wrap(new int[] { fsrc.length() }));
		gl.glCompileShader(f);
		printShaderInfoLog(gl,f, fragmentShaderFileName);

		shaderprogram = gl.glCreateProgram();
		gl.glAttachShader(shaderprogram, v);
		gl.glAttachShader(shaderprogram, f);
	
		gl.glLinkProgram(shaderprogram);
		gl.glValidateProgram(shaderprogram);

		
		gl.glUseProgram(shaderprogram);		

	}
	
	
	public void printShaderInfoLog(GL gl, int shader, String filename)
	{
		IntBuffer b = IntBuffer.allocate(2);
		gl.glGetShaderiv(shader, GL.GL_INFO_LOG_LENGTH,b);
		
		if (b.get(0) > 1)
		{
			byte[] info = new byte[b.get(0)];
			gl.glGetShaderInfoLog(shader, b.get(0), b, ByteBuffer.wrap(info));
			System.err.println("Error in " + filename + "\n" + new String(info));
		}
	}	
	
	
	public void use(GL gl)
	{
		if(shaderprogram == 0)
			compile(gl);
		gl.glUseProgram(shaderprogram);		
		int location1, location2, location3;
		location1=gl.glGetUniformLocation(shaderprogram, "Texture0"); 
		location2=gl.glGetUniformLocation(shaderprogram, "Texture1"); 
		location3=gl.glGetUniformLocation(shaderprogram, "Texture2"); 
		gl.glUniform1i(location1, 0);
		gl.glUniform1i(location2, 1);
		gl.glUniform1i(location3, 2);
		
	}
}
