package grflib.browedit.exnw.com;

import java.io.FileNotFoundException;
import java.io.InputStream;

public abstract class FileLocation
{
	public abstract InputStream getStream(String fileName) throws FileNotFoundException;
	public abstract boolean isFile(String fileName);

}
