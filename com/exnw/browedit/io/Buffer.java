package com.exnw.browedit.io;

import com.exnw.browedit.math.Matrix4;
import com.exnw.browedit.math.Quaternion;
import com.exnw.browedit.math.Vector2;
import com.exnw.browedit.math.Vector3;

public class Buffer{
	private int offset;
	private byte[] buffer;
	
	public Buffer( byte[] buffer ){
		this.buffer = buffer;
		this.offset = 0x00;
	}
	
	public byte getByte(){
		return this.buffer[this.offset++];
	}
	
	public short getShort(){
		this.offset += 2;
		return (short)( ( ( this.buffer[this.offset-1] << 8 ) & 0xFF00 ) | ( this.buffer[this.offset-2] & 0x00FF ) );
	}
	
	public int getInt(){
		this.offset += 4;
		
		return ( ( ( this.buffer[this.offset-1] << 24 ) & 0xFF000000 ) | ( ( this.buffer[this.offset-2] << 16 ) & 0x00FF0000 ) | ( ( this.buffer[this.offset-3] << 8 ) & 0x0000FF00 ) | ( ( this.buffer[this.offset-4] ) & 0x000000FF ) );
	}
	
	public float getFloat(){
		return Float.intBitsToFloat( this.getInt() );
	}
	
	public String getString( int length ){
		String s = new String( this.buffer, this.offset, length );
		int index = s.indexOf('\0');
		
		if( index != -1 ){
			s = s.substring( 0, index );
		}
		
		this.offset += length;
		
		return s;
	}
	
	public Quaternion getQuaternion(){
		return new Quaternion( new float[]{ this.getFloat(), this.getFloat(), this.getFloat(), this.getFloat() } );
	}
	
	public Matrix4 getMatrix4(){
		return new Matrix4(
					this.getFloat(), this.getFloat(), this.getFloat(), 0.0f,
					this.getFloat(), this.getFloat(), this.getFloat(), 0.0f,
					this.getFloat(), this.getFloat(), this.getFloat(), 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				);
	}
	
	public Vector2 getVector2(){
		return new Vector2( this.getFloat(), this.getFloat() );
	}
	
	public Vector3 getVector3(){
		return new Vector3( this.getFloat(), this.getFloat(), this.getFloat() );
	}
	
	public void skip( int length ){
		this.offset += length;
	}
}
