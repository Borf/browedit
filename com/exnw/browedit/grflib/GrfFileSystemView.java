package com.exnw.browedit.grflib;

public class GrfFileSystemView extends javax.swing.filechooser.FileSystemView{
	public java.io.File createFileObject( java.io.File dir, String filename ){
		return new java.io.File( filename );
	}

	public java.io.File createFileObject( String path ){
		return new java.io.File( path );
	}

	protected java.io.File createFileSystemRoot( java.io.File f ){
		return new java.io.File("");
	}

	public java.io.File getChild( java.io.File parent, String fileName ){
		return super.getChild( parent, fileName );
	}

	@Override
	public String getSystemDisplayName( java.io.File f ){
		// TODO Auto-generated method stub
		return f.getPath();
	}

	public String getSystemTypeDescription( java.io.File f ){
		return "This is a file";
	}

	public boolean isComputerNode( java.io.File dir ){
		// TODO Auto-generated method stub
		return super.isComputerNode( dir );
	}

	@Override
	public boolean isDrive( java.io.File dir ){
		// TODO Auto-generated method stub
		return super.isDrive(dir);
	}

	@Override
	public boolean isFileSystem( java.io.File f ){
		// TODO Auto-generated method stub
		return super.isFileSystem(f);
	}

	@Override
	public boolean isFileSystemRoot( java.io.File dir ){
		// TODO Auto-generated method stub
		return super.isFileSystemRoot(dir);
	}

	@Override
	public boolean isParent( java.io.File folder, java.io.File file ){
		// TODO Auto-generated method stub
		return super.isParent(folder, file);
	}

	@Override
	public boolean isRoot( java.io.File f ){
		// TODO Auto-generated method stub
		return super.isRoot(f);
	}

	@Override
	public Boolean isTraversable( java.io.File f ){
		if( f != null && ( f.getPath().equals("") || f.getPath().endsWith("\\") ) )
			return true;
		else
			return false;
	}

	public java.io.File getDefaultDirectory(){
		return new java.io.File("");
	}

	public java.io.File[] getFiles( java.io.File dir, boolean useFileHiding ){
		java.util.Set<String> files = GrfLib.listFiles("data\\");
		java.util.List<java.io.File> filelist = new java.util.ArrayList<java.io.File>(); 
		
		for( String file : files ){
			if( file.endsWith( ".rsw" ) ){
				filelist.add( new java.io.File( file ) );
			}
		}
		
		return filelist.toArray( new java.io.File[0] );
	}

	public java.io.File getHomeDirectory(){
		return new java.io.File("");
	}

	public java.io.File getParentDirectory( java.io.File dir ){
		return new java.io.File("");
	}

	@Override
	public java.io.File[] getRoots(){
		return new java.io.File[]{ new java.io.File("") };
	}

	public java.io.File createNewFolder( java.io.File file ) throws java.io.IOException{
		return null;
	}
}