package com.exnw.browedit.gui;

import java.util.ArrayList;

import javax.swing.JFrame;

import com.exnw.browedit.data.Map;


public class MainFrame extends JFrame
{
	private static final long serialVersionUID = -418172863646438090L;
	ArrayList<Map> maps = new ArrayList<Map>();
	private Map currentMap = null;
	MenuBar menuBar;
	private MainPanel mainPanel;
	
	public int mouseX, mouseY;
	
	public MainFrame()
	{
		super("BrowEdit");
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setContentPane(mainPanel = new MainPanel(this));

		this.setJMenuBar(menuBar = new com.exnw.browedit.gui.MenuBar(this));
		openMap("data\\prontera.rsw");
		
		
		this.setSize(1280, 900);
		this.setVisible(true);
	}
	
	public void openMap(String fileName)
	{
		Map map = new Map(fileName);
		maps.add(currentMap);
		menuBar.addMap(fileName);
		setCurrentMap(map);
	}

	public Map getCurrentMap()
	{
		return currentMap;
	}

	public void setCurrentMap(Map currentMap)
	{
		this.currentMap = currentMap;
		((MainPanel)this.getContentPane()).setMap(currentMap);
	}

	public MainPanel getMainPanel()
	{
		return mainPanel;
	}
	
	
}
