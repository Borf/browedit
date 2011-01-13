package com.exnw.browedit.renderutils;

import java.nio.FloatBuffer;
import java.util.ArrayList;

public class VertexList extends ArrayList<Vertex>
{
	private static final long serialVersionUID = -5787888522086426390L;

	public VertexList()
	{
		super();
	}
	
	public FloatBuffer GenerateFloatBuffer()
	{
		FloatBuffer buffer = FloatBuffer.allocate(this.size());
		int offset = 0;
		for(Vertex v : this)
		{
			v.fillBuffer(buffer, offset);
			offset += v.getSize();
		}
		return buffer;
	}
}
