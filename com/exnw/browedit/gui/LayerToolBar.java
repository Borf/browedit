package com.exnw.browedit.gui;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JToggleButton;
import javax.swing.JToolBar;

public class LayerToolBar extends JToolBar
{
	private static final long serialVersionUID = 4480176463804559560L;
	JToggleButton showObjects;
	JToggleButton showShadow;
	JToggleButton showGrid;
	
	public LayerToolBar(final MainFrame mainFrame)
	{
		add(new JLabel("Show"));
		add(showObjects = new JToggleButton(new ImageIcon(getClass().getClassLoader().getResource("icons/layer_object.png"))), true);
		add(showShadow = new JToggleButton(new ImageIcon(getClass().getClassLoader().getResource("icons/layer_shadow.png"))), true);
		add(showGrid = new JToggleButton(new ImageIcon(getClass().getClassLoader().getResource("icons/layer_grid.png"))), true);
		
		showObjects.setSelected(true);
		showShadow.setSelected(true);
		
		
		showObjects.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent arg0)
			{
				mainFrame.getMainPanel().renderer.showObjects = LayerToolBar.this.showObjects.isSelected();
			}
		});
		showShadow.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent arg0)
			{
				mainFrame.getMainPanel().renderer.showShadow = LayerToolBar.this.showShadow.isSelected();
			}
		});
		showGrid.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent arg0)
			{
				mainFrame.getMainPanel().renderer.showGrid = LayerToolBar.this.showGrid.isSelected();
			}
		});
	}

}
