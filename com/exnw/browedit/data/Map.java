package com.exnw.browedit.data;

import javax.media.opengl.GL;

public class Map
{
	private Gat gat;
	private Gnd gnd;
	private Rsw rsw;
	
	public Map(String fileName)
	{
		if(fileName.toLowerCase().endsWith(".rsw"))
			fileName = fileName.substring(0, fileName.lastIndexOf("."));
		
//		rsw = new Rsw(fileName + ".rsw");
	//	gnd = new Gnd(fileName + ".gnd");
		setGat(new Gat(fileName + ".gat"));
	}
	
	public void render(GL gl)
	{
		getGat().render(gl);
		
	}

	public void setGat(Gat gat)
	{
		this.gat = gat;
	}

	public Gat getGat()
	{
		return gat;
	}
}
