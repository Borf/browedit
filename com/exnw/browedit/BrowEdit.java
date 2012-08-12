package com.exnw.browedit;

import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

import org.pushingpixels.substance.api.skin.SubstanceGraphiteAquaLookAndFeel;

import com.exnw.browedit.config.Settings;
import com.exnw.browedit.grflib.GrfLib;
import com.exnw.browedit.gui.MainFrame;
import com.exnw.browedit.server.BrowServer;

public class BrowEdit{
	/**
	 * @param args
	 */
	public static void main( String[] args ){
		try{
			if( Settings.json.getBoolean( "readini" ) ){
				System.out.println("Opening GRFs");
				long l = System.currentTimeMillis();
				
				try{
					GrfLib.addGrfs( Settings.json.getString("ini") );
				}catch( java.io.FileNotFoundException e ){
					System.err.println("INI file \""+ Settings.json.getString("ini") +"\" was not found.");
					e.printStackTrace();
				}catch( java.io.IOException e ){
					e.printStackTrace();
				}
				System.out.println("Bench: " + (System.currentTimeMillis() - l));
			}else{
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
			}
		}catch( org.json.JSONException e ){
			System.err.println("Missing readini config.");
			e.printStackTrace();
			System.exit(1);
		}
		
		try{
			if( Settings.json.getBoolean( "readdata" ) ){
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
			}
		}catch( org.json.JSONException e ){
			System.err.println("Missing readdata config.");
			e.printStackTrace();
			System.exit(1);
		}
		
		GrfLib.enableJar();
		

		new BrowServer();
		
	    JFrame.setDefaultLookAndFeelDecorated(true);
	    JDialog.setDefaultLookAndFeelDecorated(true);
	    SwingUtilities.invokeLater(new Runnable() {
	      public void run() {
	        try
			{
				UIManager.setLookAndFeel(new SubstanceGraphiteAquaLookAndFeel());
			} catch (UnsupportedLookAndFeelException e)
			{
				e.printStackTrace();
			}
			new MainFrame();
	      }
	    });
	}

}
