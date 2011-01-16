package com.exnw.browedit.gui;

import java.awt.Dimension;
import java.awt.FlowLayout;

import javax.swing.JButton;
import javax.swing.JToolBar;

public class ToolToolBar extends JToolBar
{
	public ToolToolBar(MainFrame mainFrame)
	{
		super(JToolBar.VERTICAL);
		add(new JButton("Select"));
		add(new JButton("Move"));
		add(new JButton("Rotate"));
		add(new JButton("Scale"));
		addSeparator();
		add(new JButton("Texture Brush"));
		add(new JButton("Height Brush"));
		add(new JButton("Gat Brush"));
		add(new JButton("Object Brush"));
		add(new JButton("Effect Brush"));
		add(new JButton("Sound Brush"));
		add(new JButton("Light Brush"));
		
		
		
	}
}
