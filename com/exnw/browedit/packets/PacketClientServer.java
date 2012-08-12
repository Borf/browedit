package com.exnw.browedit.packets;

import com.exnw.browedit.net.ServerClient;


public interface PacketClientServer extends Packet
{
	public void handlePacket(ServerClient client);
}
