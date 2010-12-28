package gui.browedit.exnw.com;

import javax.media.opengl.GLCanvas;
import javax.swing.JFrame;

import com.sun.opengl.util.Animator;

public class MainFrame extends JFrame
{
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
