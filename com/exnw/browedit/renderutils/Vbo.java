package com.exnw.browedit.renderutils;

import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;

import javax.media.opengl.GL4;
import javax.media.opengl.GLContext;

import com.jogamp.common.nio.Buffers;
import com.sun.org.apache.bcel.internal.generic.GETSTATIC;

public class Vbo<T extends Vertex>
{
	private IntBuffer vbo;
	private IntBuffer vertexArray;
	private int length; // length in bytes
	private T oneVertex;
	
	public Vbo()
	{
		GL4 gl = GLContext.getCurrent().getGL().getGL4();

		vertexArray = IntBuffer.allocate(1);
		gl.glGenVertexArrays(1, vertexArray);
		gl.glBindVertexArray(vertexArray.get(0));

		vbo = IntBuffer.allocate(1);
		gl.glGenBuffers(1, vbo);

		length = 0;
	}
	
	public void bind()
	{
		GL4 gl = GLContext.getCurrent().getGL().getGL4();
		gl.glBindVertexArray(vertexArray.get(0));
		//gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbo.get(0));
	}
	
	public void generate(VertexList<T> vertices, Shader shader)
	{
		oneVertex = vertices.get(0);
		generate(vertices.GenerateFloatBuffer(), shader);
	}
	
	private void generate(FloatBuffer buffer, Shader shader)
	{
		this.bind();
		GL4 gl = GLContext.getCurrent().getGL().getGL4();
		//glBufferData automatically throws away the old one
		
	    gl.glBindVertexArray(vertexArray.get(0));
		gl.glBindBuffer(GL4.GL_ARRAY_BUFFER, vbo.get(0));
		gl.glBufferData(GL4.GL_ARRAY_BUFFER, buffer.limit()*Buffers.SIZEOF_FLOAT, buffer, GL4.GL_DYNAMIC_DRAW);
		length = buffer.limit()*Buffers.SIZEOF_FLOAT;
		
		oneVertex.setPointers(gl, shader);
		
		gl.glBindVertexArray(0);
	}
	public void put(int index, T v)
	{
		this.bind();
		GL4 gl = GLContext.getCurrent().getGL().getGL4();

		VertexList<T> tmpList = new VertexList<T>();
		tmpList.add(v);
		
		int size = v.getSize();
		gl.glBufferSubData(GL4.GL_ARRAY_BUFFER, index * (size*Buffers.SIZEOF_FLOAT), size*Buffers.SIZEOF_FLOAT, tmpList.GenerateFloatBuffer());
	}
	
	public FloatBuffer getMap(GL4 gl)
	{
		this.bind();
		ByteBuffer buf = gl.glMapBufferRange(GL4.GL_ARRAY_BUFFER, 0, length*Buffers.SIZEOF_FLOAT, GL4.GL_READ_WRITE);
		if(buf == null)
		{
			int error = gl.glGetError();
			System.err.println("openGL error " + error);
		}
		return buf.asFloatBuffer();
	}
	
	public void unMap(GL4 gl)
	{
		this.bind();
		gl.glUnmapBuffer(GL4.GL_ARRAY_BUFFER);
	}

	public int size()
	{
		return length;
	}

	public void destroy(GL4 gl)
	{
		gl.glDeleteBuffers(1, vbo);
	}

	public int elementSize()
	{
		return oneVertex.getSize();
	}
}
