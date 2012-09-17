package com.exnw.browedit.edittools;

import java.awt.event.MouseEvent;

import com.exnw.browedit.gui.MainPanel;
import com.exnw.browedit.math.Vector3;

public class SelectTool extends EditTool
{

	Vector3 startDrag;
	
	public SelectTool(MainPanel mainPanel)
	{
		super(mainPanel);
	}

	public void useLeftMouseDown(MouseEvent e)
	{
		startDrag = mainPanel.renderer.mouse3d;
	}

	public void useLeftMouseUp(MouseEvent e)
	{
		startDrag = mainPanel.renderer.mouse3d;
	}

	public void useLeftMouseDrag(MouseEvent lastMouseEvent, MouseEvent e)
	{
		mainPanel.renderer.changeSelection();
	}

	public void useRightMouseDrag(MouseEvent lastMouseEvent, MouseEvent e)
	{
	}


}
