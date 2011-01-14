package com.exnw.browedit.renderutils.vertexFormats;

import java.awt.Color;
import java.nio.FloatBuffer;

import javax.media.opengl.GL;
import javax.media.opengl.GLContext;

import com.exnw.browedit.math.Vector3;
import com.exnw.browedit.renderutils.Vbo;
import com.sun.opengl.util.BufferUtil;

public class VertexPNC extends VertexPN
{
	Color color;
	public VertexPNC() {}
	
	public VertexPNC(Vector3 position, Vector3 normal, Color color)
	{
		super(position, normal);
		this.color = color;
	}

	@Override
	public int getSize()
	{
		return super.getSize()+4;
	}

	@Override
	public void fillBuffer(FloatBuffer buffer, int offset)
	{
		super.fillBuffer(buffer, offset);
		buffer.put(offset+super.getSize()+0, color.getRed()/255.0f);
		buffer.put(offset+super.getSize()+1, color.getGreen()/255.0f);
		buffer.put(offset+super.getSize()+2, color.getBlue()/255.0f);
		buffer.put(offset+super.getSize()+3, color.getAlpha()/255.0f);
	}
	
	@Override
	public void setPointers(Vbo vbo)
	{
		super.setPointers(vbo);
		GL gl = GLContext.getCurrent().getGL();
		gl.glEnableClientState(GL.GL_COLOR_ARRAY);
		gl.glColorPointer(4, GL.GL_FLOAT, getSize()*BufferUtil.SIZEOF_FLOAT, super.getSize()*BufferUtil.SIZEOF_FLOAT);
	}	
}
