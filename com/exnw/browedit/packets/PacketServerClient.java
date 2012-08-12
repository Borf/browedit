package com.exnw.browedit.packets;

import com.exnw.browedit.gui.MainFrame;


public interface PacketServerClient extends Packet
{
	public void handlePacket(MainFrame mainFrame);
}
