package com.exnw.browedit.net;

import com.exnw.browedit.packets.PacketClientServer;

public abstract class BrowClient
{
	abstract public void send(PacketClientServer packet);
}
