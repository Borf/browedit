package com.exnw.browedit.io;

public class SwappedInputStream
{
	private java.io.DataInputStream in;

	public SwappedInputStream( java.io.InputStream input )
	{
		if( input == null )
			throw new IllegalArgumentException();
		
		this.in = new java.io.DataInputStream( input );
	}
	
	public final void close() throws java.io.IOException{
		this.in.close();
	}
	
	public final byte readByte() throws java.io.IOException{
		return this.in.readByte();
	}
	
	public final float readFloat() throws java.io.IOException{
		return Float.intBitsToFloat( this.readInt() );
	}

	public final int readInt() throws java.io.IOException{
		return SwappedInputStream.swap( this.in.readInt() );
	}
	
    public final String readISOString() throws java.io.IOException{
    	java.util.List<Byte> bytes = new java.util.ArrayList<Byte>();
    	byte b;
    	
    	while( (( b = this.readByte() )&0xFF) > 0 ){
    		bytes.add( b );
    	}
    	
    	byte[] output = new byte[bytes.size()];
    	
    	for( int i = 0; i < bytes.size(); i++ )
    		output[i] = bytes.get(i);
    	
    	return new String( output, "ISO-8859-1" ).trim();
    }
	
    public final short readShort() throws java.io.IOException{
        int ch1 = this.in.read();
        int ch2 = this.in.read();
        
        if( ( ch1 | ch2 ) < 0 )
            throw new java.io.EOFException();
        
        return (short)( ( ch2 << 8 ) + ch1 );
    }
	
	private static int swap( int other ){
		return ( ( other & 0xFF ) << 24 ) | ( ( ( other >> 8 ) & 0xFF ) << 16 ) | ( ( ( other >> 16 ) & 0xFF ) << 8 ) | ( ( other >> 24 ) & 0xFF );
	}
}
