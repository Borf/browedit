package com.exnw.browedit.gui;

import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;

import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLEventListener;
import javax.media.opengl.glu.GLU;

import com.exnw.browedit.math.Vector2;

public class BrowRenderer implements GLEventListener, MouseMotionListener, MouseListener{
	static GLU glu = new GLU();
	float rotateT = 0;
	float dist = 200;
	float x = -1;
	float y = -1;
	MainFrame mainFrame;
	
	// Variables for mouse dragging
	private int oldx = 0;
	private int oldy = 0;
	private boolean dragged = false;
	
	public BrowRenderer(MainFrame mainFrame)
	{
		this.mainFrame = mainFrame;
	}
	
	public void display(GLAutoDrawable glDrawable)
	{
		if(x == -1)
		{
			x = mainFrame.currentMap.getGnd().getWidth()*5;
			y = mainFrame.currentMap.getGnd().getHeight()*5;
		}
		final GL gl = glDrawable.getGL();
		gl.glClear(GL.GL_COLOR_BUFFER_BIT);
		gl.glClear(GL.GL_DEPTH_BUFFER_BIT);
		float d = 100+(float) (Math.abs(Math.sin(dist))*1000.0);
		gl.glLoadIdentity();
		glu.gluLookAt(
						x+dist*Math.cos(rotateT+Math.PI/2.0), dist, y+dist*Math.sin(rotateT+Math.PI/2.0),
						x, 0,y,
						0,1,0);
		gl.glEnable(GL.GL_ALPHA_TEST);
		gl.glAlphaFunc(GL.GL_GREATER, 0.1f);
		mainFrame.currentMap.render(gl);
		
	}

	public void displayChanged(GLAutoDrawable gLDrawable, boolean modeChanged, boolean deviceChanged)
	{
	}

	public void init(GLAutoDrawable gLDrawable)
	{
		GL gl = gLDrawable.getGL();
		gl.glShadeModel(GL.GL_SMOOTH);
		gl.glClearColor(0.7f, 0.7f, 1.0f, 0.0f);
		gl.glClearDepth(1.0f);
		gl.glEnable(GL.GL_DEPTH_TEST);
		gl.glDepthFunc(GL.GL_LEQUAL);
		gl.glHint(GL.GL_PERSPECTIVE_CORRECTION_HINT, 
			  GL.GL_NICEST);
	}

	public void reshape(GLAutoDrawable gLDrawable, int x, int y, int width, int height)
	{
		GL gl = gLDrawable.getGL();
		if(height <= 0) {
		    height = 1;
		}
		float h = (float)width / (float)height;
		gl.glMatrixMode(GL.GL_PROJECTION);
		gl.glLoadIdentity();
		glu.gluPerspective(50.0f, h, 1.0, 10000.0);
		gl.glMatrixMode(GL.GL_MODELVIEW);
		gl.glLoadIdentity();		
	}

	public void mouseDragged( MouseEvent e ){		
		if( ( e.getModifiers() & MouseEvent.BUTTON1_MASK ) != 0 ){
			Vector2 v = new Vector2(oldx - e.getX(), oldy - e.getY());
			v.rotateBy(rotateT);
			x += v.getX();
			y += v.getY();
		}
		if( ( e.getModifiers() & MouseEvent.BUTTON3_MASK ) != 0 ){
			rotateT += (oldx - e.getX()) / 300.0f;
			dist += (oldy - e.getY());
			dist = Math.max(dist, 20);
		}
		oldx = e.getX();
		oldy = e.getY();
	}
	
	@Override
	public void mousePressed( MouseEvent e ){
		if( ( e.getModifiers() & MouseEvent.BUTTON1_MASK ) != 0 || ( e.getModifiers() & MouseEvent.BUTTON3_MASK ) != 0 ){
			if( !dragged ){
				dragged = true;
				oldx = e.getX();
				oldy = e.getY();
			}
		}
	}

	@Override
	public void mouseReleased( MouseEvent e ){
		if( ( e.getModifiers() & MouseEvent.BUTTON1_MASK ) != 0 || ( e.getModifiers() & MouseEvent.BUTTON3_MASK ) != 0){
			if( dragged ){
				dragged = false;
				oldx = e.getX();
				oldy = e.getY();
			}
		}		
	}

	@Override
	public void mouseMoved( MouseEvent e ){}

	@Override
	public void mouseClicked( MouseEvent e ){}

	@Override
	public void mouseEntered( MouseEvent e ){}

	@Override
	public void mouseExited( MouseEvent e ){}
}
