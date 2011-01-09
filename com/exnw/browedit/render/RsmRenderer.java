package com.exnw.browedit.render;

import java.util.Observable;

import javax.media.opengl.GL;

import com.exnw.browedit.data.Rsm;
import com.exnw.browedit.data.Rsw;

public class RsmRenderer implements Renderer
{
	Rsw.ModelResource modelProperties;
	Rsm rsm;
	
	RsmRenderer(Rsw.ModelResource resource)
	{
		modelProperties = resource;
		rsm = new Rsm(modelProperties.getModelname());
	}
	public void render(GL gl)
	{
	}
	public void update(Observable o, Object arg)
	{
		
	}
}
