package com.exnw.browedit.gui;

import javax.swing.ButtonGroup;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JToggleButton;
import javax.swing.JToolBar;

public class ToolToolBar extends JToolBar
{
	JToggleButton select;
	JToggleButton move;
	JToggleButton rotate;
	JToggleButton scale;
	
	JToggleButton brush;
	JToggleButton rectangle;
	
	
	public ToolToolBar(MainFrame mainFrame)
	{
		super(JToolBar.VERTICAL);
		ButtonGroup group = new ButtonGroup();
		
		add(select = new JToggleButton(new ImageIcon(getClass().getClassLoader().getResource("icons/select.png"))), true);
		add(move = new JToggleButton(new ImageIcon(getClass().getClassLoader().getResource("icons/move.png"))));
		add(rotate = new JToggleButton(new ImageIcon(getClass().getClassLoader().getResource("icons/rotate.png"))));
		add(scale = new JToggleButton(new ImageIcon(getClass().getClassLoader().getResource("icons/scale.png"))));
		addSeparator();
		add(brush = new JToggleButton(new ImageIcon(getClass().getClassLoader().getResource("icons/brush.png"))));
		add(rectangle = new JToggleButton(new ImageIcon(getClass().getClassLoader().getResource("icons/rectangle.png"))));
	
		group.add(select);
		group.add(move);
		group.add(rotate);
		group.add(scale);
		group.add(brush);
		group.add(rectangle);
	}
}
