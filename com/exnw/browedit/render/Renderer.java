package com.exnw.browedit.render;

import java.util.Observer;

import javax.media.opengl.GL4;

import com.exnw.browedit.renderutils.Shader;

public interface Renderer extends Observer
{
	public void render(GL4 gl, Shader shader);
}
