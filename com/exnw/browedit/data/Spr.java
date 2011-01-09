package com.exnw.browedit.data;

public class Spr{
	private static byte[] magic = "SP".getBytes();
	private static byte[][] supportedVersions = new byte[][]{
		{ 2, 0 },
		{ 2, 1 }
	};
	
	private String filename;
	
	// Version
	private byte version_major;
	private byte version_minor;
	
	private java.awt.Color[] palette;
	private java.util.List<java.awt.image.BufferedImage> frames;
	
	public Spr( String filename ){
		if( filename == null || filename.isEmpty() )
			throw new IllegalArgumentException("No empty filename allowed.");
		
		filename = filename.trim();
		
		if( !filename.endsWith(".spr") )
			filename += ".spr";
		
		this.filename = filename;
		this.read();
	}

	public static boolean isSupported( byte major, byte minor ){
		for( byte[]version : Spr.supportedVersions ){
			if( major == version[0] && minor == version[1] ){
				return true;
			}
		}
		return false;
	}
	
	public void read(){
		com.exnw.browedit.io.SwappedInputStream dis = null;
		
		try{
			dis = new com.exnw.browedit.io.SwappedInputStream( com.exnw.browedit.grflib.GrfLib.openFile( this.filename ) );
			
			for( byte c : Spr.magic ){
				if( c != dis.readByte() ){
					throw new IllegalArgumentException("SPR file header is corrupted.");
				}
			}
			
			this.version_minor = dis.readByte();
			this.version_major = dis.readByte();
			
			if( !Spr.isSupported( this.version_major, this.version_minor) )
				throw new IllegalArgumentException( String.format( "SPR Version %01d.%01d not supported.", this.version_major, this.version_minor ) );
			
			this.frames = new java.util.ArrayList<java.awt.image.BufferedImage>();
			byte[][] images = new byte[dis.readInt()][];
			for( int i = 0; i < images.length; i++ ){
				java.awt.image.BufferedImage img = new java.awt.image.BufferedImage( dis.readShort(), dis.readShort(), java.awt.image.BufferedImage.TYPE_4BYTE_ABGR );
				byte[] buffer = null;
				
				if( this.version_minor == 1 ){
					buffer = new byte[dis.readShort()];
					dis.readBytes( buffer );
					buffer = Spr.RLE.Decrypt( buffer, (byte)0x00 );
				}else{
					buffer = new byte[img.getWidth() * img.getHeight()];
					dis.readBytes( buffer );
				}
				
	            for( int j = 0, x = 0, y = 0; j < buffer.length; j++ ){
	                img.setRGB( x, y, this.palette[buffer[j]].getRGB() );
	                x = ++x % img.getWidth();
	                if( x == 0 )
	                    y++;
	                if( y == img.getHeight() )
	                    break;
	            }
	            
	            this.frames.add( img );
			}
			
			java.awt.Color[] palette = new java.awt.Color[256];
			for( int i = 0; i < 256; i++ ){
				palette[i] = new java.awt.Color( dis.readByte(), dis.readByte(), dis.readByte(), dis.readByte() );
			}
			
			palette[0] = new java.awt.Color( palette[0].getRed(), palette[0].getGreen(), palette[0].getBlue(), 0 );
			
			for( int i = 0; i < images.length; i++ ){
				java.awt.image.BufferedImage img = this.frames.get( i );
				byte[] buffer = images[i];
				
	            for( int j = 0, x = 0, y = 0; j < buffer.length; j++ ){
	                img.setRGB( x, y, this.palette[buffer[j]].getRGB() );
	                x = ++x % img.getWidth();
	                if( x == 0 )
	                    y++;
	                if( y == img.getHeight() )
	                    break;
	            }
	            
	            this.frames.add( img );
			}
		}catch( java.io.IOException ex ){
			ex.printStackTrace();
		}finally{
			if( dis != null ){
				try{
					dis.close();
				}catch( java.io.IOException ex ){
					ex.printStackTrace();
				}
			}
		}		
	}
	
	private static final class RLE{
	    private static final byte[] Decrypt( byte[] buffer, byte character ) {
	        java.util.List<Byte> data = new java.util.ArrayList<Byte>();
	
	        for( int i = 0; i < buffer.length; i++ ) {
	            if( buffer[i] == character && i < buffer.length - 1 ) {
	                for( int j = 0; j < buffer[i + 1]; j++ )
	                    data.add( character );
	                i++;
	            } else {
	                data.add( buffer[i] );
	            }
	        }
	        
	        byte[] buf = new byte[data.size()];
	        for( int i = 0; i < buf.length; i++ )
	        	buf[i] = data.get(i);
	
	        return buf;
	    }
	}
}
