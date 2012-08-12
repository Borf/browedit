package com.exnw.browedit.packets;

import com.exnw.browedit.data.Map;
import com.exnw.browedit.gui.MainFrame;

public class MapData implements PacketServerClient
{
	Map map;
	
	public MapData(Map map)
	{
		this.map = map;
	}

	public void handlePacket(MainFrame mainFrame)
	{
		System.err.println("Client: Got a new map");
		mainFrame.getMainPanel().setNewMap(map);
	}

}
