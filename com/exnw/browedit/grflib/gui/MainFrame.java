package com.exnw.browedit.grflib.gui;

import java.util.ArrayList;

import javax.swing.JFrame;

import com.exnw.browedit.data.Map;


public class MainFrame extends JFrame
{
	ArrayList<Map> maps = new ArrayList<Map>();
	Map currentMap = null;
	
	public MainFrame()
	{
		super("BrowEdit");
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setContentPane(new MainPanel());

		this.setJMenuBar(new com.exnw.browedit.grflib.gui.MenuBar());
		
		
		
		
		this.setSize(1280, 900);
		this.setVisible(true);
	}
}
