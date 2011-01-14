package com.exnw.browedit.data;

import java.util.HashMap;

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
}
