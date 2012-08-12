package com.exnw.browedit.render;

import java.util.Observable;

import javax.media.opengl.GL4;

import com.exnw.browedit.data.Map;
import com.exnw.browedit.data.RsmCache;
import com.exnw.browedit.gui.MainPanel;
import com.exnw.browedit.renderutils.Shader;

public class MapRenderer implements Renderer
{
	private GndRenderer gndRenderer;
	private RswRenderer rswRenderer;
	private MainPanel mainPanel;
	
	

	public MapRenderer(Map map, MainPanel mainPanel)
	{
		this.mainPanel = mainPanel;
		
		gndRenderer = new GndRenderer(map.getGnd());
		rswRenderer = new RswRenderer(map.getRsw(), map);
	}

	@Override
	public void update(Observable arg0, Object arg1)
	{
	}

	@Override
	public void render(GL4 gl, Shader shader)
	{
		shader.getUniform("showFloorShadow", gl).set(mainPanel.renderer.showShadow, gl);
		
		gndRenderer.render(gl,  shader);
		if(mainPanel.renderer.showObjects)
			rswRenderer.render(gl, shader);		
	}

	public void destroy(GL4 gl)
	{
		if(gndRenderer != null)
			gndRenderer.destroy(gl);
		if(rswRenderer != null)
			rswRenderer.destroy(gl);
		RsmCache.destroy(gl);
	}

}
