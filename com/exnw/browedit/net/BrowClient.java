package com.exnw.browedit.net;

import java.io.EOFException;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.util.zip.GZIPInputStream;
import java.util.zip.GZIPOutputStream;

import com.exnw.browedit.gui.MainFrame;
import com.exnw.browedit.packets.PacketClientServer;
import com.exnw.browedit.packets.PacketServerClient;

public class BrowClient implements Runnable
{
	Socket socket;
	Thread thread;
	MainFrame mainFrame;
	
	ObjectOutputStream oos;
	GZIPOutputStream gzipos;
	
	public BrowClient(String host, int port, MainFrame mainFrame)
	{
		this.mainFrame = mainFrame;
		try
		{
			socket = new Socket(host, port);
			gzipos = new GZIPOutputStream(socket.getOutputStream());
			oos = new ObjectOutputStream(gzipos);
			
			thread = new Thread(this);
			thread.start();
			
		} catch (IOException e)
		{
			e.printStackTrace();
		}
		
	}

	public void run()
	{
		try
		{
			ObjectInputStream ois = new ObjectInputStream(new GZIPInputStream(socket.getInputStream()));
			while(true)
			{
				PacketServerClient packet = (PacketServerClient) ois.readObject();
				packet.handlePacket(mainFrame);
			}
		} catch (EOFException e) {}
		catch (IOException e)
		{
			e.printStackTrace();
		} catch (ClassNotFoundException e)
		{
			e.printStackTrace();
		}
	}
	
	public synchronized void send(PacketClientServer packet)
	{
		try
		{
			oos.writeObject(packet);
			oos.flush();
			gzipos.flush();
			gzipos.finish();
		} catch (IOException e)
		{
			e.printStackTrace();
		}
	}

}
