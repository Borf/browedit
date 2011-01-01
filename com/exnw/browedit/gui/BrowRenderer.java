package com.exnw.browedit.gui;

import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLEventListener;
import javax.media.opengl.glu.GLU;

public class BrowRenderer implements GLEventListener
{
	static GLU glu = new GLU();
	float rotateT = 0;
	
	MainFrame mainFrame;
	
	public BrowRenderer(MainFrame mainFrame)
	{
		this.mainFrame = mainFrame;
	}
	
	
	public void display(GLAutoDrawable glDrawable)
	{
		final GL gl = glDrawable.getGL();
		gl.glClear(GL.GL_COLOR_BUFFER_BIT);
		gl.glClear(GL.GL_DEPTH_BUFFER_BIT);
		gl.glLoadIdentity();
		glu.gluLookAt(	0, 100, 0,	//eye 
						mainFrame.currentMap.getGat().getWidth()/2, 0, mainFrame.currentMap.getGat().getHeight()/2,	//lookat 
						0, 1, 0);	//up
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
		glu.gluPerspective(50.0f, h, 1.0, 1000.0);
		gl.glMatrixMode(GL.GL_MODELVIEW);
		gl.glLoadIdentity();		
	}

}
