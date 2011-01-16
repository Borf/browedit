package com.exnw.browedit.gui;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JToolBar;

public class ToolBar extends JToolBar
{
	JButton buttonOpen;
	public ToolBar(final MainFrame mainFrame)
	{
		add(buttonOpen = new JButton("Open"));

	
	
	
		buttonOpen.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent e)
			{
				mainFrame.menuBar.fileOpen.doClick();
			}
		});
	}
}
