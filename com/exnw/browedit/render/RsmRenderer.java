package com.exnw.browedit.render;

import java.util.Observable;

import javax.media.opengl.GL;

import com.exnw.browedit.data.Map;
import com.exnw.browedit.data.Rsm;
import com.exnw.browedit.data.Rsw;
import com.sun.opengl.util.GLUT;

public class RsmRenderer implements Renderer
{
	Rsw.ModelResource modelProperties;
	Rsm rsm;
	Map map;
	
	static GLUT glut = new GLUT();
	
	RsmRenderer(Rsw.ModelResource resource, Map map)
	{
		this.map = map;
		this.modelProperties = resource;
		rsm = new Rsm("data\\model\\" + modelProperties.getModelname());
	}
	public void render(GL gl)
	{
		gl.glPushMatrix();
		gl.glTranslatef(5*map.getGnd().getWidth()+modelProperties.getPosition().getX(), -modelProperties.getPosition().getY(), 5*map.getGnd().getHeight()-modelProperties.getPosition().getZ());
		
		
		glut.glutSolidCube(10);
		
		gl.glPopMatrix();
	}
	public void update(Observable o, Object arg)
	{
		
	}
}
