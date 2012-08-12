package com.exnw.browedit.data;

import java.io.Serializable;

import javax.media.opengl.GL;
import javax.media.opengl.GL4;

import com.exnw.browedit.gui.MainFrame;
import com.exnw.browedit.math.Vector3;
import com.exnw.browedit.render.GatRenderer;
import com.exnw.browedit.render.GndRenderer;
import com.exnw.browedit.render.RswRenderer;
import com.exnw.browedit.render.TextureCache;
import com.exnw.browedit.renderutils.Shader;

public class Map implements Serializable
{
	private Gat gat;
	private Gnd gnd;
	private Rsw rsw;
	
	public Rsw getRsw()
	{
		return rsw;
	}
	
/*	private GatRenderer gatRenderer;
	private GndRenderer gndRenderer;
	private RswRenderer rswRenderer;*/
	
	public Map(String fileName)
	{
		if(fileName.toLowerCase().endsWith(".rsw"))
			fileName = fileName.substring(0, fileName.lastIndexOf("."));
		
		setRsw(new Rsw(fileName + ".rsw"));
		setGnd(new Gnd(fileName + ".gnd"));
		setGat(new Gat(fileName + ".gat"));
	}
	
/*	public void render(MainFrame mainFrame, GL4 gl, Shader shader)
	{
		gatRenderer.render(gl);
		
		Vector3 mouse3D = get3DCursor(mainFrame, gl); 
       
        int tx = (int) (mouse3D.getX()/5);
        int ty = (int) (mouse3D.getZ()/5);
        if(tx >= 0 && tx < gat.getWidth() && ty >= 0 && ty < gat.getHeight())
        {
        	for(int i = 0; i < 4; i++)
        		gat.setCellHeight(tx, gat.getHeight()-1-ty, i, gat.getCell(tx, gat.getHeight()-1-ty).getHeight()[i]-0.1f);
        }

		gndRenderer.render(gl, shader);
		if(mainFrame.getMainPanel().renderer.showObjects)
			rswRenderer.render(gl, shader);
	}*/
	
	
	private Vector3 get3DCursor(MainFrame mainFrame, GL gl)
	{
/*		int viewport[] = new int[4];
	    double mvmatrix[] = new double[16];
	    double projmatrix[] = new double[16];
	    double wcoord[] = new double[4];
	    float winz[] = new float[2];
	    
		gl.glGetIntegerv(GL.GL_VIEWPORT, viewport, 0);
		gl.glGetDoublev(GL.GL_MODELVIEW_MATRIX, mvmatrix, 0);
		gl.glGetDoublev(GL.GL_PROJECTION_MATRIX, projmatrix, 0);

		gl.glReadPixels( mainFrame.mouseX, mainFrame.getMainPanel().getCanvas().getHeight()-mainFrame.mouseY, 1, 1, GL.GL_DEPTH_COMPONENT, GL.GL_FLOAT, FloatBuffer.wrap(winz));
		
        glu.gluUnProject((double) mainFrame.mouseX, (double) (mainFrame.getMainPanel().getCanvas().getHeight()-mainFrame.mouseY), winz[0],
                mvmatrix, 0,
                projmatrix, 0, 
                viewport, 0, 
                wcoord, 0);	
        return new Vector3(wcoord[0], wcoord[1], wcoord[2]);*/
		return null;
	}

	public void setGat(Gat gat)
	{
		this.gat = gat;
	}
	public void setRsw(Rsw rsw)
	{
		this.rsw = rsw;
	}
	public void setGnd(Gnd gnd)
	{
		this.gnd = gnd;
	}		

	public Gat getGat()
	{
		return gat;
	}
	public Gnd getGnd()
	{
		return gnd;
	}

/*	public void destroy(GL4 gl)
	{
		gndRenderer.destroy(gl);
		gatRenderer.destroy(gl);
		rswRenderer.destroy(gl);
		RsmCache.destroy(gl);
		TextureCache.destroy(gl);
	}*/


}
