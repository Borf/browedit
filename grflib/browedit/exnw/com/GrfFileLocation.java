package grflib.browedit.exnw.com;

import java.io.ByteArrayInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.util.Arrays;
import java.util.HashMap;
import java.util.zip.DataFormatException;
import java.util.zip.Inflater;

public class GrfFileLocation extends FileLocation
{
	private String fileName;
	private RandomAccessFile grfFileStream;
	long fileSize;
	int fileCount;
	
	public class GrfFileDesc
	{
		String name;
		int comprLen;
		int comprLenAligned;
		int len;
		int flags;
		int pos;
		
		public String toString()
		{
			return name + " (" + this.len + ")";
		}
	}
	
	HashMap<String, GrfFileDesc> files = new HashMap<String, GrfFileDesc>();
	
	
	int swap(int other)
	{
		return (((other>>0)&0xFF)<<24) | (((other>>8)&0xFF)<<16) | (((other>>16)&0xFF)<<8) | (((other>>24)&0xFF)<<0);
	}
	
	public GrfFileLocation(String fileName) throws FileNotFoundException
	{
		this.fileName = fileName;
		this.grfFileStream = new RandomAccessFile(fileName,"r");
		try
		{
			
			byte[] header = new byte[16];
			this.grfFileStream.read(header);
			if(!Arrays.equals(header, "Master of Magic\0".getBytes()))
				return;
			
			byte[] crypto = new byte[14];
			this.grfFileStream.read(crypto);
			
			int tocOffset = swap(this.grfFileStream.readInt());
			int c1 = swap(this.grfFileStream.readInt());
			int c2 = swap(this.grfFileStream.readInt());
			fileCount = c2 - c1 - 7;
			int version = swap(this.grfFileStream.readInt());
			fileSize = grfFileStream.length();
			
			this.grfFileStream.seek(tocOffset+46);
			
			if(version == 0x200)
				open_0x200();
			
		} catch (IOException e)
		{
			e.printStackTrace();
		}
		
		
	}

	private void open_0x200() throws IOException
	{
		int comprlen = swap(this.grfFileStream.readInt());
		int uncomprlen = swap(this.grfFileStream.readInt());
		
		byte compr[] = new byte[comprlen];
		this.grfFileStream.read(compr);
		
		Inflater decompresser = new Inflater();
		decompresser.setInput(compr);
		byte[] result = new byte[uncomprlen];
		try
		{
			int resultLength = decompresser.inflate(result);
		} catch (DataFormatException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		decompresser.end();
		
		int index = 0;
		for(int i = 0; i < fileCount; i++)
		{
			GrfFileDesc file = new GrfFileDesc();
			int p = 0;
			while(result[index+p] != 0)
				p++;			
			file.name = new String(result,index, p, "ISO-8859-1");
			file.comprLen = 		((result[index+p+1]&0xFF)<<0) | ((result[index+p+2]&0xFF)<<8) | ((result[index+p+3]&0xFF)<<16) | ((result[index+p+4]&0xFF)<<24);
			file.comprLenAligned =	((result[index+p+5]&0xFF)<<0) | ((result[index+p+6]&0xFF)<<8) | ((result[index+p+7]&0xFF)<<16) | ((result[index+p+8]&0xFF)<<24);
			file.len = 				((result[index+p+9]&0xFF)<<0) | ((result[index+p+10]&0xFF)<<8) | ((result[index+p+11]&0xFF)<<16) | ((result[index+p+12]&0xFF)<<24);
			file.flags =			result[13];
			file.pos = 				46 + (((result[index+p+14]&0xFF)<<0) | ((result[index+p+15]&0xFF)<<8) | ((result[index+p+16]&0xFF)<<16) | ((result[index+p+17]&0xFF)<<24));
			files.put(file.name, file);	
			index += p+18; 
		}
		
		
		
		
	}

	public InputStream getStream(String fileName)
	{
		GrfFileDesc file = files.get(fileName);
		if(file == null)
			return null;
		
		try
		{
			this.grfFileStream.seek(file.pos);
			byte[] compr = new byte[file.comprLenAligned];
			byte[] data = new byte[file.len];
			this.grfFileStream.read(compr);
			Inflater decompresser = new Inflater();
			decompresser.setInput(compr);
			try
			{
				int resultLength = decompresser.inflate(data);
			} catch (DataFormatException e)
			{
				e.printStackTrace();
			}
			decompresser.end();		
			
			return new ByteArrayInputStream(data);

		} catch (IOException e)
		{
			e.printStackTrace();
		}
		return null;
	}


	public boolean isFile(String fileName)
	{
		return files.containsKey(fileName);
	}

}
