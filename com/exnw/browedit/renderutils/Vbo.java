package com.exnw.browedit.renderutils;

import java.nio.FloatBuffer;
import java.nio.IntBuffer;

import javax.media.opengl.GL;
import javax.media.opengl.GLContext;

import com.sun.opengl.util.BufferUtil;

public class Vbo
{
	private IntBuffer vbo;
	private int length;
	
	public Vbo()
	{
		GL gl = GLContext.getCurrent().getGL();
		vbo = IntBuffer.allocate(1);
		gl.glGenBuffers(1, vbo);
		length = 0;
	}
	
	public void dispose()
	{
		GL gl = GLContext.getCurrent().getGL();
		gl.glDeleteBuffers(1, vbo);
	}
	
	public void bind()
	{
		GL gl = GLContext.getCurrent().getGL();
		gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbo.get(0));
	}
	
	public void generate(FloatBuffer buffer)
	{
		this.bind();
		GL gl = GLContext.getCurrent().getGL();
		//glBufferData automatically throws away the old one
		gl.glBufferData(GL.GL_ARRAY_BUFFER, buffer.limit()*BufferUtil.SIZEOF_FLOAT, buffer, GL.GL_STATIC_DRAW);
	}
	public void put(int index, Vertex v)
	{
		this.bind();
		GL gl = GLContext.getCurrent().getGL();

		VertexList tmpList = new VertexList();
		tmpList.add(v);
		
		gl.glBufferSubData(GL.GL_ARRAY_BUFFER, index * (v.getSize()*BufferUtil.SIZEOF_FLOAT), v.getSize()*BufferUtil.SIZEOF_FLOAT, tmpList.GenerateFloatBuffer());
	}
	
	
	public FloatBuffer getMap()
	{
		this.bind();
		GL gl = GLContext.getCurrent().getGL();
		return gl.glMapBuffer(GL.GL_ARRAY_BUFFER, GL.GL_WRITE_ONLY).asFloatBuffer();
	}
	
	public void unMap()
	{
		this.bind();
		GL gl = GLContext.getCurrent().getGL();
		gl.glUnmapBuffer(GL.GL_ARRAY_BUFFER);
	}
}
