package com.exnw.browedit.grflib;

import java.io.FileNotFoundException;
import java.io.InputStream;
import java.util.ArrayList;

public class GrfLib {
		private static ArrayList<FileLocation> locations = new ArrayList<FileLocation>();
		
		public static void addGrf(String fileName) 
		{
			try
			{
				locations.add(new GrfFileLocation(fileName));
			} catch (FileNotFoundException e)
			{
				e.printStackTrace();
			}
		}
		public static void addDir(String path)
		{
			locations.add(new RealFileLocation(path));
		}
		
		
		public static InputStream openFile(String fileName)
		{
			for(FileLocation l: locations)
				if(l.isFile(fileName))
				{
					try
					{
						return l.getStream(fileName);
					} catch (FileNotFoundException e)
					{
						e.printStackTrace();
					}
				}
			return null;
		}
		
		public static ArrayList<String> listFiles(String f)
		{
			ArrayList<String> files = new ArrayList<String>();
			for(FileLocation l: locations)
			{
				l.listFiles(files, f);
			}
			return files;
		}
		
}
