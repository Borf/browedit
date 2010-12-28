package grflib.browedit.exnw.com;

import java.io.FileNotFoundException;
import java.io.InputStream;
import java.util.ArrayList;

public abstract class FileLocation
{
	public abstract InputStream getStream(String fileName) throws FileNotFoundException;
	public abstract boolean isFile(String fileName);
	public abstract void listFiles(ArrayList<String> files, String f);
}
