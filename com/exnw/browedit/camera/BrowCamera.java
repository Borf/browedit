package com.exnw.browedit.camera;

import java.awt.event.MouseEvent;
import java.awt.event.MouseWheelEvent;

import com.exnw.browedit.math.Matrix4;
import com.exnw.browedit.math.Vector3;

public class BrowCamera extends Camera
{
	Vector3 viewPosition;
	Vector3 rotation;
	float distance;
	
	
	public BrowCamera(Vector3 viewPosition)
	{
		this.viewPosition = viewPosition;
		this.rotation = new Vector3(45,0,0);
		this.distance = 1000;
	}


	public Matrix4 getMatrix()
	{
		Matrix4 mat = new Matrix4();
		mat = mat.mult(Matrix4.makeTranslation(0, 0, -distance));
		mat = mat.mult(Matrix4.makeRotation(rotation.getX(), 1, 0, 0));
		mat = mat.mult(Matrix4.makeRotation(rotation.getY(), 0, 1, 0));
		mat = mat.mult(Matrix4.makeRotation(rotation.getZ(), 0, 0, 1));
		mat = mat.mult(Matrix4.makeTranslation(-viewPosition.getX(), -viewPosition.getY(), -viewPosition.getZ()));
		return mat;
	}
	
	
	public void useMouseDrag(MouseEvent lastEvent, MouseEvent event)
	{
		if((event.getModifiers() & MouseEvent.SHIFT_MASK) != 0)
		{
			rotation = rotation.add(new Vector3((event.getY() - lastEvent.getY()) / 5.0, (event.getX() - lastEvent.getX()) / 5.0, 0));
		}
		else if((event.getModifiers() & MouseEvent.CTRL_MASK) != 0)
		{
			viewPosition.getData()[1] += (event.getY() - lastEvent.getY()) / 2.0f;
		}
		else
		{
			float angle = rotation.getY() / 180.0f * (float)Math.PI;
			
			float x = -(event.getX() - lastEvent.getX()) / 2.0f;
			float y = -(event.getY() - lastEvent.getY()) / 2.0f;			
			viewPosition = viewPosition.add(new Vector3(x * (float)Math.cos(angle) + y * (float)Math.cos(angle+Math.PI/2), 0,  x * (float)Math.sin(angle) + y * (float)Math.sin(angle+Math.PI/2)));
		}
	}


	public void useMouseWheel(MouseWheelEvent event)
	{
		distance += event.getWheelRotation() * 9;
		if(distance < 0)
			distance = 0;
	}


	public void useMouseDoubleClick(MouseEvent e)
	{
		this.rotation = new Vector3(45,0,0);
	}
	

}
