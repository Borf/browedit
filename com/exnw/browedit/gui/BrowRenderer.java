package com.exnw.browedit.gui;

import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;

import javax.media.opengl.GL;
import javax.media.opengl.GL2;
import javax.media.opengl.GL4;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLEventListener;
import javax.media.opengl.glu.GLU;

import com.exnw.browedit.camera.BrowCamera;
import com.exnw.browedit.camera.Camera;
import com.exnw.browedit.math.Matrix4;
import com.exnw.browedit.math.Vector3;
import com.exnw.browedit.renderutils.Shader;

public class BrowRenderer implements GLEventListener, MouseMotionListener, MouseListener, MouseWheelListener {
	static GLU glu = new GLU();
	float dist = 200;
	MainFrame mainFrame;
	
	// Variables for mouse dragging
	private Camera camera;
	private MouseEvent lastMouseEvent;
	private Shader shader;
	

	
	public BrowRenderer(MainFrame mainFrame)
	{
		this.mainFrame = mainFrame;
		camera = new BrowCamera(new Vector3(500,50,500));		
	}
	
	public void display(GLAutoDrawable glDrawable)
	{
		final GL4 gl = glDrawable.getGL().getGL4();
		gl.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT);

		if(shader == null)
			shader = new Shader("shader.vert", "shader.frag", gl);

		shader.use();
		shader.getUniform("projectionMatrix").set(Matrix4.makePerspective(90, 1, 1, 1000));
		shader.getUniform("viewMatrix").set(camera.getMatrix());
		shader.getUniform("modelMatrix").set(new Matrix4());
		mainFrame.getCurrentMap().render(mainFrame, gl, shader);
		
	}

	public void displayChanged(GLAutoDrawable gLDrawable, boolean modeChanged, boolean deviceChanged)
	{
	}

	public void init(GLAutoDrawable gLDrawable)
	{
		GL2 gl = gLDrawable.getGL().getGL2();
		gl.glShadeModel(GL2.GL_SMOOTH);
		gl.glClearColor(0.7f, 0.7f, 1.0f, 0.0f);
		gl.glClearDepth(1.0f);
		gl.glEnable(GL.GL_DEPTH_TEST);
		gl.glDepthFunc(GL.GL_LEQUAL);
		gl.glHint(GL2.GL_PERSPECTIVE_CORRECTION_HINT, GL.GL_NICEST);
	}

	public void reshape(GLAutoDrawable gLDrawable, int x, int y, int width, int height)
	{
		if(height <= 0) {
		    height = 1;
		}
		float h = (float)width / (float)height;
		if(shader != null)
			shader.getUniform("projectionMatrix").set(Matrix4.makePerspective(90, h, 1, 1000));
	}

	public void mouseDragged( MouseEvent e ){
		mainFrame.mouseX = e.getX();
		mainFrame.mouseY = e.getY();		

		if( ( e.getModifiers() & MouseEvent.BUTTON2_MASK ) != 0 )
		{
			camera.useMouseDrag(lastMouseEvent, e);
		}
		lastMouseEvent = e;
	}
	
	@Override
	public void mousePressed( MouseEvent e ){
		if( ( e.getModifiers() & MouseEvent.BUTTON2_MASK ) != 0 )
		{
			if( lastMouseEvent == null)
			{
				lastMouseEvent = e;
			}
		}
	}

	@Override
	public void mouseReleased( MouseEvent e ){
		if( ( e.getModifiers() & MouseEvent.BUTTON2_MASK ) != 0){
			if( lastMouseEvent != null ){
				lastMouseEvent = null;
			}
		}		
	}
	
	@Override
	public void mouseWheelMoved(MouseWheelEvent e){
		camera.useMouseWheel(e);
	}

	@Override
	public void mouseMoved( MouseEvent e )
	{
		mainFrame.mouseX = e.getX();
		mainFrame.mouseY = e.getY();		
	}

	@Override
	public void mouseClicked( MouseEvent e ){}

	@Override
	public void mouseEntered( MouseEvent e ){}

	@Override
	public void mouseExited( MouseEvent e ){}

	@Override
	public void dispose(GLAutoDrawable arg0)
	{
	}

}
