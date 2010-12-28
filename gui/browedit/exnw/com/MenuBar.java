package gui.browedit.exnw.com;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;


public class MenuBar extends JMenuBar
{
	JMenu file;
	JMenuItem fileNew, fileOpen, fileSave, fileSaveAs, fileExit;
	
	public MenuBar()
	{
		file = new JMenu("File");
		file.add(fileNew = 		new JMenuItem("New"));
		file.add(fileOpen = 	new JMenuItem("Open"));
		file.add(fileSave = 	new JMenuItem("Save"));
		file.add(fileSaveAs = 	new JMenuItem("Save As"));
		file.add(fileExit = 	new JMenuItem("Exit"));
		add(file);
		
		
		
		
		
		fileExit.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent e)
			{
				System.exit(0);
			}
		});
		
		
	}
}
