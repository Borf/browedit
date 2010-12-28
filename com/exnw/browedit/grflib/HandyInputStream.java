package com.exnw.browedit.grflib;

import java.io.IOException;
import java.io.InputStream;

public class HandyInputStream extends InputStream
{
	InputStream stream;
	public HandyInputStream(InputStream stream)
	{
		this.stream = stream;
	}

	public int read() throws IOException
	{
		return stream.read();
	}

	public int readInt() throws IOException
	{
		byte[] d = new byte[4];
		stream.read(d);
		return ((d[0]&0xFF)<<0) | ((d[1]&0xFF)<<8) | ((d[2]&0xFF)<<16) | ((d[3]&0xFF)<<24);
	}


}
