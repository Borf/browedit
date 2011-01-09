package com.exnw.browedit.io;

public class SwappedRandomAccessFile extends java.io.RandomAccessFile{
	public SwappedRandomAccessFile( java.io.File file, String mode ) throws java.io.FileNotFoundException{
		super( file, mode );
	}
	
	public SwappedRandomAccessFile( String name, String mode ) throws java.io.FileNotFoundException{
		super( name, mode );
	}
	
	public final float readSwappedFloat() throws java.io.IOException{
		return Float.intBitsToFloat( this.readSwappedInt() );
	}

	public final int readSwappedInt() throws java.io.IOException{
		return SwappedRandomAccessFile.swap( this.readInt() );
	}
	
    public final String readISOString() throws java.io.IOException{
    	java.util.List<Byte> bytes = new java.util.ArrayList<Byte>();
    	byte b;
    	
    	while( ( ( b = this.readByte() ) & 0xFF ) > 0 ){
    		bytes.add( b );
    	}
    	
    	byte[] output = new byte[bytes.size()];
    	
    	for( int i = 0; i < bytes.size(); i++ )
    		output[i] = bytes.get(i);
    	
    	return new String( output, "ISO-8859-1" ).trim();
    }
    
    public final String readISOString( int len ) throws java.io.IOException{
    	byte[] output = new byte[len];
    	this.read( output );
    	
    	int nullByte = 0;
    	while( output[nullByte] != 0 && nullByte < len )
    		nullByte++;

    	return new String( output, 0, nullByte, "ISO-8859-1" ).trim();
    }
	
    public final short readSwappedShort() throws java.io.IOException{
        int ch1 = this.read();
        int ch2 = this.read();
        
        if( ( ch1 | ch2 ) < 0 )
            throw new java.io.EOFException();
        
        return (short)( ( ch2 << 8 ) + ch1 );
    }
	
	private static int swap( int other ){
		return ( ( other & 0xFF ) << 24 ) | ( ( ( other >> 8 ) & 0xFF ) << 16 ) | ( ( ( other >> 16 ) & 0xFF ) << 8 ) | ( ( other >> 24 ) & 0xFF );
	}
}
