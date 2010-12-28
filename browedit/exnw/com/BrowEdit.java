package browedit.exnw.com;

import grflib.browedit.exnw.com.GrfLib;
import gui.browedit.exnw.com.MainFrame;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class BrowEdit {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		long l = System.currentTimeMillis();
		GrfLib.addGrf("/home/borf/Ragray/rdata.grf");
		GrfLib.addGrf("/home/borf/Ragray/data.grf");
	//	GrfLib.addDir("/home/borf/Ragray");
		System.out.println("Bench: " + (System.currentTimeMillis() - l));
		
		
		
		new MainFrame();
		
	}

}
