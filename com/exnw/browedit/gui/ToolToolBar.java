package com.exnw.browedit.gui;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.ButtonGroup;
import javax.swing.ImageIcon;
import javax.swing.JToggleButton;
import javax.swing.JToolBar;

import com.exnw.browedit.edittools.SelectTool;

public class ToolToolBar extends JToolBar
{
	private static final long serialVersionUID = 2175329152653456454L;

	JToggleButton select;
	JToggleButton move;
	JToggleButton rotate;
	JToggleButton scale;
	
	JToggleButton brush;
	JToggleButton rectangle;
	
	public ToolToolBar(final MainPanel mainPanel)
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
		
		select.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent arg0)
			{
				mainPanel.renderer.currentTool = new SelectTool(mainPanel);
			}
		});
		
		select.setSelected(true);
		mainPanel.renderer.currentTool = new SelectTool(mainPanel);
	}

}
