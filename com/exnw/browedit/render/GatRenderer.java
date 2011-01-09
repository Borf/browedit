package com.exnw.browedit.render;

import java.io.IOException;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.Observable;
import java.util.Observer;

import javax.media.opengl.GL;
import javax.media.opengl.GLException;

import com.exnw.browedit.data.Gat;
import com.exnw.browedit.data.Gat.GatCell;
import com.exnw.browedit.grflib.GrfLib;
import com.sun.opengl.util.BufferUtil;
import com.sun.opengl.util.texture.Texture;
import com.sun.opengl.util.texture.TextureIO;

public class GatRenderer implements Observer, Renderer
{
	Gat gat;
	IntBuffer vbos;
	static Texture texture = null;	//same for all gat renderers
	
	
	public GatRenderer(Gat gat)
	{
		this.gat = gat;
		this.gat.addObserver(this);
	}
	
	public void generateVbos(GL gl)
	{
		vbos = IntBuffer.allocate(2);
		gl.glGenBuffers(2, vbos);		// vertices
											// texture coordinats
		
		FloatBuffer vertices = FloatBuffer.allocate(gat.getWidth()*gat.getHeight()*4*3);
		FloatBuffer texVertices = FloatBuffer.allocate(gat.getWidth()*gat.getHeight()*4*2);
		int vertexIndex = 0;
		int texIndex = 0;
		
		for(int x = 0; x < gat.getWidth(); x++)
		{
			for(int y = 0; y < gat.getHeight(); y++)
			{
				GatCell cell = gat.getCell(x,y);
				int textureIndex = cell.getType().ordinal();
				
				float tw = 0.25f;
				float th = 0.25f;
				float tx0 = (textureIndex%4)*tw;
				float ty0 = (textureIndex/4)*th;
				float tx1 = tx0+tw;
				float ty1 = ty0+th;

				vertices.put(vertexIndex+0, 	5*x+0);
				vertices.put(vertexIndex+1,		-cell.getHeight()[0]);
				vertices.put(vertexIndex+2,		10*(gat.getHeight()-y)+5);
				texVertices.put(texIndex+0,		tx0);
				texVertices.put(texIndex+1,		ty0);
				
				vertices.put(vertexIndex+3, 	5*x+5);
				vertices.put(vertexIndex+4,		-cell.getHeight()[1]);
				vertices.put(vertexIndex+5,		10*(gat.getHeight()-y)+5);
				texVertices.put(texIndex+2,		tx1);
				texVertices.put(texIndex+3,		ty0);
				
				vertices.put(vertexIndex+6, 	5*x+5);
				vertices.put(vertexIndex+7,		-cell.getHeight()[3]);
				vertices.put(vertexIndex+8,		5*(gat.getHeight()-y)+0);
				texVertices.put(texIndex+4,		tx1);
				texVertices.put(texIndex+5,		ty1);
				
				vertices.put(vertexIndex+9, 	5*x+0);
				vertices.put(vertexIndex+10,	-cell.getHeight()[2]);
				vertices.put(vertexIndex+11,	5*(gat.getHeight()-y)+0);
				texVertices.put(texIndex+6,		tx0);
				texVertices.put(texIndex+7,		ty1);

				vertexIndex+=12;
				texIndex+=8;
			}
		}		
		gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbos.get(0));
		gl.glBufferData(GL.GL_ARRAY_BUFFER, vertices.limit()*BufferUtil.SIZEOF_FLOAT, vertices, GL.GL_STATIC_DRAW);

		gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbos.get(1));
		gl.glBufferData(GL.GL_ARRAY_BUFFER, texVertices.limit()*BufferUtil.SIZEOF_FLOAT, texVertices, GL.GL_STATIC_DRAW);		

	}
	
	
	public void render(GL gl)
	{
		if(this.vbos == null)
			this.generateVbos(gl);
		if(GatRenderer.texture == null)
			GatRenderer.generateTexture();
		
		texture.bind();
		gl.glEnable(GL.GL_TEXTURE_2D);
		
		gl.glEnableClientState(GL.GL_VERTEX_ARRAY);             // activate vertex coords array
		gl.glEnableClientState(GL.GL_TEXTURE_COORD_ARRAY);             // activate vertex coords array
		
		gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbos.get(0));         // for vertex coordinates
		gl.glVertexPointer(3, GL.GL_FLOAT, 0, 0);

		gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbos.get(1));         // for vertex coordinates
		gl.glTexCoordPointer(2, GL.GL_FLOAT, 0, 0);

		gl.glDrawArrays(GL.GL_QUADS, 0, gat.getWidth()*gat.getHeight()*4);
		
		gl.glDisableClientState(GL.GL_VERTEX_ARRAY);
		gl.glDisableClientState(GL.GL_TEXTURE_COORD_ARRAY);
		
		gl.glBindBuffer(GL.GL_ARRAY_BUFFER, 0);
		gl.glBindBuffer(GL.GL_ELEMENT_ARRAY_BUFFER, 0);			

	}

	private static void generateTexture()
	{
		try
		{
			texture = TextureIO.newTexture(GrfLib.openFile("gat.png"), true, "png");
			texture.setTexParameteri(GL.GL_TEXTURE_MIN_FILTER, GL.GL_NEAREST);
			texture.setTexParameteri(GL.GL_TEXTURE_MAG_FILTER, GL.GL_NEAREST);
			texture.setTexParameteri(GL.GL_TEXTURE_WRAP_S, GL.GL_CLAMP_TO_BORDER);
			texture.setTexParameteri(GL.GL_TEXTURE_WRAP_T, GL.GL_CLAMP_TO_BORDER);
			
		} catch (GLException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void update(Observable o, Object arg)
	{
		if(o.equals(gat))
		{
			this.vbos = null; // TODO: eep! this leaks!
		}
		
	}
	
}
