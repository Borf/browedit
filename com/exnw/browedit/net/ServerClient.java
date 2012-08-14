package com.exnw.browedit.net;

import java.io.EOFException;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.util.zip.GZIPInputStream;
import java.util.zip.GZIPOutputStream;

import com.exnw.browedit.packets.PacketClientServer;
import com.exnw.browedit.packets.PacketServerClient;

public class ServerClient implements Runnable
{
	public BrowServer server;
	Socket socket;
	Thread thread;
	ObjectOutputStream oos;
	GZIPOutputStream gzipos;
	
	public ServerClient(Socket clientSocket, BrowServer server)
	{
		this.server = server;
		socket = clientSocket;
		try
		{
			gzipos = new GZIPOutputStream(socket.getOutputStream(), true);
			oos = new ObjectOutputStream(gzipos);
		} catch (IOException e)
		{
			e.printStackTrace();
		}
		thread = new Thread(this);
		thread.run();
	}

	public void run()
	{
		System.err.println("Server: Handling connection from " + socket.getInetAddress());
		try
		{
			server.addClient(this);
			ObjectInputStream ois = new ObjectInputStream(new GZIPInputStream(socket.getInputStream()));

			while(true)
			{
				PacketClientServer packet = (PacketClientServer) ois.readObject();
				server.handlePacket(packet, this);				
			}			
		} catch(EOFException e)	{	}
		catch (IOException e)
		{
			e.printStackTrace();
		} catch (ClassNotFoundException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		server.removeClient(this);
	}
	
	public synchronized void send(PacketServerClient packet)
	{
		try
		{
			oos.writeObject(packet);
			oos.flush();
			gzipos.flush();
			//gzipos.finish();
		} catch (IOException e)
		{
			e.printStackTrace();
		}
	}
	

}
