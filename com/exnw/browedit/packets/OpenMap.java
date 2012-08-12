package com.exnw.browedit.packets;

import com.exnw.browedit.data.Map;
import com.exnw.browedit.net.ServerClient;

public class OpenMap implements PacketClientServer
{
	String fileName;
	
	public OpenMap(String fileName)
	{
		this.fileName = fileName;
	}

	public void handlePacket(ServerClient client)
	{
		client.server.map = new Map(fileName);
		System.err.println("Server: Opened " + fileName);

		client.server.sendAll(new MapData(client.server.map));		
	}

}
