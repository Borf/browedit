package com.exnw.browedit.math;

import java.io.IOException;

import com.exnw.browedit.io.SwappedInputStream;

public class Quaternion
{
	private float data[];
	
	public Quaternion(float[] data)
	{
		this.data = data;
	}

	public Quaternion(SwappedInputStream in) throws IOException
	{
		this.data = new float[4];
		for(int i = 0; i < 4; i++)
			this.data[i] = in.readFloat();
	}
	
	public Quaternion getNormalized()
	{
		float len = getLength();
		return new Quaternion(new float[]{data[0]/len, data[1]/len, data[2]/len, data[3]/len});
	}
	float getLength()
	{
		return (float)Math.sqrt(data[0]*data[0]+data[1]*data[1]+data[2]*data[2]+data[3]*data[3]);
	}
	
	public Matrix4 getRotationMatrix()
	{
		Matrix4 ret = new Matrix4();
		ret.getData()[0] = ( float )( 1.0 - 2.0*data[1]*data[1] - 2.0*data[2]*data[2] );
		ret.getData()[1] = ( float )( 2.0*data[0]*data[1] + 2.0*data[3]*data[2] );
		ret.getData()[2] = ( float )( 2.0*data[0]*data[2] - 2.0*data[3]*data[1] );
		
		ret.getData()[4] = ( float )( 2.0*data[0]*data[1] - 2.0*data[3]*data[2] );
		ret.getData()[5] = ( float )( 1.0 - 2.0*data[0]*data[0] - 2.0*data[2]*data[2] );
		ret.getData()[6] = ( float )( 2.0*data[1]*data[2] + 2.0*data[3]*data[0] );
		
		ret.getData()[8] = ( float )( 2.0*data[0]*data[2] + 2.0*data[3]*data[1] );
		ret.getData()[9] = ( float )( 2.0*data[1]*data[2] - 2.0*data[3]*data[0] );
		ret.getData()[10] = ( float )( 1.0 - 2.0*data[0]*data[0] - 2.0*data[1]*data[1] );			

		return ret;
	}
	
}
