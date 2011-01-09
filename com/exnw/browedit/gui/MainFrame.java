package com.exnw.browedit.gui;

import java.util.ArrayList;

import javax.swing.JFrame;

import com.exnw.browedit.data.Map;


public class MainFrame extends JFrame
{
	ArrayList<Map> maps = new ArrayList<Map>();
	Map currentMap = null;
	MenuBar menuBar;
	
	public MainFrame()
	{
		super("BrowEdit");
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setContentPane(new MainPanel(this));

		this.setJMenuBar(menuBar = new com.exnw.browedit.gui.MenuBar(this));
		openMap("data\\prontera.rsw");
		
		
		this.setSize(1280, 900);
		this.setVisible(true);
	}
	
	public void openMap(String fileName)
	{
		currentMap = new Map(fileName);
		maps.add(currentMap);
		menuBar.addMap(fileName);
	}
	
	
}
