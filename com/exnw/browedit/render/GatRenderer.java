package com.exnw.browedit.render;

import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.Observable;
import java.util.Observer;

import javax.media.opengl.GL;

import com.exnw.browedit.data.Gat;
import com.exnw.browedit.data.Gat.GatCell;
import com.sun.opengl.util.BufferUtil;

public class GatRenderer implements Observer
{
	Gat gat;
	IntBuffer vbos;
	int		texture;
	
	
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
		IntBuffer indices = IntBuffer.allocate(gat.getWidth()*gat.getHeight()*4);
		int i = 0;
		for(int x = 0; x < gat.getWidth(); x++)
		{
			for(int y = 0; y < gat.getHeight(); y++)
			{
				GatCell cell = gat.getCell(x,y);
				gl.glColor3f(cell.getType().getColor().getRed()/255.0f, cell.getType().getColor().getGreen()/255.0f, cell.getType().getColor().getBlue()/255.0f); 

				vertices.put(i, 	x+0);
				vertices.put(i+1,	-cell.getHeight()[0]);
				vertices.put(i+2,	y+0);
				
				vertices.put(i+3, 	x+1);
				vertices.put(i+4,	-cell.getHeight()[1]);
				vertices.put(i+5,	y+0);
				
				vertices.put(i+6, 	x+1);
				vertices.put(i+7,	-cell.getHeight()[2]);
				vertices.put(i+8,	y+1);
				
				vertices.put(i+9, 	x+0);
				vertices.put(i+10,	-cell.getHeight()[3]);
				vertices.put(i+11,	y+1);
				i+=12;
			}
		}		
		gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbos.get(0));
		gl.glBufferData(GL.GL_ARRAY_BUFFER, vertices.limit()*BufferUtil.SIZEOF_FLOAT, vertices, GL.GL_STATIC_DRAW);

//		gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbos.get(1));
	//	gl.glBufferData(GL.GL_ARRAY_BUFFER, indices.limit()*BufferUtil.SIZEOF_INT, indices, GL.GL_STATIC_DRAW);		

	}
	
	
	public void render(GL gl)
	{
		if(this.vbos == null)
			this.generateVbos(gl);
		
		gl.glEnableClientState(GL.GL_VERTEX_ARRAY);             // activate vertex coords array
		
		gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbos.get(0));         // for vertex coordinates
		gl.glVertexPointer(3, GL.GL_FLOAT, 0, 0);

		gl.glDrawArrays(GL.GL_QUADS, 0, gat.getWidth()*gat.getHeight()*4);
		
		gl.glDisableClientState(GL.GL_VERTEX_ARRAY);
		gl.glBindBuffer(GL.GL_ARRAY_BUFFER, 0);
		gl.glBindBuffer(GL.GL_ELEMENT_ARRAY_BUFFER, 0);			

	}

	@Override
	public void update(Observable o, Object arg)
	{
		if(o.equals(gat))
		{
			this.vbos = null; // TODO: eep! this leaks!
		}
		
	}
	
}
