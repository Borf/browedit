package com.exnw.browedit.packets;

import com.exnw.browedit.data.Map;
import com.exnw.browedit.server.ServerClient;

public class OpenMap implements PacketClientServer
{
	String fileName;
	
	public void handlePacket(ServerClient client)
	{
		client.server.map = new Map(fileName);
		
	}

}
