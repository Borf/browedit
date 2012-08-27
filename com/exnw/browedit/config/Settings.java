package com.exnw.browedit.config;

import java.io.FileNotFoundException;
import java.net.UnknownHostException;

public class Settings{
	public static org.json.JSONObject json;
	
	static{
		Settings.json = null;
		String hostname = "";
		try
		{
			hostname = java.net.InetAddress.getLocalHost().getHostName();
		} catch (UnknownHostException e1)
		{
			e1.printStackTrace();
		}
		setConfig("settings." + hostname + "." + System.getProperty("user.name")+".json");

		if(Settings.json == null)
			setConfig("settings."+System.getProperty("user.name")+".json");
		if(Settings.json == null)
			setConfig("settings.json");
		if(Settings.json == null)
		{
			new FileNotFoundException("settings.json").printStackTrace();
			System.exit( 1 );
		}

	}
	
	public static void setConfigurationFile( String file ) throws java.io.FileNotFoundException, org.json.JSONException{
		Settings.json = new org.json.JSONObject( new org.json.JSONTokener( new java.io.FileInputStream( file ) ) );
	}

	private static void setConfig(String fileName)
	{
		try{
			Settings.setConfigurationFile(fileName);
		}catch( java.io.FileNotFoundException e ){
			System.err.println( "Configuration file \""+fileName+"\" was not found." );
		}catch( org.json.JSONException e ){
			System.err.println( "Invalid configuration in \""+fileName+"\"." );
		}
	}
}
