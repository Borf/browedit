package com.exnw.browedit.renderutils.vertexFormats;

import java.nio.FloatBuffer;

import javax.media.opengl.GL4;

import com.exnw.browedit.math.Vector3;
import com.exnw.browedit.renderutils.Shader;
import com.exnw.browedit.renderutils.Vertex;

public class VertexP extends Vertex
{
	Vector3 position;
	Vector3 normal;
	
	public VertexP() { }

	public VertexP(Vector3 position)
	{
		super();
		this.position = position;
	}

	@Override
	public int getSize()
	{
		return 3;
	}

	@Override
	public void fillBuffer(FloatBuffer buffer, int offset)
	{
		for(int i = 0; i < 3; i++)
			buffer.put(offset+0+i, position.getData()[i]);
	}
	
	@Override
	public void setPointers(GL4 gl, Shader shader)
	{
		setAttrib(gl, shader, "a_position", 3, 0);
	}

}
