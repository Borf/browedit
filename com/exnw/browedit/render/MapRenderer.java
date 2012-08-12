package com.exnw.browedit.render;

import java.nio.FloatBuffer;
import java.util.Observable;

import javax.media.opengl.GL;
import javax.media.opengl.GL4;
import javax.media.opengl.glu.GLU;

import com.exnw.browedit.data.Map;
import com.exnw.browedit.data.RsmCache;
import com.exnw.browedit.gui.MainPanel;
import com.exnw.browedit.math.Matrix4;
import com.exnw.browedit.math.Vector3;
import com.exnw.browedit.renderutils.Shader;

public class MapRenderer implements Renderer
{
	private GndRenderer gndRenderer;
	private RswRenderer rswRenderer;
	private MainPanel mainPanel;
	
	

	public MapRenderer(Map map, MainPanel mainPanel)
	{
		this.mainPanel = mainPanel;
		
		gndRenderer = new GndRenderer(map.getGnd());
		rswRenderer = new RswRenderer(map.getRsw(), map);
	}

	@Override
	public void update(Observable arg0, Object arg1)
	{
	}

	GLU glu = new GLU();
	@Override
	public void render(GL4 gl, Shader shader)
	{
		shader.getUniform("showFloorShadow", gl).set(mainPanel.renderer.showShadow, gl);
		
		gndRenderer.render(gl,  shader);
		
		
		Matrix4 viewMat = shader.getUniform("viewMatrix", gl).valueMatrix4;
		Matrix4 modelMat = shader.getUniform("modelMatrix", gl).valueMatrix4;
		Matrix4 projMat = shader.getUniform("projectionMatrix", gl).valueMatrix4;
		
//		Matrix4 finalMat = viewMat.mult(modelMat.mult(projMat)).invert();
		
		
		int viewport[] = new int[4];
	    float mvmatrix[] = new float [16];
	    float projmatrix[] = new float [16];
	    float  wcoord[] = new float [4];
	    float winz[] = new float[2];
	    
		gl.glGetIntegerv(GL4.GL_VIEWPORT, viewport, 0);
		mvmatrix = modelMat.mult(viewMat).getData();
		projmatrix = projMat.getData();

		gl.glReadPixels( mainPanel.mouseX, mainPanel.getRenderHeight()-mainPanel.mouseY, 1, 1, GL4.GL_DEPTH_COMPONENT, GL.GL_FLOAT, FloatBuffer.wrap(winz));
		
		if(winz[0] != 1.0)
		{		
	        glu.gluUnProject((float) mainPanel.mouseX, (float) (mainPanel.getRenderHeight()-mainPanel.mouseY), winz[0],
	                mvmatrix, 0,
	                projmatrix, 0, 
	                viewport, 0, 
	                wcoord, 0);	
        //return new Vector3(wcoord[0], wcoord[1], wcoord[2]);
	        mainPanel.setCoordinats((int)wcoord[0], (int)wcoord[1], (int)wcoord[2]);
		}
		
		
		
		
		if(mainPanel.renderer.showObjects)
			rswRenderer.render(gl, shader);		
	}

	public void destroy(GL4 gl)
	{
		if(gndRenderer != null)
			gndRenderer.destroy(gl);
		if(rswRenderer != null)
			rswRenderer.destroy(gl);
		RsmCache.destroy(gl);
	}

}
