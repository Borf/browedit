package com.exnw.browedit.data;

import java.awt.Color;

import javax.media.opengl.GL;

import com.exnw.browedit.grflib.GrfLib;

/**
 * This java class covers support for reading a Ragnarok Online GAT file,
 * where GAT seems to stand for Ground Altitude Table.
 *  
 * @author Lemongrass
 */
public class Gat
{
	private static byte[] magic = "GRAT".getBytes();
	private static byte[][] supportedVersions = new byte[][]{
		{ 1, 2 }
	};
	
	/*
	 * 0 = walkable block
	 * 1 = non-walkable block
	 * 2 = non-walkable water (not snipable)
	 * 3 = walkable water
	 * 4 = non-walkable water (snipable)
	 * 5 = cliff (snipable)
	 * 6 = cliff (not snipable)
	 * Everything else = unknown
	*/
	private static enum GatCellType{
		WALKABLE_BLOCK (new Color(0.0f,1.0f,0.0f)),
		NON_WALKABLE_BLOCK (new Color(1.0f,0.0f,0.0f)),
		NON_WALKABLE_WATER_NOT_SNIPEABLE (new Color(0.0f,0.0f,1.0f)),
		WALKABLE_WATER (new Color(1.0f,1.0f,0.0f)),
		NON_WALKABLE_WATER_SNIPEABLE (new Color(1.0f,0.0f,1.0f)),
		CLIFF_SNIPEABLE (new Color(0.0f,1.0f,1.0f)),
		CLIFF_NOT_SNIPEABLE (new Color(1.0f,1.0f,1.0f));
		
		private Color color;
		public Color getColor()
		{
			return color;
		}
		GatCellType(Color color)
		{
			this.color = color;
		}
	};
	
	private String filename;
	
	// Version
	private byte version_major;
	private byte version_minor;
	
	// Size
	private int width;
	private int height;
	
	private java.util.List<Gat.GatCell> cells;
	
	public Gat( String filename ){
		if( filename == null || filename.isEmpty() )
			throw new IllegalArgumentException("No empty filename allowed.");
		
		filename = filename.trim();
		
		if( !filename.endsWith(".gat") )
			filename += ".gat";
		
		this.filename = filename;
		this.read();
	}

	public static boolean isSupported( byte major, byte minor ){
		for( byte[]version : Gat.supportedVersions ){
			if( major == version[0] && minor == version[1] ){
				return true;
			}
		}
		return false;
	}
	
	public void read(){
		com.exnw.browedit.io.SwappedInputStream dis = null;
		
		try{
			dis = new com.exnw.browedit.io.SwappedInputStream( GrfLib.openFile( this.filename ) );
			
			for( byte c : Gat.magic ){
				if( c != dis.readByte() ){
					throw new IllegalArgumentException("GAT file header is corrupted.");
				}
			}
			
			this.version_major = dis.readByte();
			this.version_minor = dis.readByte();
			
			if( !Gat.isSupported( this.version_major, this.version_minor) )
				throw new IllegalArgumentException( String.format( "GAT Version %01d.%01d not supported.", this.version_major, this.version_minor ) );
			
			this.cells = new java.util.ArrayList<Gat.GatCell>();
			
			this.width = dis.readInt();
			this.height = dis.readInt();
			
			for( int i = 0, max = this.getWidth() * this.getHeight(); i < max; i++ ){
				this.cells.add( new Gat.GatCell( dis ) );
			}
		}catch( java.io.IOException ex ){
			ex.printStackTrace();
		}finally{
			if( dis != null ){
				try{
					dis.close();
				}catch( java.io.IOException ex ){
					ex.printStackTrace();
				}
			}
		}		
	}
	
	public Gat.GatCell getCell( int x, int y ){
		if( x < 0 || y < 0 )
			throw new IllegalArgumentException();
		
		return this.cells.get( x + y * this.getWidth() );
	}
	
	@Override
	public String toString(){
		return String.format( "Ragnarok Online GAT File(Version %01d.%01d): %s | %01d cells", this.version_major, this.version_minor, this.filename, this.cells.size() );
	}

	public void render(GL gl)
	{
		gl.glBegin(GL.GL_QUADS);
		for(int x = 0; x < getWidth(); x++)
		{
			for(int y = 0; y < getHeight(); y++)
			{
				GatCell cell = getCell(x,y);
				gl.glColor3f(cell.type.getColor().getRed()/255.0f, cell.type.getColor().getGreen()/255.0f, cell.type.getColor().getBlue()/255.0f); 

				gl.glVertex3f(x+0, cell.height[0], y+0);
				gl.glVertex3f(x+1, cell.height[1], y+0);
				gl.glVertex3f(x+1, cell.height[2], y+1);
				gl.glVertex3f(x+0, cell.height[3], y+1);
			}
		}
		gl.glEnd();
	}
	
	public int getWidth()
	{
		return width;
	}

	public int getHeight()
	{
		return height;
	}


	private class GatCell{
		private float[] height;
		private Gat.GatCellType type;
		
		public GatCell(){
			this.height = new float[4];
		}
		
		public GatCell( com.exnw.browedit.io.SwappedInputStream dis ) throws java.io.IOException{
			this();
			this.read( dis );
		}
		
		public void read( com.exnw.browedit.io.SwappedInputStream dis ) throws java.io.IOException{
			for( int i = 0; i < 4; i++ )
				this.height[i] = dis.readFloat();
			
			int type = dis.readInt();
			
			try{
				this.type = Gat.GatCellType.values()[type];
			}catch( ArrayIndexOutOfBoundsException ex ){
				throw new IllegalArgumentException( String.format( "Gat Celltype %01d is unknown.", type ) );
			}
		}
	}
}
