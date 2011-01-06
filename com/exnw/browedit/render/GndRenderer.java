package com.exnw.browedit.render;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Observable;

import javax.media.opengl.GL;
import javax.media.opengl.GLException;

import com.exnw.browedit.data.Gnd;
import com.exnw.browedit.grflib.GrfLib;
import com.sun.opengl.util.texture.Texture;
import com.sun.opengl.util.texture.TextureIO;

public class GndRenderer implements Renderer
{
	Gnd gnd;
	
	ArrayList<Texture> textures = null;
	
	
	public GndRenderer(Gnd gnd)
	{
		this.gnd = gnd;
	}
	public void render(GL gl)
	{
		if(textures == null)
			this.loadTextures();
		
		gl.glEnable(GL.GL_TEXTURE_2D);
		for(int x = 0; x < gnd.getWidth(); x++)
		{
			for(int y = 0; y < gnd.getHeight(); y++)
			{
				Gnd.GndCell cell = gnd.getCell(x, y);
				int[] surfaces = cell.getSurface();
				if(surfaces[0] != -1)
				{
					Gnd.Surface surface = gnd.getSurfaces().get(surfaces[0]);
					textures.get(surface.getTextureID()).bind();
					gl.glBegin(GL.GL_QUADS);
					gl.glTexCoord2f(surface.getU()[2], surface.getV()[2]);
					gl.glVertex3f(10*x, -cell.getHeight()[2], 10*(gnd.getHeight()-y));//tr

					gl.glTexCoord2f(surface.getU()[0], surface.getV()[0]);
					gl.glVertex3f(10*x, -cell.getHeight()[0], 10*(gnd.getHeight()-y)+10);//tl

					gl.glTexCoord2f(surface.getU()[1], surface.getV()[1]);
					gl.glVertex3f(10*x+10, -cell.getHeight()[1], 10*(gnd.getHeight()-y)+10);//bl

					gl.glTexCoord2f(surface.getU()[3], surface.getV()[3]);
					gl.glVertex3f(10*x+10, -cell.getHeight()[3], 10*(gnd.getHeight()-y));//br
					gl.glEnd();
				}				
			}
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
					textures.add(TextureIO.newTexture(GrfLib.openFile("data\\texture\\" + s), true, s.substring(s.lastIndexOf('.'))));
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
		}
	}
	public void update(Observable arg0, Object arg1)
	{
		
	}

}
