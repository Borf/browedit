package gui.browedit.exnw.com;

import java.util.ArrayList;

import javax.swing.JFrame;

import data.browedit.exnw.com.Map;

public class MainFrame extends JFrame
{
	ArrayList<Map> maps = new ArrayList<Map>();
	Map currentMap = null;
	
	public MainFrame()
	{
		super("BrowEdit");
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setContentPane(new MainPanel());

		this.setJMenuBar(new gui.browedit.exnw.com.MenuBar());
		
		
		
		
		this.setSize(1280, 900);
		this.setVisible(true);
	}
}
