package grflib.browedit.exnw.com;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;

public class RealFileLocation extends FileLocation
{
	private String path;
	public RealFileLocation(String path)
	{
		this.path = path;
		if(!this.path.endsWith("/") && !this.path.endsWith("\\"))
			this.path += "/";
	}
	public InputStream getStream(String fileName) throws FileNotFoundException
	{
		return new FileInputStream(this.path + fileName);
	}

	public boolean isFile(String fileName)
	{
		File f = new File(this.path + fileName);
		return f.exists();
	}
	
}
