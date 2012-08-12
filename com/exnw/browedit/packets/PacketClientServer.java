package com.exnw.browedit.packets;

import com.exnw.browedit.server.ServerClient;


public interface PacketClientServer extends Packet
{
	public void handlePacket(ServerClient client);
}
