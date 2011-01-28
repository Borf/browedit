package com.exnw.browedit.grflib;

public class GrfLib {
		private static java.util.Map<String,FileLocation> locations = new java.util.LinkedHashMap<String,FileLocation>();
		
		public static void addGrf( String file ) throws java.io.FileNotFoundException{
			GrfLib.locations.put( file, new GrfFileLocation( file ) );
		}
		
		public static void addGrfs( String file ) throws java.io.FileNotFoundException, java.io.IOException{
			java.io.BufferedReader br = null;
			
			try{
				br = new java.io.BufferedReader( new java.io.FileReader( file ) );
				
				String line = null;
				java.util.Map<Integer,String> grfs = new java.util.TreeMap<Integer, String>();
				
				if( ( line = br.readLine() ) == null || line.trim().isEmpty() || !line.trim().equalsIgnoreCase("[DATA]") )
					throw new IllegalArgumentException();
				
				while( ( line = br.readLine() ) != null ){
					String[] pts = line.trim().split( "=", 2 );
					
					try{
						grfs.put( Integer.parseInt( pts[0].trim() ), pts[1].trim() );
					}catch( NumberFormatException e ){
						continue;
					}
				}
				
				String path = file.substring( 0, file.lastIndexOf( java.io.File.separatorChar ) + 1 );
				
				for( String grf : grfs.values() ){
					try{
						GrfLib.addGrf( path + grf );
					}catch( java.io.FileNotFoundException e ){
						System.err.println("Could not find GRF File \"" + ( path + grf ) + "\".");
					}
				}
			}finally{
				if( br != null ){
					br.close();
				}
			}
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
						System.err.println("---------------\nCould not open " + file + "\n--------------\n");
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
