package com.exnw.browedit.edittools;

import java.awt.event.MouseEvent;

import com.exnw.browedit.gui.MainPanel;

public abstract class EditTool
{
	public MainPanel mainPanel;
	
	public EditTool(MainPanel mainPanel)
	{
		super();
		this.mainPanel = mainPanel;
	}
	public void useLeftMouseDrag(MouseEvent lastMouseEvent, MouseEvent e) {}
	public void useRightMouseDrag(MouseEvent lastMouseEvent, MouseEvent e) {}
	public void useLeftMouseDown(MouseEvent e) {}
	public void useRightMouseDown(MouseEvent e) {}
	public void useLeftMouseUp(MouseEvent e) {}
	public void useRightMouseUp(MouseEvent e) {}
}
