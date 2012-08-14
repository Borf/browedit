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

import com.exnw.browedit.camera.BrowCamera;
import com.exnw.browedit.camera.Camera;
import com.exnw.browedit.edittools.EditTool;
import com.exnw.browedit.math.Matrix3;
import com.exnw.browedit.math.Matrix4;
import com.exnw.browedit.math.Vector3;
import com.exnw.browedit.renderutils.Shader;

public class BrowRenderer implements GLEventListener, MouseMotionListener, MouseListener, MouseWheelListener {
	MainFrame mainFrame;
	
	// Variables for mouse dragging
	private Camera camera;
	private MouseEvent lastMouseEvent;
	private Shader shader;
	Vector3 mouse3d;

	public boolean showObjects = true;
	public boolean showShadow = true;
	public boolean showGrid = false;
	public boolean showGrid2 = false;
	
	
	public EditTool currentTool;

	
	
	public BrowRenderer(MainFrame mainFrame)
	{
		this.mainFrame = mainFrame;
		camera = new BrowCamera(new Vector3(800,50,800));		
	}
	
	public void display(GLAutoDrawable glDrawable)
	{
		final GL4 gl = glDrawable.getGL().getGL4();
		gl.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT);

		if(shader == null)
			shader = new Shader("shader.vert", "shader.frag", gl);
		shader.use(gl);
		
		
		try
		{
		shader.getUniform("viewMatrix", gl).set(camera.getMatrix(), gl);
		shader.getUniform("modelMatrix", gl).set(new Matrix4(), gl);
		shader.getUniform("lighting", gl).set(true, gl);
		float h = (float)glDrawable.getWidth()  / (float)glDrawable.getHeight();
		if(shader != null)
		{
			if(shader.getUniform("projectionMatrix", gl) != null)
				shader.getUniform("projectionMatrix", gl).set(Matrix4.makePerspective(90, h, 1, 5000), gl);
		}
		
		Matrix4 modelViewMatrix = new Matrix4();//camera.getMatrix();
		
		Matrix3 normalMatrix = new Matrix3(
					modelViewMatrix.getValue(0,0), modelViewMatrix.getValue(0,1), modelViewMatrix.getValue(0,2),
					modelViewMatrix.getValue(1,0), modelViewMatrix.getValue(1,1), modelViewMatrix.getValue(1,2),
					modelViewMatrix.getValue(2,0), modelViewMatrix.getValue(2,1), modelViewMatrix.getValue(2,2));
		normalMatrix = normalMatrix.invert().transpose();		
		shader.getUniform("normalMatrix", gl).set(normalMatrix, gl);
		
		shader.getUniform("s_texture", gl).set(0, gl);		
		shader.getUniform("s_textureShadow", gl).set(1, gl);		
		shader.getUniform("s_textureLight", gl).set(2, gl);		
		}
		catch(NullPointerException e){	}
				
		
		if(mainFrame.getMainPanel().mapRenderer != null)
			mainFrame.getMainPanel().mapRenderer.render(gl, shader);
	}

	public void displayChanged(GLAutoDrawable gLDrawable, boolean modeChanged, boolean deviceChanged)
	{
		System.err.println("Client: DISPLAY CHANGED");
	}

	public void init(GLAutoDrawable gLDrawable)
	{
		GL4 gl = gLDrawable.getGL().getGL4();
//		gl.glShadeModel(GL2.GL_SMOOTH);
//		gl.glHint(GL2.GL_PERSPECTIVE_CORRECTION_HINT, GL.GL_NICEST);
		gl.glClearColor(0.7f, 0.7f, 1.0f, 0.0f);
		gl.glClearDepth(1.0f);
		gl.glEnable(GL.GL_DEPTH_TEST);
		gl.glDepthFunc(GL.GL_LEQUAL);
	}

	public void reshape(GLAutoDrawable gLDrawable, int x, int y, int width, int height)
	{
		GL4 gl = gLDrawable.getGL().getGL4();
		gl.glViewport(0, 0, width, height);
		if(height <= 0) {
		    height = 1;
		}
		float h = (float)width / (float)height;
		if(shader != null)
		{
			if(shader.getUniform("projectionMatrix",gl) != null)
				shader.getUniform("projectionMatrix",gl).set(Matrix4.makePerspective(90, h, 1, 5000),gl);
		}
	}

	public void mouseDragged( MouseEvent e ){
		mainFrame.getMainPanel().mouseX = e.getX();
		mainFrame.getMainPanel().mouseY = e.getY();		

		if( ( e.getModifiers() & MouseEvent.BUTTON2_MASK ) != 0 )
		{
			camera.useMouseDrag(lastMouseEvent, e);
		}
		lastMouseEvent = e;
	}
	
	@Override
	public void mousePressed( MouseEvent e ){
		if( lastMouseEvent == null)
			lastMouseEvent = e;
		if( ( e.getModifiers() & MouseEvent.BUTTON2_MASK ) != 0 )
		{
		}
	}

	@Override
	public void mouseReleased( MouseEvent e ){
		if( lastMouseEvent != null )
			lastMouseEvent = null;
		if( ( e.getModifiers() & MouseEvent.BUTTON2_MASK ) != 0)
		{
		}
	}
	
	@Override
	public void mouseWheelMoved(MouseWheelEvent e)
	{
		camera.useMouseWheel(e);
	}

	@Override
	public void mouseMoved( MouseEvent e )
	{
		mainFrame.getMainPanel().mouseX = e.getX();
		mainFrame.getMainPanel().mouseY = e.getY();		
	}

	@Override
	public void mouseClicked( MouseEvent e )
	{
		
	}

	@Override
	public void mouseEntered( MouseEvent e )
	{
		
	}

	@Override
	public void mouseExited( MouseEvent e )
	{
		
	}

	@Override
	public void dispose(GLAutoDrawable arg0)
	{
	}

}
