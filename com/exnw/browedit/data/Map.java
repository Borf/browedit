package com.exnw.browedit.data;

import javax.media.opengl.GL;

import com.exnw.browedit.render.GatRenderer;
import com.exnw.browedit.render.GndRenderer;
import com.exnw.browedit.render.RswRenderer;

public class Map
{
	private Gat gat;
	private Gnd gnd;
	private Rsw rsw;
	
	public Rsw getRsw()
	{
		return rsw;
	}
	
	private GatRenderer gatRenderer;
	private GndRenderer gndRenderer;
	private RswRenderer rswRenderer;
	
	public Map(String fileName)
	{
		if(fileName.toLowerCase().endsWith(".rsw"))
			fileName = fileName.substring(0, fileName.lastIndexOf("."));
		
		setRsw(new Rsw(fileName + ".rsw"));
		setGnd(new Gnd(fileName + ".gnd"));
		setGat(new Gat(fileName + ".gat"));
	}
	
	public void render(GL gl)
	{
		gatRenderer.render(gl);
		//gndRenderer.render(gl);
		//rswRenderer.render(gl);
	}

	public void setGat(Gat gat)
	{
		this.gat = gat;
		this.gatRenderer = new GatRenderer(this.gat);
	}
	public void setRsw(Rsw rsw)
	{
		this.rsw = rsw;
		this.rswRenderer = new RswRenderer(rsw,this);
	}
	public void setGnd(Gnd gnd)
	{
		this.gnd = gnd;
		this.gndRenderer = new GndRenderer(this.gnd);
	}		

	public Gat getGat()
	{
		return gat;
	}
	public Gnd getGnd()
	{
		return gnd;
	}


}
