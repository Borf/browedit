package com.exnw.browedit.grflib;

public class RealFileLocation extends FileLocation{
	private String path;
	
	public RealFileLocation( String path ) throws java.io.FileNotFoundException{
		if( path == null ){
			throw new NullPointerException();
		}
		
		java.io.File file = new java.io.File( path );
		
		if( !file.isDirectory() ){
			throw new java.io.FileNotFoundException();
		}
		
		if( !path.endsWith( java.io.File.separator ) ){
			this.path = path + java.io.File.separator;
		}else{
			this.path = path;
		}
	}
	
	public java.io.InputStream getStream( String file ) throws java.io.FileNotFoundException{		
		return new java.io.FileInputStream( this.path + RealFileLocation.toSystemFileName( file ) );
	}

	public boolean isFile( String file ){
		if( file == null )
			return false;
		
		java.io.File f = new java.io.File( this.path + RealFileLocation.toSystemFileName( file ) );
		
		return f.exists() && f.isFile();
	}
	
	public static String toSystemFileName( String file ){
		return file.replace( '/' , java.io.File.separatorChar );
	}
	
	public void listFiles( java.util.Set<String> files, String file ){
		java.io.File f = new java.io.File( this.path + RealFileLocation.toSystemFileName( file ) );
		
		if( f.isDirectory() ){
			java.util.Collections.addAll( files, f.list() );
		}
	}
}
