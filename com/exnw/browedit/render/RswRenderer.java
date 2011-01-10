package com.exnw.browedit.render;

import java.util.ArrayList;
import java.util.List;
import java.util.Observable;

import javax.media.opengl.GL;

import com.exnw.browedit.data.Map;
import com.exnw.browedit.data.Rsw;


public class RswRenderer implements Renderer
{
	List<RsmRenderer> models; 
	Rsw rsw;
	Map map;
	
	public RswRenderer(Rsw rsw, Map map)
	{
		this.rsw = rsw;
		this.map = map;
	}
	
	public void render(GL gl)
	{
		if(models == null)
			loadModels(gl);
		

		gl.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA);
		gl.glEnable(GL.GL_BLEND);
		gl.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA);
		gl.glPushMatrix();
		gl.glScalef(1,1,-1);
		for(RsmRenderer model : models)
			model.render(gl);
		gl.glPopMatrix();
		gl.glDisable(GL.GL_BLEND);
	}
	
	public void loadModels(GL gl)
	{
		models = new ArrayList<RsmRenderer>();
		int i = 0;

		for(Rsw.ModelResource model : rsw.getModels())
		{
			models.add(new RsmRenderer(model, map));
		}
	}
	
	
	public void update(Observable arg0, Object arg1)
	{
	}
}
