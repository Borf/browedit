package com.exnw.browedit.config;

public class Settings{
	public static org.json.JSONObject json;
	
	static{
		try{
			Settings.setConfigurationFile( "settings."+System.getProperty("user.name")+".json" );
		}catch( java.io.FileNotFoundException e ){
			System.err.println( "Configuration file \"settings.json\" was not found." );
			e.printStackTrace();
			System.exit( 1 );
		}catch( org.json.JSONException e ){
			System.err.println( "Invalid configuration in \"settings.json\"." );
			e.printStackTrace();
			System.exit( 1 );
		}
	}
	
	public static void setConfigurationFile( String file ) throws java.io.FileNotFoundException, org.json.JSONException{
		Settings.json = new org.json.JSONObject( new org.json.JSONTokener( new java.io.FileInputStream( file ) ) );
	}
}
