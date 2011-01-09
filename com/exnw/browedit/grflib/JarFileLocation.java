package com.exnw.browedit.grflib;

public class JarFileLocation extends FileLocation{
	private ClassLoader loader;
	
	public JarFileLocation(){
		this.loader = this.getClass().getClassLoader();
	}

	@Override
	public java.io.InputStream getStream( String file ) throws java.io.FileNotFoundException{
		return this.loader.getResourceAsStream( file );
	}

	@Override
	public boolean isFile( String file ){
		return this.loader.getResource( file ) != null;
	}

	@Override
	public void listFiles( java.util.Set<String> files, String file ){}
}
