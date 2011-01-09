package com.exnw.browedit.grflib;

public class GrfLib {
		private static java.util.Map<String,FileLocation> locations = new java.util.LinkedHashMap<String,FileLocation>();
		
		public static void addGrf( String file ) throws java.io.FileNotFoundException{
			GrfLib.locations.put( file, new GrfFileLocation( file ) );
		}
		
		public static void addDir( String path ) throws java.io.FileNotFoundException{
			GrfLib.locations.put( path, new RealFileLocation( path ) );
		}
		
		public static void enableJar(){
			GrfLib.locations.put( "", new JarFileLocation() );
		}
		
		public static java.io.InputStream openFile( String file ){
			for( FileLocation l: GrfLib.locations.values() ){
				if( l.isFile( file ) ){
					try{
						return l.getStream( file );
					}catch( java.io.FileNotFoundException e ){
						e.printStackTrace();
					}
				}
			}
			
			return null;
		}
		
		public static java.util.Set<String> listFiles( String f ){
			java.util.Set<String> files = new java.util.HashSet<String>();
			
			for( FileLocation l: GrfLib.locations.values() ){
				l.listFiles( files, f );
			}
			
			return files;
		}
		
}
