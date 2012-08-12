package com.exnw.browedit.gui;

import java.awt.BorderLayout;
import java.awt.FlowLayout;

import javax.media.opengl.GL4;
import javax.media.opengl.awt.GLCanvas;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.ToolTipManager;

import com.exnw.browedit.data.Map;
import com.exnw.browedit.net.BrowClient;
import com.exnw.browedit.packets.OpenMap;
import com.jogamp.opengl.util.Animator;

public class MainPanel extends JPanel
{
	private static final long serialVersionUID = -5433542069926154067L;
	BrushToolBar brushToolBar;
	ToolToolBar toolToolBar;
	ToolBar toolBar;
	GLCanvas panel;
	
	public BrowClient client;
	
	
	public BrowRenderer renderer;
	
	public MainPanel(MainFrame mainFrame)
	{
		this.setLayout(new BorderLayout());

	
		JPanel northPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
		add(northPanel, BorderLayout.NORTH);
	
		northPanel.add(toolBar = new ToolBar(mainFrame));
		northPanel.add(new LayerToolBar(mainFrame));
		add(toolToolBar = new ToolToolBar(mainFrame), BorderLayout.WEST);
		add(brushToolBar = new BrushToolBar(mainFrame), BorderLayout.EAST);
		

		
		ToolTipManager.sharedInstance().setLightWeightPopupEnabled(false);
		JPopupMenu.setDefaultLightWeightPopupEnabled(false);
		
		
//        GLProfile glprofile = GLProfile.get("GL4");
//        GLCapabilities glcapabilities = new GLCapabilities( glprofile );
        
        panel = new GLCanvas();
		renderer = new BrowRenderer(mainFrame);
		
		
		panel.addGLEventListener(renderer);
		panel.addMouseListener(renderer);
		panel.addMouseMotionListener(renderer);
		panel.addMouseWheelListener(renderer);
	
		this.add(panel, BorderLayout.CENTER);

	    Animator animator = new Animator(panel);
	    animator.start();
	    animator.setRunAsFastAsPossible(true);
	    panel.requestFocus();
	    
	    client = new BrowClient("localhost", 8203, mainFrame);
	    
	    client.send(new OpenMap("data\\prontera.rsw"));
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
