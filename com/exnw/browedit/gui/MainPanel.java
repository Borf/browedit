package com.exnw.browedit.gui;

import java.awt.BorderLayout;

import javax.media.opengl.GL4;
import javax.media.opengl.awt.GLCanvas;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.ToolTipManager;

import com.exnw.browedit.data.Map;
import com.jogamp.opengl.util.Animator;

public class MainPanel extends JPanel
{
	private static final long serialVersionUID = -5433542069926154067L;
	BrushToolBar brushToolBar;
	ToolToolBar toolToolBar;
	ToolBar toolBar;
	GLCanvas panel;
	
	public MainPanel(MainFrame mainFrame)
	{
		this.setLayout(new BorderLayout());
		
		
		add(toolBar = new ToolBar(mainFrame), BorderLayout.PAGE_START);
		add(toolToolBar = new ToolToolBar(mainFrame), BorderLayout.WEST);
		add(brushToolBar = new BrushToolBar(mainFrame), BorderLayout.EAST);
		

		
		ToolTipManager.sharedInstance().setLightWeightPopupEnabled(false);
		JPopupMenu.setDefaultLightWeightPopupEnabled(false);
		
		
//        GLProfile glprofile = GLProfile.get("GL4");
//        GLCapabilities glcapabilities = new GLCapabilities( glprofile );
        
        panel = new GLCanvas();
		BrowRenderer renderer = new BrowRenderer(mainFrame);
		
		
		panel.addGLEventListener(renderer);
		panel.addMouseListener(renderer);
		panel.addMouseMotionListener(renderer);
		panel.addMouseWheelListener(renderer);
	
		this.add(panel, BorderLayout.CENTER);

	    Animator animator = new Animator(panel);
	    animator.start();
	    animator.setRunAsFastAsPossible(true);
	    panel.requestFocus();
	}

	public void setMap(Map currentMap)
	{
		brushToolBar.setMap(currentMap);
	}
	
	public GL4 getGL()
	{
		return panel.getGL().getGL4();
	}
}
