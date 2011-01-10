package com.exnw.browedit.render;

import java.io.IOException;
import java.util.HashMap;

import javax.media.opengl.GLException;

import com.exnw.browedit.grflib.GrfLib;
import com.sun.opengl.util.texture.Texture;
import com.sun.opengl.util.texture.TextureIO;

public class TextureCache
{
	private static HashMap<String, Texture> textures = new HashMap<String, Texture>();
	
	public static Texture getTexture(String fileName)
	{
		if(!textures.containsKey(fileName))
		{
			try
			{
				textures.put(fileName, TextureIO.newTexture(GrfLib.openFile(fileName), false, fileName.substring(fileName.lastIndexOf('.'))));
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
		return textures.get(fileName);
	}
}
