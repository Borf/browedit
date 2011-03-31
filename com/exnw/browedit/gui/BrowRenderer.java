package com.exnw.browedit.gui;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;

import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLEventListener;
import javax.media.opengl.glu.GLU;

import org.json.JSONException;

import com.exnw.browedit.math.Vector2;
import com.exnw.browedit.math.Vector3;

import com.exnw.browedit.config.Settings;
import com.exnw.browedit.grflib.GrfLib;
import com.exnw.browedit.gui.ToolToolBar;

public class BrowRenderer implements GLEventListener, MouseMotionListener, MouseListener, MouseWheelListener {
	static GLU glu = new GLU();
	float dist = 200;
	float x = -1;
	float y = -1;
	MainFrame mainFrame;
	boolean inversecamera;
	
	// Variables for mouse dragging
	private int oldx = 0;
	private int oldy = 0;
	private boolean dragged = false;
	private Vector3 viewpoint;
	
	public BrowRenderer(MainFrame mainFrame)
	{
		this.mainFrame = mainFrame;
		
		double x = Math.PI / 2.0;
		viewpoint = new Vector3(Math.cos(x),Math.cos(x/2),Math.sin(x));
		try
		{
			inversecamera = Settings.json.getBoolean("inversecamera");
		} catch (JSONException e)
		{
			System.err.println("No configuration for inversecamera in the settings.");
			e.printStackTrace();
			System.exit(1);
		}
		
	}
	
	public void display(GLAutoDrawable glDrawable)
	{
		if(x == -1)
		{
			x = mainFrame.getCurrentMap().getGnd().getWidth()*5;
			y = mainFrame.getCurrentMap().getGnd().getHeight()*5;
		}
		final GL gl = glDrawable.getGL();
		gl.glClear(GL.GL_COLOR_BUFFER_BIT);
		gl.glClear(GL.GL_DEPTH_BUFFER_BIT);
		float d = 100+(float) (Math.abs(Math.sin(dist))*1000.0);
		gl.glLoadIdentity();
		glu.gluLookAt(
						x+dist*viewpoint.getX(), dist*viewpoint.getY(), y+dist*viewpoint.getZ(),
						x, 0,y,
						0,1,0);
		gl.glEnable(GL.GL_ALPHA_TEST);
		gl.glAlphaFunc(GL.GL_GREATER, 0.1f);
		mainFrame.getCurrentMap().render(gl);
		
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
		if(mainFrame.mainPanel.toolToolBar.isBasicViewSelected()){
			if( ( e.getModifiers() & MouseEvent.BUTTON1_MASK ) != 0 ){
				Vector2 v = new Vector2((((oldx - e.getX())/300.0f)*dist), (((oldy - e.getY())/300.0f)*dist));
				float rot = (float) (viewpoint.getHorizontalAngle() - (Math.PI/2));
				v.rotateBy(rot);
				x += v.getX();
				y += v.getY();
			}
			
			if( ( e.getModifiers() & MouseEvent.BUTTON3_MASK ) != 0 ){
				if(!inversecamera){
						viewpoint.rotateBy((oldx-e.getX()) / 300.0f, (oldy-e.getY()) / 300.0f);
				} else {
						viewpoint.rotateBy((oldx-e.getX()) / 300.0f, (oldy-e.getY()) / 300.0f);
				}
			}
			oldx = e.getX();
			oldy = e.getY();
		}
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
	public void mouseWheelMoved(MouseWheelEvent e){
		if(mainFrame.mainPanel.toolToolBar.isBasicViewSelected()){
			int notches = e.getWheelRotation();
			dist += (notches*9);
			dist = Math.max(dist, 20);
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
