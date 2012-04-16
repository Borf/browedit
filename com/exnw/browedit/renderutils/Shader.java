package com.exnw.browedit.renderutils;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import java.util.HashMap;
import java.util.Map;

import javax.media.opengl.GL;
import javax.media.opengl.GL4;

import com.exnw.browedit.grflib.GrfLib;

public class Shader
{
	int shaderprogram;
	String vsrc;
	String fsrc;
	String vertexShaderFileName;
	String fragmentShaderFileName;
	
	GL4 gl;
	
	
	Map<String, Uniform> uniforms = new HashMap<String, Uniform>();
	
	
	public Shader(String vertexShader, String fragmentShader, GL gl)
	{
		this.gl = gl.getGL4();
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
	
	void compile()
	{
		int v = gl.glCreateShader(GL4.GL_VERTEX_SHADER);
		int f = gl.glCreateShader(GL4.GL_FRAGMENT_SHADER);
		gl.glShaderSource(v, 1, new String[]{vsrc}, IntBuffer.wrap(new int[] { vsrc.length() }));
		gl.glCompileShader(v);
		printShaderInfoLog(v,vertexShaderFileName);
		gl.glShaderSource(f, 1, new String[]{fsrc}, IntBuffer.wrap(new int[] { fsrc.length() }));
		gl.glCompileShader(f);
		printShaderInfoLog(f, fragmentShaderFileName);

		shaderprogram = gl.glCreateProgram();
		gl.glAttachShader(shaderprogram, v);
		gl.glAttachShader(shaderprogram, f);
	}
	
	void link()
	{
		gl.glLinkProgram(shaderprogram);
		gl.glValidateProgram(shaderprogram);
		gl.glUseProgram(shaderprogram);		
	}
	
	
	public void printShaderInfoLog(int shader, String filename)
	{
		IntBuffer b = IntBuffer.allocate(2);
		gl.glGetShaderiv(shader, GL4.GL_INFO_LOG_LENGTH,b);
		
		if (b.get(0) > 1)
		{
			byte[] info = new byte[b.get(0)];
			gl.glGetShaderInfoLog(shader, b.get(0), b, ByteBuffer.wrap(info));
			System.err.println("Error in " + filename + "\n" + new String(info));
		}
	}
	
	public Uniform getUniform(String name)
	{
		if(!uniforms.containsKey(name))
		{
			int location = getUniformLocation(name);
			if(location > 0)
				uniforms.put(name, new Uniform(this, location));
		}
		if(uniforms.containsKey(name))
			return uniforms.get(name);
		return null;
	}
	
	
	int getUniformLocation(String location)
	{
		return gl.glGetUniformLocation(shaderprogram, location);
	}
	
	void bindAttributeLocation(String name, int position)
	{
		gl.glBindAttribLocation(shaderprogram, position, name);
	}
	
	
	private String[] attributes = { "a_position", "a_texcoord", "a_normal", "a_color", "a_texcoord2" };
	
	public int getAttribLocation(String name)
	{
		for(int i = 0; i < attributes.length; i++)
			if(attributes[i].equals(name))
				return i;
		return -1;
	}
	

	public void use()
	{
		if(shaderprogram == 0)
		{
			compile();
			for(int i = 0; i < attributes.length; i++)
				bindAttributeLocation(attributes[i], i);
			link();
		}
		gl.glUseProgram(shaderprogram);		
	}
}

