package com.exnw.browedit.gui;

import java.util.ArrayList;

import javax.swing.JFrame;

import com.exnw.browedit.data.Map;


public class MainFrame extends JFrame
{
	ArrayList<Map> maps = new ArrayList<Map>();
	private Map currentMap = null;
	MenuBar menuBar;
	MainPanel mainPanel;
	
	public MainFrame()
	{
		super("BrowEdit");
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setContentPane(mainPanel = new MainPanel(this));

		this.setJMenuBar(menuBar = new com.exnw.browedit.gui.MenuBar(this));
		openMap("data\\hugel.rsw");
		
		
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
	
	
}
