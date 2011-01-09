package com.exnw.browedit.grflib;

public abstract class FileLocation{
	public abstract java.io.InputStream getStream( String file ) throws java.io.FileNotFoundException;
	public abstract boolean isFile( String file );
	public abstract void listFiles( java.util.Set<String> files, String f );
}
