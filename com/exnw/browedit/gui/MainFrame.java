package com.exnw.browedit.gui;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

import com.exnw.browedit.data.Map;


public class MainFrame extends JFrame
{
	private static final long serialVersionUID = -418172863646438090L;
	private Map currentMap = null;
	MenuBar menuBar;
	private MainPanel mainPanel;
	
	public int mouseX, mouseY;
	
	public MainFrame()
	{
		super("BrowEdit");
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		
		this.setContentPane(mainPanel = new MainPanel(this));

		MainFrame.this.setJMenuBar(menuBar = new com.exnw.browedit.gui.MenuBar(MainFrame.this));
		
	    SwingUtilities.invokeLater(new Runnable() {
		      public void run() {
	    		openMap("data\\prontera.rsw");
		      }
		    });
		
		
		
		this.setSize(1280, 900);
		this.setVisible(true);
	}
	
	public void openMap(String fileName)
	{
		if(currentMap != null)
			currentMap.destroy(getMainPanel().getGL());
		currentMap = new Map(fileName);
	}


	public MainPanel getMainPanel()
	{
		return mainPanel;
	}

	public Map getCurrentMap()
	{
		return currentMap;
	}
	
	
}
