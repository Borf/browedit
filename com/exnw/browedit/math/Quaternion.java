package com.exnw.browedit.math;

import java.io.IOException;
import java.io.Serializable;

import com.exnw.browedit.io.SwappedInputStream;

public class Quaternion implements Serializable
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
	
	public Quaternion(Quaternion q1, Quaternion q2, float interp)
	{
		this.data = new float[4];
		this.slerp( q1, q2, interp );
	}	

	private void slerp(Quaternion q1, Quaternion q2, float interp)
	{
		int i;
		float a = 0, b = 0;
		for ( i = 0; i < 4; i++ )
		{
			a += ( q1.data[i]-q2.data[i] )*( q1.data[i]-q2.data[i] );
			b += ( q1.data[i]+q2.data[i] )*( q1.data[i]+q2.data[i] );
		}
		if ( a > b )
			q2.inverse();
		
		float cosom = q1.data[0]*q2.data[0]+q1.data[1]*q2.data[1]+q1.data[2]*q2.data[2]+q1.data[3]*q2.data[3];
		double sclq1, sclq2;
		
		if (( 1.0+cosom ) > 0.00000001 )
		{
			if (( 1.0-cosom ) > 0.00000001 )
			{
				double omega = Math.acos( cosom );
				double sinom = Math.sin( omega );
				sclq1 = Math.sin(( 1.0-interp )*omega )/sinom;
				sclq2 = Math.sin( interp*omega )/sinom;
			}
			else
			{
				sclq1 = 1.0-interp;
				sclq2 = interp;
			}
			for ( i = 0; i < 4; i++ )
				data[i] = ( float )( sclq1*q1.data[i]+sclq2*q2.data[i] );
		}
		else
		{
			data[0] = -q1.data[1];
			data[1] = q1.data[0];
			data[2] = -q1.data[3];
			data[3] = q1.data[2];
			
			sclq1 = Math.sin(( 1.0-interp )*0.5*Math.PI );
			sclq2 = Math.sin( interp*0.5*Math.PI );
			for ( i = 0; i < 3; i++ )
				data[i] = ( float )( sclq1*q1.data[i]+sclq2*data[i] );
		}
	}

	private void inverse()
	{
		data[0] = -data[0];
		data[1] = -data[1];
		data[2] = -data[2];
		data[3] = -data[3];
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
