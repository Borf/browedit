package com.exnw.browedit.gui;

import javax.swing.ButtonGroup;
import javax.swing.ImageIcon;
import javax.swing.JToggleButton;
import javax.swing.JToolBar;

public class ToolToolBar extends JToolBar
{
	private static final long serialVersionUID = 2175329152653456454L;

	JToggleButton basicview;
	
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
		
		add(basicview = new JToggleButton(new ImageIcon(getClass().getClassLoader().getResource("icons/select.png"))), true);
		addSeparator();
		add(select = new JToggleButton(new ImageIcon(getClass().getClassLoader().getResource("icons/select.png"))), true);
		add(move = new JToggleButton(new ImageIcon(getClass().getClassLoader().getResource("icons/move.png"))));
		add(rotate = new JToggleButton(new ImageIcon(getClass().getClassLoader().getResource("icons/rotate.png"))));
		add(scale = new JToggleButton(new ImageIcon(getClass().getClassLoader().getResource("icons/scale.png"))));
		addSeparator();
		add(brush = new JToggleButton(new ImageIcon(getClass().getClassLoader().getResource("icons/brush.png"))));
		add(rectangle = new JToggleButton(new ImageIcon(getClass().getClassLoader().getResource("icons/rectangle.png"))));
		
		group.add(basicview);
		group.add(select);
		group.add(move);
		group.add(rotate);
		group.add(scale);
		group.add(brush);
		group.add(rectangle);
		
		basicview.setSelected(true);
	}
	
	public boolean isBasicViewSelected(){
		return basicview.isSelected();
	}
	
	public boolean isSelectSelected(){
		return basicview.isSelected();
	}
	
	public boolean isMoveSelected(){
		return basicview.isSelected();
	}
	
	public boolean isRotateSelected(){
		return basicview.isSelected();
	}
	
	public boolean isScaleSelected(){
		return basicview.isSelected();
	}
	
	public boolean isBrushSelected(){
		return basicview.isSelected();
	}
	
	public boolean isRectangleSelected(){
		return basicview.isSelected();
	}
}
