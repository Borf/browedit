package com.exnw.browedit.data;

import java.util.HashMap;

import javax.media.opengl.GL4;

public class RsmCache
{
	private static HashMap<String, Rsm> models = new HashMap<String, Rsm>();
	
	public static Rsm getModel(String fileName)
	{
		if(!models.containsKey(fileName))
		{
			models.put(fileName, new Rsm(fileName));
		}
		return models.get(fileName);
	}

	public static void destroy(GL4 gl)
	{
		for(String m : models.keySet())
			models.get(m).destroy(gl);
		
	}
}
