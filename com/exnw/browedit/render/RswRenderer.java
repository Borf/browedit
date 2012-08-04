package com.exnw.browedit.render;

import java.util.List;
import java.util.Observable;

import javax.media.opengl.GL4;

import com.exnw.browedit.data.Map;
import com.exnw.browedit.data.Rsw;
import com.exnw.browedit.math.Matrix3;
import com.exnw.browedit.renderutils.Shader;

public class RswRenderer implements Renderer
{
	List<RsmRenderer> models;
	Rsw rsw;
	Map map;
	
	public RswRenderer( Rsw rsw, Map map ){
		this.rsw = rsw;
		this.map = map;
	}
	
	public void render(GL4 gl, Shader shader)
	{		
		gl.glBlendFunc(GL4.GL_SRC_ALPHA, GL4.GL_ONE_MINUS_SRC_ALPHA);
		gl.glEnable(GL4.GL_BLEND);
		gl.glBlendFunc(GL4.GL_SRC_ALPHA, GL4.GL_ONE_MINUS_SRC_ALPHA);

		Matrix3 oldNormalMatrix = shader.getUniform("normalMatrix").valueMatrix3; 
		
		shader.getUniform("lighting").set(false);
		
		
		for( Rsw.ModelResource model : this.rsw.getModels() ){
			model.getRsm().getRSMRenderer(gl).render( gl, shader, this.map, model );
		}
		shader.getUniform("lighting").set(true);
		shader.getUniform("normalMatrix").set(oldNormalMatrix); 
		
		gl.glDisable(GL4.GL_BLEND);
	}
	
	public void update(Observable arg0, Object arg1)
	{
	}
}
