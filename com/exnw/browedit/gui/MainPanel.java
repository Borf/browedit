package com.exnw.browedit.gui;

import java.awt.BorderLayout;

import javax.media.opengl.GLCapabilities;
import javax.media.opengl.GLProfile;
import javax.media.opengl.awt.GLCanvas;
import javax.swing.JPanel;

import com.exnw.browedit.data.Map;
import com.jogamp.opengl.util.Animator;

public class MainPanel extends JPanel
{
	private static final long serialVersionUID = -5433542069926154067L;
	BrushToolBar brushToolBar;
	ToolToolBar toolToolBar;
	ToolBar toolBar;
	GLCanvas canvas;
	
	public MainPanel(MainFrame mainFrame)
	{
		this.setLayout(new BorderLayout());
		
		
		add(toolBar = new ToolBar(mainFrame), BorderLayout.PAGE_START);
		add(toolToolBar = new ToolToolBar(mainFrame), BorderLayout.WEST);
		add(brushToolBar = new BrushToolBar(mainFrame), BorderLayout.EAST);
		

        GLProfile glprofile = GLProfile.get("GL4");
        GLCapabilities glcapabilities = new GLCapabilities( glprofile );
        canvas = new GLCanvas( glcapabilities );
		
		BrowRenderer renderer = new BrowRenderer(mainFrame);
		
		
		canvas.addGLEventListener(renderer);
		canvas.addMouseListener(renderer);
		canvas.addMouseMotionListener(renderer);
		canvas.addMouseWheelListener(renderer);
		
		this.add(canvas, BorderLayout.CENTER);

	    Animator animator = new Animator(canvas);
	    animator.start();
	    animator.setRunAsFastAsPossible(true);
	    canvas.requestFocus();
	}

	public void setMap(Map currentMap)
	{
		brushToolBar.setMap(currentMap);
		
	}

	public GLCanvas getCanvas()
	{
		return canvas;
	}
	
}
