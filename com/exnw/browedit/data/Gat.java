package com.exnw.browedit.data;

/**
 * This java class covers support for reading a Ragnarok Online GAT file,
 * where GAT seems to stand for Ground Altitude Table.
 *  
 * @author Lemongrass
 */
public class Gat
{
	private static char[] magic = new char[]{ 'G', 'R', 'A', 'T' };
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
		WALKABLE_BLOCK,
		NON_WALKABLE_BLOCK,
		NON_WALKABLE_WATER_NOT_SNIPEABLE,
		WALKABLE_WATER,
		NON_WALKABLE_WATER_SNIPEABLE,
		CLIFF_SNIPEABLE,
		CLIFF_NOT_SNIPEABLE
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
	}
	
	// TODO: hand over an file handler for GRF/Local files,
	// 		 as for now we do it only with local files
	public void read(){
		java.io.DataInputStream dis = null;
		
		try{
			dis = new java.io.DataInputStream( new java.io.FileInputStream( this.filename ) );
			
			for( char c : Gat.magic ){
				if( c != dis.readChar() ){
					throw new IllegalArgumentException("GAT file header is corrupted.");
				}
			}
			
			this.version_major = dis.readByte();
			this.version_minor = dis.readByte();
			
			{
				boolean supported = false;
				
				for( byte[]version : Gat.supportedVersions ){
					if( this.version_major == version[0] && this.version_minor == version[1] ){
						supported = true;
						break;
					}
				}
				
				if( !supported )
					throw new IllegalArgumentException( String.format( "GAT Version %01d.%01d not supported.", this.version_major, this.version_minor ) );
			}
			
			this.cells = new java.util.ArrayList<Gat.GatCell>();
			
			this.width = dis.readInt();
			this.height = dis.readInt();
			
			for( int i = 0, max = this.width * this.height; i < max; i++ ){
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
		
		return this.cells.get( x + y * this.width );
	}
	
	@Override
	public String toString(){
		return String.format( "Ragnarok Online GAT File(Version %01d.%01d): %s | %01d cells", this.version_major, this.version_minor, this.filename, this.cells.size() );
	}
	
	private class GatCell{
		private float[] height;
		private Gat.GatCellType type;
		
		public GatCell(){
			this.height = new float[4];
		}
		
		public GatCell( java.io.DataInputStream dis ) throws java.io.IOException{
			this();
			this.read( dis );
		}
		
		public void read( java.io.DataInputStream dis ) throws java.io.IOException{
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
