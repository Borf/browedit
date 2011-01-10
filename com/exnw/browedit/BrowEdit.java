package com.exnw.browedit;

import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

import com.exnw.browedit.config.Settings;
import com.exnw.browedit.grflib.GrfLib;
import com.exnw.browedit.gui.MainFrame;

public class BrowEdit{
	/**
	 * @param args
	 */
	public static void main( String[] args ){
		try{
			System.out.println("Opening GRFs");
			long l = System.currentTimeMillis();
			org.json.JSONArray grfs = Settings.json.getJSONArray( "grfs" );
			
			for( int i = 0; i < grfs.length(); i++ ){
				//System.out.println("Initiating GRF["+i+"]: "+ grfs.getString(i) );
				try{
					GrfLib.addGrf( grfs.getString(i) );
				}catch( java.io.FileNotFoundException e ){
					System.err.println("Could not find GRF File \"" + grfs.getString(i) + "\".");
					//e.printStackTrace();
				}
			}
			
			System.out.println("Bench: " + (System.currentTimeMillis() - l));
		}catch( org.json.JSONException e ){
			System.err.println("No configuration for any grf file in the settings.");
			e.printStackTrace();
			System.exit(1);
		}
		
		try{
			System.out.println("Opening Directories");
			long l = System.currentTimeMillis();
			org.json.JSONArray dirs = Settings.json.getJSONArray( "datadir" );
			
			for( int i = 0; i < dirs.length(); i++ ){
				try{
					GrfLib.addDir( dirs.getString( i ) );
				}catch( java.io.FileNotFoundException e ){
					System.err.println("Could not find data directory File \"" + dirs.getString(i) + "\".");
					//e.printStackTrace();
				}
			}
			
			System.out.println("Bench: " + (System.currentTimeMillis() - l));
		}catch( org.json.JSONException e ){
			System.err.println("No configuration for any grf file in the settings.");
			e.printStackTrace();
			System.exit(1);
		}
		
		GrfLib.enableJar();
		
		try {
		    // Set System L&F
	        UIManager.setLookAndFeel(
	            UIManager.getSystemLookAndFeelClassName());
	    } 
	    catch (UnsupportedLookAndFeelException e) {
	       // handle exception
	    }
	    catch (ClassNotFoundException e) {
	       // handle exception
	    }
	    catch (InstantiationException e) {
	       // handle exception
	    }
	    catch (IllegalAccessException e) {
	       // handle exception
	    }
		new MainFrame();
	}

}
