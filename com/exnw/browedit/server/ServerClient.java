package com.exnw.browedit.server;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;

import com.exnw.browedit.packets.PacketClientServer;

public class ServerClient implements Runnable
{
	public BrowServer server;
	Socket socket;
	Thread thread;
	ObjectOutputStream oos;
	
	public ServerClient(Socket clientSocket, BrowServer server)
	{
		this.server = server;
		socket = clientSocket;
		try
		{
			oos = new ObjectOutputStream(socket.getOutputStream());
		} catch (IOException e)
		{
			e.printStackTrace();
		}
		thread = new Thread(this);
		thread.run();
	}

	public void run()
	{
		try
		{
			server.addClient(this);
			ObjectInputStream ois = new ObjectInputStream(socket.getInputStream());

			while(true)
			{
				PacketClientServer packet = (PacketClientServer) ois.readObject();
				packet.handlePacket(this);				
			}			
		} catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (ClassNotFoundException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		server.removeClient(this);
		
	}

}
