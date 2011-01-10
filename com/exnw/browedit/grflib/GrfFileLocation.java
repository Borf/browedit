package com.exnw.browedit.grflib;

public class GrfFileLocation extends FileLocation{
	private static byte[] magic = "Master of Magic".getBytes();
	private static byte[][] supportedVersions = new byte[][]{
		{ 2, 0 }
	};
	
	private String filename;
	
	private byte version_major;
	private byte version_minor;
	
	private com.exnw.browedit.io.SwappedRandomAccessFile grfFile;
	
	private java.util.Map<String, GrfFileDesc> files;
	
	public GrfFileLocation( String filename ) throws java.io.FileNotFoundException{
		if( filename == null || filename.isEmpty() )
			throw new IllegalArgumentException("No empty filename allowed.");
		
		filename = filename.trim();
		
		if( !filename.endsWith(".grf") )
			filename += ".grf";
		
		java.io.File f = new java.io.File( filename );
		
		if( !f.exists() || !f.isFile() )
			throw new java.io.FileNotFoundException();
		
		this.filename = filename;
		this.read();
	}

	public static boolean isSupported( byte major, byte minor ){
		for( byte[]version : GrfFileLocation.supportedVersions ){
			if( major == version[0] && minor == version[1] ){
				return true;
			}
		}
		return false;
	}

	public void read(){		
		try{
			this.grfFile = new com.exnw.browedit.io.SwappedRandomAccessFile( this.filename, "rw" );
			
			for( byte c : GrfFileLocation.magic ){
				if( c != this.grfFile.readByte() ){
					throw new IllegalArgumentException("GRF file header is corrupted.");
				}
			}
	
			this.grfFile.read( new byte[15] ); // Crypto
			
			int tableoffset = this.grfFile.readSwappedInt() + 0x2E;
			int fileCount = ( -this.grfFile.readSwappedInt() ) + this.grfFile.readSwappedInt() - 7;
			
			this.version_minor = this.grfFile.readByte();
			this.version_major = this.grfFile.readByte();
			
			if( !GrfFileLocation.isSupported( this.version_major, this.version_minor) )
				throw new IllegalArgumentException( String.format( "GRF Version %01d.%01d not supported.", this.version_major, this.version_minor ) );
			
			this.grfFile.seek( tableoffset );
			
			switch( version_major ){
				case 0x01:
					byte[] filelist = new byte[ (int)( this.grfFile.length() - tableoffset ) ];
					
					this.grfFile.read( filelist );
					
					this.files = new java.util.HashMap<String, GrfFileLocation.GrfFileDesc>();
					// TODO: do a borfi style implementation here xD
					break;
					
				case 0x02:
					byte[] compressed = new byte[ this.grfFile.readSwappedInt() ];
					byte[] result = new byte[ this.grfFile.readSwappedInt() ];
					
					this.grfFile.read( compressed );
					
					java.util.zip.Inflater decompresser = new java.util.zip.Inflater();
					decompresser.setInput( compressed );
					
					try{
						decompresser.inflate( result );
					}catch( java.util.zip.DataFormatException e ){
						e.printStackTrace();
					}
					
					decompresser.end();
					
					this.files = new java.util.HashMap<String, GrfFileLocation.GrfFileDesc>();
					for( int i = 0, index = 0; i < fileCount; i++ ){
						GrfFileDesc file = new GrfFileDesc();
						int p = 0;
						while(result[index+p] != 0)
							p++;			
						file.name = new String(result,index, p, "ISO-8859-1").toLowerCase();
						file.comprLen = 		((result[index+p+1]&0xFF)<<0) | ((result[index+p+2]&0xFF)<<8) | ((result[index+p+3]&0xFF)<<16) | ((result[index+p+4]&0xFF)<<24);
						file.comprLenAligned =	((result[index+p+5]&0xFF)<<0) | ((result[index+p+6]&0xFF)<<8) | ((result[index+p+7]&0xFF)<<16) | ((result[index+p+8]&0xFF)<<24);
						file.len = 				((result[index+p+9]&0xFF)<<0) | ((result[index+p+10]&0xFF)<<8) | ((result[index+p+11]&0xFF)<<16) | ((result[index+p+12]&0xFF)<<24);
						file.flags =			result[index+p+13];
						file.pos = 				46 + (((result[index+p+14]&0xFF)<<0) | ((result[index+p+15]&0xFF)<<8) | ((result[index+p+16]&0xFF)<<16) | ((result[index+p+17]&0xFF)<<24));
						
			            switch( ( int )file.flags)
			            {
				            case 1:
				            	file.cycle = -1;
				                break;
				            case 3:
				            	file.cycle = 1;
				                for( int j = 10; file.comprLen >= j; j *= 10, file.cycle++ )
				                    ;
				                break;
				            case 5:
				            	file.cycle = 0;
				                break;
			            }
						files.put(file.name, file);	
						index += p+18; 
					}
					
					// TODO: get this clean code working faster :(
					/*
					com.exnw.browedit.io.SwappedInputStream sis = null;
					
					try{
						sis = new com.exnw.browedit.io.SwappedInputStream( new java.io.ByteArrayInputStream( result ) );
						
						for( int i = 0; i < fileCount; i++ ){
							GrfFileLocation.GrfFileDesc file = new GrfFileLocation.GrfFileDesc();
							
							file.name = sis.readISOString();
							file.comprLen = sis.readInt();
							file.comprLenAligned = sis.readInt();
							file.len = sis.readInt();
							file.flags = sis.readByte();
							file.pos = 0x2E + sis.readInt(); // Headersize + Offset
							
				            switch( ( int )file.flags )
				            {
					            case 1:
					            	file.cycle = -1;
					                break;
					            case 3:
					            	file.cycle = 1;
					                for( int j = 10; file.comprLen >= j; j *= 10, file.cycle++ )
					                    ;
					                break;
					            case 5:
					            	file.cycle = 0;
					                break;
				            }
				            
							this.files.put( file.name.toLowerCase(), file );							
						}
					}catch( java.io.IOException e ){
						e.printStackTrace();
					}finally{
						if( sis != null ){
							try{
								sis.close();
							}catch( java.io.IOException e ){
								e.printStackTrace();
							}
						}
					}*/
			}
		}catch( java.io.IOException ex ){
			ex.printStackTrace();
		}	
	}

	public java.io.InputStream getStream( String fileName ){
		GrfFileLocation.GrfFileDesc file = this.files.get( fileName.toLowerCase() );
		
		if( file == null )
			return null;
		
		try{
			this.grfFile.seek( file.pos );
			byte[] compr = new byte[file.comprLenAligned];
			byte[] data = new byte[file.len];
			
			this.grfFile.read( compr );
			
			if( file.cycle >= 0 )
				DES.decodeGRF(compr, file.cycle );
			
			if( file.comprLen < file.len ){
				java.util.zip.Inflater decompresser = new java.util.zip.Inflater();
				decompresser.setInput(compr);
				try{
					decompresser.inflate(data);
				}catch( java.util.zip.DataFormatException e ){
					e.printStackTrace();
				}
				decompresser.end();	
			}
			
			return new java.io.ByteArrayInputStream( data );
		}catch( java.io.IOException e ){
			e.printStackTrace();
		}
		
		return null;
	}

	public boolean isFile( String file ){
		return this.files.containsKey( file.toLowerCase() );
	}
	
	public void listFiles( java.util.Set<String> files, String f ){
		for( String file : this.files.keySet() ){
			if( file.length() >= f.length() && file.startsWith( f ) ){
				if( file.indexOf( "\\", f.length() ) != -1 ){
					files.add( file.substring( 0, file.indexOf( "\\", f.length() ) + 1 ) );
				}else{
					files.add( file );
				}
			}
		}
	}	

	public class GrfFileDesc{
		private String name;
		private int comprLen;
		private int comprLenAligned;
		private int len;
		private byte flags;
		private int pos;
		private int cycle;
		
		public String toString(){
			return name + " (" + this.len + ")";
		}
	}
}
