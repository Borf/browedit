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
		return Float.intBitsToFloat(this.in.readInt());
	}

	public final int readInt() throws java.io.IOException{
		return SwappedInputStream.swap( this.in.readInt() );
	}
	
	private static int swap( int other ){
		return ( ( other & 0xFF ) << 24 ) | ( ( ( other >> 8 ) & 0xFF ) << 16 ) | ( ( ( other >> 16 ) & 0xFF ) << 8 ) | ( ( other >> 24 ) & 0xFF );
	}
}
