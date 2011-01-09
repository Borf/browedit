package com.exnw.browedit.data;

public class Rsw{
	private static byte[] magic = "GRSW".getBytes();
	private static byte[][] supportedVersions = new byte[][]{
		{ 1, 2 },
		{ 1, 3 },
		{ 1, 4 },
		{ 1, 5 },
		{ 1, 6 },
		{ 1, 7 },
		{ 1, 8 },
		{ 1, 9 },
		{ 2, 0 },
		{ 2, 1 }
	};
	
	private String filename;
	private byte version_major;
	private byte version_minor;
	
	private String inifile;
	private String gndfile;
	private String gatfile;
	private String scrfile;
	
	private Rsw.Water water;
	private Rsw.Light light;
	
	private int ground_top;
	private int ground_bottom;
	private int ground_left;
	private int ground_right;
	
	private java.util.List<Rsw.ModelResource> models;
	private java.util.List<Rsw.LightResource> lights;
	private java.util.List<Rsw.SoundResource> sounds;
	private java.util.List<Rsw.EffectResource> effects;
	
	public Rsw( String filename ){
		if( filename == null || filename.isEmpty() )
			throw new IllegalArgumentException("No empty filename allowed.");
		
		filename = filename.trim();
		
		if( !filename.endsWith(".rsw") )
			filename += ".rsw";
		
		this.filename = filename;
		this.read();
	}
	
	public static boolean isSupported( byte major, byte minor ){
		for( byte[]version : Rsw.supportedVersions ){
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
			
			for( byte b : Rsw.magic ){
				if( b != dis.readByte() ){
					throw new IllegalArgumentException("RSW file header is corrupted.");
				}
			}
			
			this.version_major = dis.readByte();
			this.version_minor = dis.readByte();
			
			if( !Rsw.isSupported( this.version_major, this.version_minor ) )
				throw new IllegalArgumentException( String.format( "RSW Version %01d.%01d not supported.", this.version_major, this.version_minor ) );
			
			this.inifile = dis.readISOString( 40 );
			this.gndfile = dis.readISOString( 40 );
			
			if( this.version_major > 1 || this.version_minor >= 4 ){
				this.gatfile = dis.readISOString( 40 );
			}
			this.scrfile = dis.readISOString( 40 );
			
			this.water = new Rsw.Water( dis );
			
			this.light = new Rsw.Light( dis );
			
			if( this.version_major > 1 || this.version_minor >= 6 ){
				this.ground_top = dis.readInt();
				this.ground_bottom = dis.readInt();
				this.ground_left = dis.readInt();
				this.ground_right = dis.readInt();
			}else{
				this.ground_top = this.ground_left = -500;
				this.ground_bottom = this.ground_right = 500;
			}
			
			this.setModels(new java.util.ArrayList<Rsw.ModelResource>());
			this.lights = new java.util.ArrayList<Rsw.LightResource>();
			this.sounds = new java.util.ArrayList<Rsw.SoundResource>();
			this.effects = new java.util.ArrayList<Rsw.EffectResource>();
			
			for( int i = 0, count = dis.readInt(), type = 0; i < count; i++ ){
				switch( ( type = dis.readInt() ) ){
					case 1:
						this.getModels().add( new Rsw.ModelResource( dis ) );
						continue;
					case 2:
						this.lights.add( new Rsw.LightResource( dis ) );
						continue;
					case 3:
						this.sounds.add( new Rsw.SoundResource( dis ) );
						continue;
					case 4:
						this.effects.add( new Rsw.EffectResource( dis ) );
						continue;
					default:
						throw new IllegalArgumentException( String.format( "RSW: Unknown Resource type %d.", type ) );
				}
			}
			
			// TODO: borfi needs to read the QuadTree here ;)
			
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
	
	public void setModels(java.util.List<Rsw.ModelResource> models)
	{
		this.models = models;
	}

	public java.util.List<Rsw.ModelResource> getModels()
	{
		return models;
	}

	private class Water{
		private float level;
		private int type;
		private float waveHeight;
		private float waveSpeed;
		private float wavePitch;
		private int animationSpeed;
		
		public Water(){
            this.type = 0;
            this.waveHeight = 1.0f;
            this.waveSpeed = 2.0f;
            this.wavePitch = 50.0f;
            this.animationSpeed = 3;
            this.level = 0.0f;
		}
		
		public Water( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			this();
			this.read( in );
		}

		public void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			if( Rsw.this.version_major > 1 || Rsw.this.version_minor >= 3 ){
				this.level = in.readFloat();
				
				if( Rsw.this.version_major > 1 || Rsw.this.version_minor >= 8 ){
					this.type = in.readInt();
					this.waveHeight = in.readFloat();
					this.waveSpeed = in.readFloat();
					this.wavePitch = in.readFloat();
					
					if( Rsw.this.version_major > 1 || Rsw.this.version_minor >= 9 ){
						this.animationSpeed = in.readInt();
					}
				}
			}
		}
	}
	
	private class Light{
		private int longitude;
		private int latitude;
		private java.awt.Color diffusecolor;
		private java.awt.Color ambientcolor;
		private float intensity;
		
		public Light(){
            this.longitude = 45;
            this.latitude = 45;
            this.diffusecolor = new java.awt.Color( 1.0f, 1.0f, 1.0f );
            this.ambientcolor = new java.awt.Color( 0.3f, 0.3f, 0.3f );
            this.intensity = 0.5f;
		}
		
		public Light( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			this();
			this.read( in );
		}
	
		public void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			if( Rsw.this.version_major > 1 || Rsw.this.version_minor >= 5 ){
				this.longitude = in.readInt();
				this.latitude = in.readInt();
				this.ambientcolor = new java.awt.Color( in.readFloat(), in.readFloat(), in.readFloat() );
				this.diffusecolor = new java.awt.Color( in.readFloat(), in.readFloat(), in.readFloat() );
				
				if( Rsw.this.version_major > 1 || Rsw.this.version_minor >= 7 ){
					this.intensity = in.readFloat();
				}
			}
		}
	}
	
	public class ModelResource{
		private String name;
		private int animationType;
		private float animationSpeed;
		private int blockType;
		private String modelname;
		private String nodename;
		private com.exnw.browedit.math.Vector3 position;
		private com.exnw.browedit.math.Vector3 rotation;
		private com.exnw.browedit.math.Vector3 scale;
		
		public ModelResource(){
			
		}
		
		public ModelResource( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			this();
			this.read( in );
		}

		private void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			if( Rsw.this.version_major > 1 || Rsw.this.version_minor >= 3 ){
				this.name = in.readISOString( 40 );
				this.animationType = in.readInt();
				this.animationSpeed = in.readFloat();
				this.blockType = in.readInt();
			}
			
			this.setModelname(in.readISOString( 80 ));
			this.nodename = in.readISOString( 80 );
			this.setPosition(new com.exnw.browedit.math.Vector3( in ));
			this.setRotation(new com.exnw.browedit.math.Vector3( in ));
			this.setScale(new com.exnw.browedit.math.Vector3( in ));
		}

		public void setModelname(String modelname)
		{
			this.modelname = modelname;
		}

		public String getModelname()
		{
			return modelname;
		}

		public void setPosition(com.exnw.browedit.math.Vector3 position)
		{
			this.position = position;
		}

		public com.exnw.browedit.math.Vector3 getPosition()
		{
			return position;
		}

		public void setRotation(com.exnw.browedit.math.Vector3 rotation)
		{
			this.rotation = rotation;
		}

		public com.exnw.browedit.math.Vector3 getRotation()
		{
			return rotation;
		}

		public void setScale(com.exnw.browedit.math.Vector3 scale)
		{
			this.scale = scale;
		}

		public com.exnw.browedit.math.Vector3 getScale()
		{
			return scale;
		}
	}
	
	private class LightResource{
		private String name;
		private com.exnw.browedit.math.Vector3 position;
		private java.awt.Color color;
		private float range;
		
		public LightResource(){
			
		}
		
		public LightResource( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			this();
			this.read( in );
		}

		private void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			this.name = in.readISOString( 80 );
			this.position = new com.exnw.browedit.math.Vector3( in );
			this.color = new java.awt.Color( in.readFloat(), in.readFloat(), in.readFloat() );
			this.range = in.readFloat();
		}
	}
	
	private class SoundResource{
		private String name;
		private String wavname;
		private com.exnw.browedit.math.Vector3 position;
		private float volume;
		private int width;
		private int height;
		private float range;
		private float cycle;
		
		public SoundResource(){
			
		}
		
		public SoundResource( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			this();
			this.read( in );
		}

		private void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			this.name = in.readISOString( 80 );
			this.wavname = in.readISOString( 80 );
			this.position = new com.exnw.browedit.math.Vector3( in );
			this.volume = in.readFloat();
			this.width = in.readInt();
			this.height = in.readInt();
			this.range = in.readFloat();
			
			if( Rsw.this.version_major >= 2 ){
				this.cycle = in.readFloat();
			}else{
				this.cycle = 4.0f;
			}
		}
	}
	
	private class EffectResource{
		private String name;
		private com.exnw.browedit.math.Vector3 position;
		private int type;
		private float emitSpeed;
		private com.exnw.browedit.math.Vector4 parameter;
		
		public EffectResource(){
			
		}
		
		public EffectResource( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			this();
			this.read( in );
		}

		private void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			this.name = in.readISOString( 80 );
			this.position = new com.exnw.browedit.math.Vector3( in );
			this.type = in.readInt();
			this.emitSpeed = in.readFloat();
			this.parameter = new com.exnw.browedit.math.Vector4( in );
		}
	}
}
