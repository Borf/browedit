package com.exnw.browedit.render;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Observable;
import java.util.Observer;

import javax.media.opengl.GL;
import javax.media.opengl.GL2;
import javax.media.opengl.GL4;
import javax.media.opengl.GLException;

import com.exnw.browedit.data.Gat;
import com.exnw.browedit.data.Gat.GatCell;
import com.exnw.browedit.data.events.GatChange;
import com.exnw.browedit.grflib.GrfLib;
import com.exnw.browedit.math.Vector2;
import com.exnw.browedit.math.Vector3;
import com.exnw.browedit.renderutils.Vbo;
import com.exnw.browedit.renderutils.VertexList;
import com.exnw.browedit.renderutils.vertexFormats.VertexPNT;
import com.jogamp.common.nio.Buffers;
import com.jogamp.opengl.util.texture.Texture;
import com.jogamp.opengl.util.texture.TextureIO;

public class GatRenderer implements Observer, Renderer
{
	Gat gat;
	static Texture texture = null;	//same for all gat renderers
	Vbo<VertexPNT> vbo;
	ArrayList changes = new ArrayList();
	
	public GatRenderer(Gat gat)
	{
		this.gat = gat;
		this.gat.addObserver(this);
	}
	
	public void generateVbos(GL gl)
	{
		vbo = new Vbo<VertexPNT>();
		
		VertexList<VertexPNT> vertexList = new VertexList<VertexPNT>();
		
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
				Vector3 normal = new Vector3(0,1,0);
				
				vertexList.add(new VertexPNT(new Vector3(5*x+0, -cell.getHeight()[0], 5*(gat.getHeight()-y)+5), normal, new Vector2(tx0,ty0)));
				vertexList.add(new VertexPNT(new Vector3(5*x+5, -cell.getHeight()[1], 5*(gat.getHeight()-y)+5), normal, new Vector2(tx1,ty0)));
				vertexList.add(new VertexPNT(new Vector3(5*x+5, -cell.getHeight()[3], 5*(gat.getHeight()-y)+0), normal, new Vector2(tx1,ty1)));
				vertexList.add(new VertexPNT(new Vector3(5*x+0, -cell.getHeight()[2], 5*(gat.getHeight()-y)+0), normal, new Vector2(tx0,ty1)));
			}
		}
		vbo.generate(vertexList);
	}
	
	
	public void render(GL4 gl)
	{
		if(this.vbo == null)
			this.generateVbos(gl);
		if(GatRenderer.texture == null)
			GatRenderer.generateTexture(gl);
		
		if(!changes.isEmpty())
		{
			for(Object arg : changes)
			{
				if(arg instanceof GatChange)
				{
					GatChange change = (GatChange)arg;
					GatCell cell = gat.getCell(change.x, change.y);
					
					int textureIndex = cell.getType().ordinal();
					
					float tw = 0.25f;
					float th = 0.25f;
					float tx0 = (textureIndex%4)*tw;
					float ty0 = (textureIndex/4)*th;
					float tx1 = tx0+tw;
					float ty1 = ty0+th;
					Vector3 normal = new Vector3(0,1,0);				
					vbo.put((change.x*gat.getHeight() + change.y) * 4+0, new VertexPNT(new Vector3(5*change.x+0, -cell.getHeight()[0], 5*(gat.getHeight()-change.y)+5), normal, new Vector2(tx0,ty0)));
					vbo.put((change.x*gat.getHeight() + change.y) * 4+1, new VertexPNT(new Vector3(5*change.x+5, -cell.getHeight()[1], 5*(gat.getHeight()-change.y)+5), normal, new Vector2(tx1,ty0)));
					vbo.put((change.x*gat.getHeight() + change.y) * 4+2, new VertexPNT(new Vector3(5*change.x+5, -cell.getHeight()[3], 5*(gat.getHeight()-change.y)+0), normal, new Vector2(tx1,ty1)));
					vbo.put((change.x*gat.getHeight() + change.y) * 4+3, new VertexPNT(new Vector3(5*change.x+0, -cell.getHeight()[2], 5*(gat.getHeight()-change.y)+0), normal, new Vector2(tx0,ty1)));
				}
				else
				{
					System.out.println("uhoh");
				}
			}			
			changes.clear();
		}
		
		
		gl.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA);
		gl.glEnable(GL.GL_BLEND);
		
		vbo.bind();
		texture.bind(gl);
		gl.glEnable(GL.GL_TEXTURE_2D);
		
		vbo.setPointers();
		gl.glDrawArrays(GL2.GL_QUADS, 0, vbo.size()/Buffers.SIZEOF_FLOAT/8);

		gl.glDisableClientState(GL2.GL_VERTEX_ARRAY);
		gl.glDisableClientState(GL2.GL_TEXTURE_COORD_ARRAY);
		gl.glDisableClientState(GL2.GL_NORMAL_ARRAY);
		
		gl.glBindBuffer(GL.GL_ARRAY_BUFFER, 0);

	}

	private static void generateTexture(GL4 gl)
	{
		try
		{
			texture = TextureIO.newTexture(GrfLib.openFile("gat.png"), true, "png");
			texture.setTexParameteri(gl, GL.GL_TEXTURE_MIN_FILTER, GL.GL_NEAREST);
			texture.setTexParameteri(gl, GL.GL_TEXTURE_MAG_FILTER, GL.GL_NEAREST);
			texture.setTexParameteri(gl, GL.GL_TEXTURE_WRAP_S, GL2.GL_CLAMP_TO_BORDER);
			texture.setTexParameteri(gl, GL.GL_TEXTURE_WRAP_T, GL2.GL_CLAMP_TO_BORDER);
			
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
			changes.add(arg);
		
	}

}
