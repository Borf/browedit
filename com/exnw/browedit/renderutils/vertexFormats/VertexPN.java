package com.exnw.browedit.renderutils.vertexFormats;

import java.nio.FloatBuffer;

import com.exnw.browedit.math.Vector3;
import com.exnw.browedit.renderutils.Vertex;

public class VertexPN implements Vertex
{
	Vector3 position;
	Vector3 normal;

	public VertexPN(Vector3 position, Vector3 normal)
	{
		super();
		this.position = position;
		this.normal = normal;
	}

	@Override
	public int getSize()
	{
		return 3+3;
	}

	@Override
	public void fillBuffer(FloatBuffer buffer, int offset)
	{
		for(int i = 0; i < 3; i++)
			buffer.put(offset+0+i, position.getData()[i]);
		for(int i = 0; i < 3; i++)
			buffer.put(offset+3+i, normal.getData()[i]);
	}

}
