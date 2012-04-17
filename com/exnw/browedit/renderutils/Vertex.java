package com.exnw.browedit.renderutils;

import java.nio.FloatBuffer;

import javax.media.opengl.GL4;

import com.jogamp.common.nio.Buffers;

public abstract class Vertex
{
	public abstract int getSize();
	public abstract void fillBuffer(FloatBuffer buffer, int offset);
	public abstract void setPointers(GL4 gl, Shader shader);
	
	public void setAttrib(GL4 gl, Shader shader, String name, int len, int offset)
	{
		int loc = shader.getAttribLocation(name);
	    gl.glEnableVertexAttribArray(loc);
	    gl.glVertexAttribPointer(loc, len, GL4.GL_FLOAT, false, getSize() * Buffers.SIZEOF_FLOAT, offset * Buffers.SIZEOF_FLOAT);
		
	}

}
