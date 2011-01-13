package com.exnw.browedit.renderutils.vertexFormats;

import java.awt.Color;
import java.nio.FloatBuffer;

import com.exnw.browedit.math.Vector2;
import com.exnw.browedit.math.Vector3;

public class VertexPNCT extends VertexPNC
{
	Vector2 textureCoord1;
	public VertexPNCT(Vector3 position, Vector3 normal, Color color, Vector2 textureCoord1)
	{
		super(position, normal, color);
		this.textureCoord1 = textureCoord1;
	}
	@Override
	public int getSize()
	{
		return super.getSize()+2;
	}
	@Override
	public void fillBuffer(FloatBuffer buffer, int offset)
	{
		super.fillBuffer(buffer, offset);
		for(int i = 0; i < 2; i++)
			buffer.put(offset+super.getSize()+i, textureCoord1.getData()[i]);		
	}
}
