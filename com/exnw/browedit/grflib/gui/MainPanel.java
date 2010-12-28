package com.exnw.browedit.grflib.gui;

import java.awt.BorderLayout;

import javax.media.opengl.GLCanvas;
import javax.swing.JPanel;

import com.sun.opengl.util.Animator;

public class MainPanel extends JPanel
{
	public MainPanel()
	{
		this.setLayout(new BorderLayout());
		GLCanvas canvas = new GLCanvas();
		canvas.addGLEventListener(new BrowRenderer());
		
		this.add(canvas, BorderLayout.CENTER);

	    Animator animator = new Animator(canvas);
	    animator.start();
	    canvas.requestFocus();
	}
	
}
