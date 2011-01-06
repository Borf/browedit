package com.exnw.browedit.render;

import java.util.Observer;

import javax.media.opengl.GL;

public interface Renderer extends Observer
{
	public void render(GL gl);
}
