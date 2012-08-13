package com.exnw.browedit.data;

import java.io.Serializable;

public class Map implements Serializable
{
	private Gat gat;
	private Gnd gnd;
	private Rsw rsw;
	
	public Rsw getRsw()
	{
		return rsw;
	}
	
	public Map(String fileName)
	{
		if(fileName.toLowerCase().endsWith(".rsw"))
			fileName = fileName.substring(0, fileName.lastIndexOf("."));
		
		setRsw(new Rsw(fileName + ".rsw"));
		setGnd(new Gnd(fileName + ".gnd"));
		setGat(new Gat(fileName + ".gat"));
	}
	
	public void setGat(Gat gat)
	{
		this.gat = gat;
	}
	public void setRsw(Rsw rsw)
	{
		this.rsw = rsw;
	}
	public void setGnd(Gnd gnd)
	{
		this.gnd = gnd;
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
