package com.exnw.browedit.net;

import com.exnw.browedit.packets.PacketServerClient;

public class LocalServerClient extends ServerClient
{
	public LocalBrowClient client;
	public Thread thread;
	
	public LocalServerClient(BrowServer server, LocalBrowClient client)
	{
		this.server = server;
		this.client = client;
		/*thread = new Thread(this);
		thread.run();*/
	}
	
	public synchronized void send(PacketServerClient packet)
	{
		packet.handlePacket(client.mainFrame);
	}
	

}
