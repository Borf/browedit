package grflib.browedit.exnw.com;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import javax.swing.filechooser.FileSystemView;

public class GrfFileSystemView extends FileSystemView
{
	public File createFileObject(File dir, String filename)
	{
		return new File(filename);
	}

	public File createFileObject(String path)
	{
		return new File(path);
	}

	protected File createFileSystemRoot(File f)
	{
		return new File("");
	}

	public File getChild(File parent, String fileName)
	{
		return super.getChild(parent, fileName);
	}

	@Override
	public String getSystemDisplayName(File f)
	{
		// TODO Auto-generated method stub
		return f.getPath();
	}

	public String getSystemTypeDescription(File f)
	{
		return "This is a file";
	}

	public boolean isComputerNode(File dir)
	{
		// TODO Auto-generated method stub
		return super.isComputerNode(dir);
	}

	@Override
	public boolean isDrive(File dir)
	{
		// TODO Auto-generated method stub
		return super.isDrive(dir);
	}

	@Override
	public boolean isFileSystem(File f)
	{
		// TODO Auto-generated method stub
		return super.isFileSystem(f);
	}

	@Override
	public boolean isFileSystemRoot(File dir)
	{
		// TODO Auto-generated method stub
		return super.isFileSystemRoot(dir);
	}

	@Override
	public boolean isParent(File folder, File file)
	{
		// TODO Auto-generated method stub
		return super.isParent(folder, file);
	}

	@Override
	public boolean isRoot(File f)
	{
		// TODO Auto-generated method stub
		return super.isRoot(f);
	}

	@Override
	public Boolean isTraversable(File f)
	{
		if(f.getPath().equals("") || f.getPath().endsWith("\\"))
			return true;
		else
			return false;
	}

	public File getDefaultDirectory()
	{
		return new File("");
	}

	public File[] getFiles(File dir, boolean useFileHiding)
	{
		ArrayList<String> files = GrfLib.listFiles("data\\");
		for(int i = 0; i < files.size(); i++)
		{
			if(!files.get(i).toLowerCase().endsWith(".rsw"))
			{
				files.remove(i);
				i--;
			}
		}
		File[] ret = new File[files.size()];
		for(int i = 0; i < ret.length; i++)
		{
			ret[i] = new File(files.get(i));
		}
		return ret;
	}

	public File getHomeDirectory()
	{
		return new File("");
	}

	public File getParentDirectory(File dir)
	{
		return new File("");
	}

	@Override
	public File[] getRoots()
	{
		File[] files = new File[1];
		files[0] = new File("");
		return files;
	}

	public File createNewFolder(File arg0) throws IOException
	{
		return null;
	}
	
	
}