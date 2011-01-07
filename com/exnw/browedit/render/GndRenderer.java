package com.exnw.browedit.render;

import java.io.IOException;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Observable;

import javax.media.opengl.GL;
import javax.media.opengl.GLException;

import com.exnw.browedit.data.Gnd;
import com.exnw.browedit.data.Gnd.GndCell;
import com.exnw.browedit.data.Gnd.Surface;
import com.exnw.browedit.grflib.GrfLib;
import com.sun.opengl.util.BufferUtil;
import com.sun.opengl.util.texture.Texture;
import com.sun.opengl.util.texture.TextureIO;

public class GndRenderer implements Renderer
{
	Gnd gnd;
	
	ArrayList<Texture> textures = null;
	int[]				vertexCounts = null;
	IntBuffer vbos = null;
	
	
	public GndRenderer(Gnd gnd)
	{
		this.gnd = gnd;
	}
	public void render(GL gl)
	{
		if(textures == null)
			this.loadTextures();
		if(vbos == null)
			this.generateVbos(gl);
		
		gl.glEnable(GL.GL_TEXTURE_2D);
		for(int i = 0; i < textures.size(); i++)
		{
			textures.get(i).bind();

			gl.glEnableClientState(GL.GL_VERTEX_ARRAY);             // activate vertex coords array
			gl.glEnableClientState(GL.GL_TEXTURE_COORD_ARRAY);             // activate vertex coords array
			
			gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbos.get(2*i));         // for vertex coordinates
			gl.glVertexPointer(3, GL.GL_FLOAT, 0, 0);

			gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbos.get(2*i+1));         // for vertex coordinates
			gl.glTexCoordPointer(2, GL.GL_FLOAT, 0, 0);

			gl.glDrawArrays(GL.GL_QUADS, 0, vertexCounts[i]);
			
			gl.glDisableClientState(GL.GL_VERTEX_ARRAY);
			gl.glDisableClientState(GL.GL_TEXTURE_COORD_ARRAY);
			
			gl.glBindBuffer(GL.GL_ARRAY_BUFFER, 0);
			gl.glBindBuffer(GL.GL_ELEMENT_ARRAY_BUFFER, 0);			
		}
		
		
	}
	
	
	
	
	
	private void generateVbos(GL gl)
	{
		vbos = IntBuffer.allocate(textures.size()*2); // vertices, texturecoords
		gl.glGenBuffers(textures.size()*2, vbos);

		vertexCounts = new int[textures.size()];
		
		for(int i = 0; i < textures.size(); i++)
		{
			ArrayList<Float> vertices = new ArrayList<Float>();
			ArrayList<Float> textureCoords = new ArrayList<Float>();
			
			for(int x = 0; x < gnd.getWidth(); x++)
			{
				for(int y = 0; y < gnd.getHeight(); y++)
				{
					GndCell cell = gnd.getCell(x, y);
					int[] surfaces = cell.getSurface();
					if(surfaces[0] != -1)
					{
						Surface surface = gnd.getSurfaces().get(surfaces[0]);
						if(surface.getTextureID() == i)
						{
							vertices.add(10.0f*x);
							vertices.add(-cell.getHeight()[2]);
							vertices.add(10.0f*(gnd.getHeight()-y));//tr
							textureCoords.add(surface.getU()[2]);
							textureCoords.add(surface.getV()[2]);
		
							vertices.add(10.0f*x);
							vertices.add(-cell.getHeight()[0]);
							vertices.add(10.0f*(gnd.getHeight()-y)+10);//tl
							textureCoords.add(surface.getU()[0]);
							textureCoords.add(surface.getV()[0]);
		
							vertices.add(10.0f*x+10);
							vertices.add(-cell.getHeight()[1]);
							vertices.add(10.0f*(gnd.getHeight()-y)+10);//bl
							textureCoords.add(surface.getU()[1]);
							textureCoords.add(surface.getV()[1]);
		
							vertices.add(10.0f*x+10);
							vertices.add(-cell.getHeight()[3]);
							vertices.add(10.0f*(gnd.getHeight()-y));//br
							textureCoords.add(surface.getU()[3]);
							textureCoords.add(surface.getV()[3]);
						}
					}
					if(surfaces[2] != -1)
					{
						Surface surface = gnd.getSurfaces().get(surfaces[2]);
						if(surface.getTextureID() == i)
						{
							GndCell otherCell = gnd.getCell(x+1, y);
								
							vertices.add(10.0f*x+10.0f);
							vertices.add(-otherCell.getHeight()[2]);
							vertices.add(10.0f*(gnd.getHeight()-y));//tr
							textureCoords.add(surface.getU()[2]);
							textureCoords.add(surface.getV()[2]);
		
							vertices.add(10.0f*x+10.0f);
							vertices.add(-otherCell.getHeight()[0]);
							vertices.add(10.0f*(gnd.getHeight()-y)+10);//tl
							textureCoords.add(surface.getU()[3]);
							textureCoords.add(surface.getV()[3]);
		
							vertices.add(10.0f*x+10.0f);
							vertices.add(-cell.getHeight()[1]);
							vertices.add(10.0f*(gnd.getHeight()-y)+10);//bl
							textureCoords.add(surface.getU()[1]);
							textureCoords.add(surface.getV()[1]);
		
							vertices.add(10.0f*x+10.0f);
							vertices.add(-cell.getHeight()[3]);
							vertices.add(10.0f*(gnd.getHeight()-y));//br
							textureCoords.add(surface.getU()[0]);
							textureCoords.add(surface.getV()[0]);
						}
					}
					if(surfaces[1] != -1) // front surfaces
					{
						Surface surface = gnd.getSurfaces().get(surfaces[1]);
						if(surface.getTextureID() == i)
						{
							GndCell otherCell = gnd.getCell(x, y+1);
							vertices.add(10.0f*x);
							vertices.add(-otherCell.getHeight()[2]);
							vertices.add(10.0f*(gnd.getHeight()-y));//tr
							textureCoords.add(surface.getU()[2]);
							textureCoords.add(surface.getV()[2]);
		
							vertices.add(10.0f*x);
							vertices.add(-cell.getHeight()[0]);
							vertices.add(10.0f*(gnd.getHeight()-y));//tl
							textureCoords.add(surface.getU()[0]);
							textureCoords.add(surface.getV()[0]);
		
							vertices.add(10.0f*x+10);
							vertices.add(-cell.getHeight()[1]);
							vertices.add(10.0f*(gnd.getHeight()-y));//bl
							textureCoords.add(surface.getU()[1]);
							textureCoords.add(surface.getV()[1]);
		
							vertices.add(10.0f*x+10);
							vertices.add(-otherCell.getHeight()[3]);
							vertices.add(10.0f*(gnd.getHeight()-y));//br
							textureCoords.add(surface.getU()[3]);
							textureCoords.add(surface.getV()[3]);
						}
					}
				}
			}
			
			vertexCounts[i] = vertices.size()/3;
			
			FloatBuffer vertexBuffer = FloatBuffer.allocate(vertices.size());
			for(int ii = 0; ii < vertices.size(); ii++)
				vertexBuffer.put(ii, vertices.get(ii).floatValue());

			FloatBuffer texCoordBuffer = FloatBuffer.allocate(textureCoords.size());
			for(int ii = 0; ii < textureCoords.size(); ii++)
				texCoordBuffer.put(ii, textureCoords.get(ii).floatValue());
	
			
			gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbos.get(i*2));
			gl.glBufferData(GL.GL_ARRAY_BUFFER, vertexBuffer.limit()*BufferUtil.SIZEOF_FLOAT, vertexBuffer, GL.GL_STATIC_DRAW);

			gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbos.get(i*2+1));
			gl.glBufferData(GL.GL_ARRAY_BUFFER, texCoordBuffer.limit()*BufferUtil.SIZEOF_FLOAT, texCoordBuffer, GL.GL_STATIC_DRAW);
			
		}
		
		
	}
	private void loadTextures()
	{
		{
			textures = new ArrayList<Texture>();
			List<String> TextureFileNames = gnd.getTextures();
			for(String s : TextureFileNames)
			{
				try
				{
					Texture texture = TextureIO.newTexture(GrfLib.openFile("data\\texture\\" + s), true, s.substring(s.lastIndexOf('.')));
					//TODO: fix borders
					texture.setTexParameteri(GL.GL_TEXTURE_WRAP_S, GL.GL_MIRRORED_REPEAT);
					texture.setTexParameteri(GL.GL_TEXTURE_WRAP_T, GL.GL_MIRRORED_REPEAT);
					
					textures.add(texture);
				} catch (GLException e)
				{
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IOException e)
				{
					System.err.println("Could not open: " + s);
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}
	public void update(Observable arg0, Object arg1)
	{
		
	}

}
