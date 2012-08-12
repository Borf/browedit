package com.exnw.browedit.data;

import java.io.Serializable;

public class Gnd implements Serializable {
	private static byte[] magic = "GRGN".getBytes();
	private static byte[][] supportedVersions = new byte[][]{
		{ 1, 7 }
	};
	
	private String filename;
	private byte version_major;
	private byte version_minor;
	
	private int width;
	private int height;

	private float zoomfactor;
	
	private java.util.List<String> textures;
	private java.util.List<Gnd.Lightmap> lightmaps;
	private java.util.List<Gnd.Surface> surfaces;

	private java.util.List<Gnd.GndCell> cells;
	
	public Gnd( String filename ){
		if( filename == null || filename.isEmpty() )
			throw new IllegalArgumentException("No empty filename allowed.");
		
		filename = filename.trim();
		
		if( !filename.endsWith(".gnd") )
			filename += ".gnd";
		
		this.filename = filename;
		this.read();
	}
	
	public static boolean isSupported( byte major, byte minor ){
		for( byte[]version : Gnd.supportedVersions ){
			if( major == version[0] && minor == version[1] ){
				return true;
			}
		}
		return false;
	}
	
	public void read(){
		com.exnw.browedit.io.SwappedInputStream dis = null;
		
		try{
			dis = new com.exnw.browedit.io.SwappedInputStream( com.exnw.browedit.grflib.GrfLib.openFile( this.filename ) );
			
			for( byte b : Gnd.magic ){
				if( b != dis.readByte() ){
					throw new IllegalArgumentException("GND file header is corrupted.");
				}
			}
			
			this.version_major = dis.readByte();
			this.version_minor = dis.readByte();
			
			if( !Gnd.isSupported( this.version_major, this.version_minor ) )
				throw new IllegalArgumentException( String.format( "GND Version %01d.%01d not supported.", this.version_major, this.version_minor ) );
			
			this.width = dis.readInt();
			this.height = dis.readInt();
			this.zoomfactor = dis.readFloat();
			
			this.setTextures( new java.util.ArrayList<String>() );
			
			//TODO: what's maxTexname for?
			for( int i = 0, count = dis.readInt(), maxTexname = dis.readInt(); i < count; i++ ){
				this.getTextures().add( dis.readISOString( 80 ) );
			}
			
			this.setLightmaps(new java.util.ArrayList<Gnd.Lightmap>());
			{
				int lightmapcount = dis.readInt();
				
				if( dis.readInt() == 8 && dis.readInt() == 8 && dis.readInt() == 1 ) {
	                for( int i = 0; i < lightmapcount; i++ )
	                    this.getLightmaps().add( new Lightmap( dis ) );
	            }
			}
			
			this.surfaces = new java.util.ArrayList<Gnd.Surface>();
			
			for( int i = 0, count = dis.readInt(); i < count; i++ ){
				this.surfaces.add( new Surface( dis ) );
			}
			
			this.cells = new java.util.ArrayList<Gnd.GndCell>();
			
			for( int i = 0, count = this.width * this.height; i < count; i++ ){
				this.cells.add( new Gnd.GndCell( dis ) );
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
	
	public void setTextures( java.util.List<String> textures ){
		this.textures = textures;
	}
	public int getWidth(){
		return this.width;
	}
	public int getHeight(){
		return this.height;
	}	

	public java.util.List<String> getTextures(){
		return this.textures;
	}
	
	public GndCell getCell( int x, int y ){
		return this.cells.get( x + this.width * y );		
	}
	
	public java.util.List<Gnd.Surface> getSurfaces(){
		return this.surfaces;
	}

	public void setSurfaces( java.util.List<Gnd.Surface> surfaces ){
		this.surfaces = surfaces;
	}

	public void setLightmaps(java.util.List<Gnd.Lightmap> lightmaps)
	{
		this.lightmaps = lightmaps;
	}

	public java.util.List<Gnd.Lightmap> getLightmaps()
	{
		return lightmaps;
	}

	public class Lightmap implements Serializable{
		private byte[][] brightness;
		private java.awt.Color[][] color;
		
		public Lightmap(){
			this.setBrightness(new byte[8][8]);
			this.setColor(new java.awt.Color[8][8]);
		}
		
		public Lightmap( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			this();
			this.read( in );
		}
		
		public void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
            for( int i = 0; i < 8; i++ ) {
                for( int j = 0; j < 8; j++ ) {
                    this.getBrightness()[i][j] = in.readByte();
                }
            }

            for( int i = 0; i < 8; i++ ) {
                for( int j = 0; j < 8; j++ ) {
            		this.getColor()[i][j] = new java.awt.Color( in.readByte()&0xff, in.readByte()&0xff, in.readByte()&0xff );
                }
            }
		}

		public void setBrightness(byte[][] brightness)
		{
			this.brightness = brightness;
		}

		public byte[][] getBrightness()
		{
			return brightness;
		}

		public void setColor(java.awt.Color[][] color)
		{
			this.color = color;
		}

		public java.awt.Color[][] getColor()
		{
			return color;
		}
	}
	
	public class Surface implements Serializable{
		private float[] u;
		private float[] v;
		private short textureID;
		private short lightmapID;
		private java.awt.Color color;
		
		public Surface(){
			this.setU( new float[4] );
			this.setV( new float[4] );
		}
		
		public Surface( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			this();
			this.read( in );
		}
		
		public void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
            for( int i = 0; i < 4; i++ )
                this.u[i] = in.readFloat();

            for( int i = 0; i < 4; i++ )
                this.v[i] = in.readFloat();

            this.setTextureID(in.readShort());
            this.setLightmapID(in.readShort());

            byte b = in.readByte();
            byte g = in.readByte();
            byte r = in.readByte();
            byte a = in.readByte();

            this.setColor(new java.awt.Color( r & 0xff, g & 0xff, b & 0xff ));//, a );		
		}

		public void setTextureID( short textureID ){
			this.textureID = textureID;
		}

		public short getTextureID(){
			return textureID;
		}
		
		public float[] getU(){
			return u;
		}

		public void setU(float[] u){
			this.u = u;
		}

		public float[] getV(){
			return v;
		}

		public void setV( float[] v ){
			this.v = v;
		}

		public void setLightmapID(short lightmapID)
		{
			this.lightmapID = lightmapID;
		}

		public short getLightmapID()
		{
			return lightmapID;
		}

		public void setColor(java.awt.Color color)
		{
			this.color = color;
		}

		public java.awt.Color getColor()
		{
			return color;
		}
	}
	
	public class GndCell implements Serializable{
		private float[] height;
		private int[] surface;
		
		public GndCell(){
			this.setHeight( new float[4] );
			this.setSurface( new int[3] );
		}
		
		public GndCell( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			this();
			this.read( in );
		}
		
		public void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
            for( int i = 0; i < 4; i++ )
                this.getHeight()[i] = in.readFloat();
            for( int i = 0; i < 3; i++ )
            	this.getSurface()[i] = in.readInt();
		}

		public void setHeight( float[] height ){
			this.height = height;
		}

		public float[] getHeight(){
			return this.height;
		}

		public void setSurface( int[] surface ){
			this.surface = surface;
		}

		public int[] getSurface(){
			return this.surface;
		}			
	}
}