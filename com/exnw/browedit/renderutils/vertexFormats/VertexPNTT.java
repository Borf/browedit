package com.exnw.browedit.renderutils.vertexFormats;

import java.nio.FloatBuffer;

import com.exnw.browedit.math.Vector2;
import com.exnw.browedit.math.Vector3;

public class VertexPNTT extends VertexPNT
{
	Vector2 textureCoord2;
	public VertexPNTT(Vector3 position, Vector3 normal, Vector2 textureCoord1, Vector2 textureCoord2)
	{
		super(position, normal, textureCoord1);
		this.textureCoord2 = textureCoord2;
	}
	@Override
	public int getSize()
	{
		return super.getSize() + 2;
	}
	@Override
	public void fillBuffer(FloatBuffer buffer, int offset)
	{
		super.fillBuffer(buffer, offset);
		for(int i = 0; i < 2; i++)
			buffer.put(offset+super.getSize()+i, textureCoord2.getData()[i]);		
	}
	

}
