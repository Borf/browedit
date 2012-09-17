package com.exnw.browedit.net;

import com.exnw.browedit.gui.MainFrame;
import com.exnw.browedit.packets.PacketClientServer;

public class LocalBrowClient extends BrowClient
{
	BrowServer server;
	LocalServerClient client;
	MainFrame mainFrame;
	
	public LocalBrowClient(BrowServer server, MainFrame mainFrame)
	{
		this.server = server;
		this.mainFrame = mainFrame;
		
		client = new LocalServerClient(server, this);
		server.addClient(client);
	}
	public void send(PacketClientServer packet)
	{
		server.handlePacket(packet, client);
	}

}
