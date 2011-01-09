package com.exnw.browedit;

import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

import com.exnw.browedit.grflib.GrfLib;
import com.exnw.browedit.gui.MainFrame;

public class BrowEdit {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		System.out.println("Opening GRFs");
		long l = System.currentTimeMillis();
		
		try{
			GrfLib.addGrf("F:\\Users\\Lemongrass\\Desktop\\RO\\Client\\data.grf");
			GrfLib.addDir(".");
		}catch( java.io.FileNotFoundException e ){
			e.printStackTrace();
		}
		
		GrfLib.enableJar();

		System.out.println("Bench: " + (System.currentTimeMillis() - l));
		
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
