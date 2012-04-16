package com.exnw.browedit.render;

import java.util.List;
import java.util.Observable;

import javax.media.opengl.GL4;

import com.exnw.browedit.data.Map;
import com.exnw.browedit.data.Rsw;
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
		/*gl.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA);
		gl.glEnable(GL.GL_BLEND);
		gl.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA);
		gl.glPushMatrix();
		gl.glScalef(1,1,-1);
		
		for( Rsw.ModelResource model : this.rsw.getModels() ){
			model.getRsm().getRSMRenderer().render( gl, this.map, model );
		}
		
		gl.glPopMatrix();
		gl.glDisable(GL.GL_BLEND);*/
	}
	
	public void update(Observable arg0, Object arg1)
	{
	}
}
