package com.exnw.browedit.data;

import javax.media.opengl.GL;

import com.exnw.browedit.render.GatRenderer;
import com.exnw.browedit.render.GndRenderer;

public class Map
{
	private Gat gat;
	private Gnd gnd;


	private Rsw rsw;
	private GatRenderer gatRenderer;
	private GndRenderer gndRenderer;
	
	public Map(String fileName)
	{
		if(fileName.toLowerCase().endsWith(".rsw"))
			fileName = fileName.substring(0, fileName.lastIndexOf("."));
		
//		rsw = new Rsw(fileName + ".rsw");
		setGnd(new Gnd(fileName + ".gnd"));
		setGat(new Gat(fileName + ".gat"));
	}
	
	public void render(GL gl)
	{
//		gatRenderer.render(gl);
		gndRenderer.render(gl);
	}

	public void setGat(Gat gat)
	{
		this.gat = gat;
		this.gatRenderer = new GatRenderer(this.gat);
	}

	public Gat getGat()
	{
		return gat;
	}
	public Gnd getGnd()
	{
		return gnd;
	}

	public void setGnd(Gnd gnd)
	{
		this.gnd = gnd;
		this.gndRenderer = new GndRenderer(this.gnd);
	}	
}
