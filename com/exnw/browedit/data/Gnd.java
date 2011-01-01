package com.exnw.browedit.data;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;

import com.exnw.browedit.grflib.GrfLib;

public class Gnd
{
	GndTile[][] tiles;
	
	public Gnd(String fileName)
	{
		try
		{
			read(GrfLib.openFile(fileName));
		} catch (IOException e)
		{
			e.printStackTrace();
		}
	}
	
	private short readShort(InputStream in) throws IOException
	{
		int a = in.read();
		int b = in.read();
		return (short) (((a&0xFF)<<0) | ((b&0xFF)<<8));
	}
	
	private void read(InputStream in) throws IOException
	{
		byte[] header = new byte[4];
		in.read(header);
		if(!Arrays.equals(header, "GRGN".getBytes()))
			return;
		short version = readShort(in);
		System.out.printf("%x\n", version);
		
	}

	private void write(OutputStream outStream)
	{
	}
}


class GndTile
{
	public float cell1;
	public float cell2;
	public float cell3;
	public float cell4;
	public int tileUp;
	public int tileSide;
	public int tileOtherSide;
}