package com.exnw.browedit.net;

import com.exnw.browedit.packets.PacketServerClient;

public abstract class ServerClient
{
	public BrowServer server;
	
	public abstract void send(PacketServerClient packet);
}
