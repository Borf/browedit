package com.exnw.browedit.gui;


import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JFileChooser;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;

import com.exnw.browedit.grflib.GrfFileSystemView;


public class MenuBar extends JMenuBar
{
	private static final long serialVersionUID = 4113918595080984089L;
	JMenu file;
	JMenuItem fileNew, fileOpen, fileSave, fileSaveAs, fileExit;
	
	JMenu tools;
	
	JMenu maps;
	
	public MenuBar(final MainFrame mainFrame)
	{
		add(file = new JMenu("File"));
		file.add(fileNew = 		new JMenuItem("New"));
		file.add(fileOpen = 	new JMenuItem("Open"));
		file.add(fileSave = 	new JMenuItem("Save"));
		file.add(fileSaveAs = 	new JMenuItem("Save As"));
		file.add(fileExit = 	new JMenuItem("Exit"));
		

	//	add(tools = new JMenu("Tools"));
		

		fileOpen.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent e)
			{
				JFileChooser fileChooser = new JFileChooser(new GrfFileSystemView());
				 if(fileChooser.showOpenDialog(null) == JFileChooser.APPROVE_OPTION)
				 {
//					mainFrame.openMap(fileChooser.getSelectedFile().getPath());
				 }			
			}
		});
		
		
		fileExit.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent e)
			{
				System.exit(0);
			}
		});
		
		
	}
}


