package com.exnw.browedit.data;

public class Gnd{
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
		
		if( !filename.endsWith(".gat") )
			filename += ".gat";
		
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
			
			this.textures = new java.util.ArrayList<String>();
			
			for( int i = 0, count = this.width * this.height; i < count; i++ ){
				this.textures.add( dis.readISOString() );
			}
			
			int lightmapcount = dis.readInt();
			this.lightmaps = new java.util.ArrayList<Gnd.Lightmap>();
			
			if( dis.readInt() == 8 && dis.readInt() == 8 && dis.readInt() == 1 ) {
                for( int i = 0; i < lightmapcount; i++ )
                    this.lightmaps.add( new Lightmap( dis ) );
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
	
	private class Lightmap{
		private byte[][] brightness;
		private java.awt.Color[][] color;
		
		public Lightmap(){
			this.brightness = new byte[8][8];
			this.color = new java.awt.Color[8][8];
		}
		
		public Lightmap( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			this();
			this.read( in );
		}
		
		public void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
            for( int i = 0; i < 8; i++ ) {
                for( int j = 0; j < 8; j++ ) {
                    this.brightness[i][j] = in.readByte();
                }
            }

            for( int i = 0; i < 8; i++ ) {
                for( int j = 0; j < 8; j++ ) {
                    this.color[i][j] = new java.awt.Color( in.readByte(), in.readByte(), in.readByte() );
                }
            }
		}
	}
	
	private class Surface{
		private float[] u;
		private float[] v;
		private short textureID;
		private short lightmapID;
		private java.awt.Color color;
		
		public Surface(){
			this.u = new float[4];
			this.v = new float[4];
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

            this.textureID = in.readShort();
            this.lightmapID = in.readShort();

            byte b = in.readByte();
            byte g = in.readByte();
            byte r = in.readByte();
            byte a = in.readByte();

            this.color = new java.awt.Color( r, g, b );//, a );		
		}		
	}
	
	private class GndCell{
		private float[] height;
		private int[] surface;
		
		public GndCell(){
			this.height = new float[4];
			this.surface = new int[3];
		}
		
		public GndCell( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			this();
			this.read( in );
		}
		
		public void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
            for( int i = 0; i < 4; i++ )
                this.height[i] = in.readFloat();
            for( int i = 0; i < 3; i++ )
            	this.surface[i] = in.readInt();
		}			
	}
}