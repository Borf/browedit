package com.exnw.browedit.gui;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JToolBar;

public class ToolBar extends JToolBar
{
	private static final long serialVersionUID = -5839738291507072960L;
	JButton buttonOpen;
	public ToolBar(final MainFrame mainFrame)
	{
		add(buttonOpen = new JButton("Open"));
		add(buttonOpen = new JButton("Save"));
		add(buttonOpen = new JButton("Save As"));

	
	
		buttonOpen.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent e)
			{
				mainFrame.menuBar.fileOpen.doClick();
			}
		});
	}
}
