package com.exnw.browedit.renderutils.vertexFormats;

import java.nio.FloatBuffer;

import javax.media.opengl.GL4;

import com.exnw.browedit.math.Vector2;
import com.exnw.browedit.math.Vector3;
import com.exnw.browedit.renderutils.Shader;

public class VertexPNT extends VertexPN
{
	Vector2 textureCoord1;
	
	public VertexPNT() {}
	
	public VertexPNT(Vector3 position, Vector3 normal, Vector2 textureCoord1)
	{
		super(position, normal);
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

	@Override
	public void setPointers(GL4 gl, Shader shader)
	{
		super.setPointers(gl, shader);
		setAttrib(gl, shader, "a_texcoord", 2, 6);
	}
	
	
}
