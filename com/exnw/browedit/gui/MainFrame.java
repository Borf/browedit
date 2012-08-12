package com.exnw.browedit.gui;

import javax.swing.JFrame;


public class MainFrame extends JFrame
{
	private static final long serialVersionUID = -418172863646438090L;

	MenuBar menuBar;
	private MainPanel mainPanel;
	
	public int mouseX, mouseY;
	
	public MainFrame()
	{
		super("BrowEdit");
		this.setLocationByPlatform(true);
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		
		this.setContentPane(mainPanel = new MainPanel(this));

		MainFrame.this.setJMenuBar(menuBar = new com.exnw.browedit.gui.MenuBar(MainFrame.this));
		
		
		
		this.setSize(1280, 900);
		this.setVisible(true);
	}



	public MainPanel getMainPanel()
	{
		return mainPanel;
	}


	
}
