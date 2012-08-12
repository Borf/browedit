package com.exnw.browedit.gui;

import java.awt.BorderLayout;
import java.awt.FlowLayout;

import javax.media.opengl.GL4;
import javax.media.opengl.awt.GLCanvas;
import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.SwingUtilities;
import javax.swing.ToolTipManager;

import com.exnw.browedit.data.Map;
import com.exnw.browedit.net.BrowClient;
import com.exnw.browedit.packets.OpenMap;
import com.exnw.browedit.render.MapRenderer;
import com.jogamp.opengl.util.Animator;

public class MainPanel extends JPanel
{
	private static final long serialVersionUID = -5433542069926154067L;
	BrushToolBar brushToolBar;
	ToolToolBar toolToolBar;
	ToolBar toolBar;
	GLCanvas panel;
	
	public BrowClient client;
	
	private Map currentMap = null;
	public MapRenderer mapRenderer = null;
	
	public int mouseX, mouseY;
	
	public BrowRenderer renderer;
	private JLabel lblCoordinats;
	
	public MainPanel(MainFrame mainFrame)
	{
		this.setLayout(new BorderLayout());

	
		JPanel northPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
		add(northPanel, BorderLayout.NORTH);
	
		northPanel.add(toolBar = new ToolBar(mainFrame));
		northPanel.add(new LayerToolBar(mainFrame));
		add(toolToolBar = new ToolToolBar(mainFrame), BorderLayout.WEST);
		add(brushToolBar = new BrushToolBar(mainFrame), BorderLayout.EAST);
		
		
		JPanel statusBar = new JPanel(new FlowLayout(FlowLayout.LEFT, 0,0));
		statusBar.setBorder(BorderFactory.createEtchedBorder());
		
		add(statusBar, BorderLayout.SOUTH);

		statusBar.add(lblCoordinats = new JLabel("0,0"));
		lblCoordinats.setBorder(BorderFactory.createLoweredBevelBorder());

		
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

	public void setCoordinats(final int x, final int y, final int z)
	{
		SwingUtilities.invokeLater(new Runnable()
		{
			public void run()
			{
				lblCoordinats.setText(x + ", " + y + ", " + z);
			}
		});

	}
	
	public void setNewMap(Map map)
	{
		this.currentMap = map;
		if(this.mapRenderer != null)
			this.mapRenderer.destroy(getGL());
		this.mapRenderer = new MapRenderer(this.currentMap, this);		
	}
	
	public int getRenderHeight()
	{
		return panel.getHeight();
	}
	
	public int getRenderWidth()
	{
		return panel.getWidth();
	}
}
