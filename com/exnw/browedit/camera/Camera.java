package com.exnw.browedit.camera;

import java.awt.event.MouseEvent;
import java.awt.event.MouseWheelEvent;

import com.exnw.browedit.math.Matrix4;

public abstract class Camera
{
	public abstract Matrix4 getMatrix();
	public abstract void useMouseDrag(MouseEvent lastEvent, MouseEvent event);
	public abstract void useMouseWheel(MouseWheelEvent event);
}
