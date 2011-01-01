package com.exnw.browedit.data;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.RandomAccess;

import com.exnw.browedit.grflib.GrfLib;

public class Rsw
{

	public Rsw(String fileName)
	{
		read(GrfLib.openFile(fileName));
	}

	private void read(InputStream instream)
	{
		
	}

	private void write(OutputStream outStream)
	{
	}
}
