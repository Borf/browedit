package com.exnw.browedit.data;

public class Rsm{
	private static byte[] magic = "GRSM".getBytes();
	private static byte[][] supportedVersions = new byte[][]{
		{ 1, 1 },
		{ 1, 2 },
		{ 1, 3 },
		{ 1, 4 },
		{ 1, 5 }
	};
	
	private String filename;
	private byte version_major;
	private byte version_minor;
	
	private int animationlength;
	private int shadetype;
	private byte alpha;
	
	private java.util.List<String> textures;
	private String root;
	private java.util.List<Rsm.RsmMesh> meshes;
	
	public Rsm( String filename ){
		if( filename == null || filename.isEmpty() )
			throw new IllegalArgumentException("No empty filename allowed.");
		
		filename = filename.trim();
		
		if( !filename.endsWith(".rsm") )
			filename += ".rsm";
		
		this.filename = filename;
		this.read();
	}
	
	public static boolean isSupported( byte major, byte minor ){
		for( byte[]version : Rsm.supportedVersions ){
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
			
			for( byte b : Rsm.magic ){
				if( b != dis.readByte() ){
					throw new IllegalArgumentException("RSM file header is corrupted.");
				}
			}
			
			this.version_major = dis.readByte();
			this.version_minor = dis.readByte();
			
			if( !Rsm.isSupported( this.version_major, this.version_minor ) )
				throw new IllegalArgumentException( String.format( "RSM Version %01d.%01d not supported.", this.version_major, this.version_minor ) );
			
			this.animationlength = dis.readInt();
			this.shadetype = dis.readInt();
			
			if( this.version_minor >= 4 ){ // Since RSM 1.4
				this.alpha = dis.readByte();
			}else{
				this.alpha = (byte)0xFF; // TODO: Check this, since you use 0 here.
			}
			
			dis.skip( 16 );
			
			this.textures = new java.util.ArrayList<String>();
			
			for( int i = 0, count = dis.readInt(); i < count; i++ ){
				byte[] buffer = new byte[40];
				dis.readBytes( buffer );
				this.textures.add( new String( buffer, "ISO-8859-1" ).trim().toLowerCase() );
			}
			
			{
				byte[] buffer = new byte[40];
				dis.readBytes( buffer );
				this.root = new String( buffer, "ISO-8859-1" ).trim().toLowerCase();
			}
			
			this.meshes = new java.util.ArrayList<Rsm.RsmMesh>();
			for( int i = 0, count = dis.readInt(); i < count; i++ ){
				Rsm.RsmMesh mesh = new Rsm.RsmMesh();
				mesh.read( dis );
				this.meshes.add( mesh );
			}
			
			if( this.version_minor < 5 ){
				Rsm.RsmMesh mesh = this.meshes.get(0);
				
				mesh.positionframes = new java.util.ArrayList<Rsm.RsmMesh.PositionFrame>();
				
				for( int i = 0, count = dis.readInt(); i < count; i++ ){
					Rsm.RsmMesh.PositionFrame pf = mesh.new PositionFrame();
					
					pf.frame = dis.readInt();
					
					pf.position = mesh.new Vector4();
					
					pf.position.x = dis.readFloat();
					pf.position.y = dis.readFloat();
					pf.position.z = dis.readFloat();
					pf.position.w = dis.readFloat();
				}
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
	
	private class RsmMesh{
		private String name;
		private String parent;
		private java.util.List<Integer> textureids;
		
		private Rsm.RsmMesh.Matrix4 matrix;
		private Rsm.RsmMesh.Vector3 position;
		private Rsm.RsmMesh.Vector3 position2;
		private float rotationangle;
		private Rsm.RsmMesh.Vector3 rotationaxis;
		private Rsm.RsmMesh.Vector3 scale;
		
		private java.util.List<Rsm.RsmMesh.Vector3> vectors;
		private java.util.List<Rsm.RsmMesh.TextureVertex> texturevertices;
		private java.util.List<Rsm.RsmMesh.Surface> surfaces;
		private java.util.List<Rsm.RsmMesh.RotationFrame> rotationframes;
		private java.util.List<Rsm.RsmMesh.PositionFrame> positionframes;
		
		public RsmMesh(){
			
		}
		
		public void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			byte[] buffer = new byte[40];
			
			in.readBytes( buffer );
			this.name = new String( buffer, "ISO-8859-1" ).trim().toLowerCase();
			
			in.readBytes( buffer );
			this.parent = new String( buffer, "ISO-8859-1" ).trim().toLowerCase();
			
			this.textureids = new java.util.ArrayList<Integer>();			
			for( int i = 0, count = in.readInt(); i < count; i++ ){
				this.textureids.add( in.readInt() );
			}
			
			this.matrix = new Rsm.RsmMesh.Matrix4();
			
			this.matrix.m[0][0] = in.readFloat();
			this.matrix.m[0][1] = in.readFloat();
			this.matrix.m[0][2] = in.readFloat();
			this.matrix.m[0][3] = 0;
			
			this.matrix.m[1][0] = in.readFloat();
			this.matrix.m[1][1] = in.readFloat();
			this.matrix.m[1][2] = in.readFloat();
			this.matrix.m[1][3] = 0;
			
			this.matrix.m[2][0] = in.readFloat();
			this.matrix.m[2][1] = in.readFloat();
			this.matrix.m[2][2] = in.readFloat();
			this.matrix.m[2][3] = 0;
			
			this.matrix.m[3][0] = 0;
			this.matrix.m[3][1] = 0;
			this.matrix.m[3][2] = 0;
			this.matrix.m[3][3] = 0;
			
			this.position = new Rsm.RsmMesh.Vector3();
			
			this.position.x = in.readFloat();
			this.position.y = in.readFloat();
			this.position.z = in.readFloat();
			
			this.position2 = new Rsm.RsmMesh.Vector3();
			
			this.position2.x = in.readFloat();
			this.position2.y = in.readFloat();
			this.position2.z = in.readFloat();
			
			this.rotationangle = in.readFloat();
			
			this.rotationaxis = new Rsm.RsmMesh.Vector3();
			
			this.rotationaxis.x = in.readFloat();
			this.rotationaxis.y = in.readFloat();
			this.rotationaxis.z = in.readFloat();
			
			this.scale = new Rsm.RsmMesh.Vector3();
			
			this.scale.x = in.readFloat();
			this.scale.y = in.readFloat();
			this.scale.z = in.readFloat();
			
			this.vectors = new java.util.ArrayList<Rsm.RsmMesh.Vector3>();
			for( int i = 0, count = in.readInt(); i < count; i++ ){
				Rsm.RsmMesh.Vector3 v = new Rsm.RsmMesh.Vector3();
				
				v.x = in.readFloat();
				v.y = in.readFloat();
				v.z = in.readFloat();
				
				this.vectors.add( v );
			}
			
			this.texturevertices = new java.util.ArrayList<Rsm.RsmMesh.TextureVertex>();
			for( int i = 0, count = in.readInt(); i < count; i++ ){
				Rsm.RsmMesh.TextureVertex tv = new Rsm.RsmMesh.TextureVertex();
				
				if( Rsm.this.version_minor >= 2 )
					tv.color = new java.awt.Color( in.readInt() );
				else
					tv.color = new java.awt.Color( 0xFFFFFFFF );
				
				tv.u = in.readFloat();
				tv.v = in.readFloat();
				
				this.texturevertices.add( tv );
			}
			
			this.surfaces = new java.util.ArrayList<Rsm.RsmMesh.Surface>();
			for( int i = 0, count = in.readInt(); i < count; i++ ){
				Rsm.RsmMesh.Surface s = new Rsm.RsmMesh.Surface();
				
				for( int j = 0; j < 3; j++ )
					s.surfacevertices[j] = in.readShort();
				
				for( int j = 0; j < 3; j++ )
					s.texturevertices[j] = in.readShort();
				
				s.textureid = in.readShort();
				s.padding = in.readShort();
				s.twoside = in.readInt();
				
				if( Rsm.this.version_minor >= 2 ){
					s.smoothgroup = in.readInt();
				}else{
					s.smoothgroup = 0;
				}
				
				this.surfaces.add( s );
			}
			
			if( Rsm.this.version_minor < 5 ){
				this.positionframes = new java.util.ArrayList<Rsm.RsmMesh.PositionFrame>();
				
				for( int i = 0, count = in.readInt(); i < count; i++ ){
					Rsm.RsmMesh.PositionFrame pf = new Rsm.RsmMesh.PositionFrame();
					
					pf.frame = in.readInt();
					
					pf.position = new Rsm.RsmMesh.Vector4();
					
					pf.position.x = in.readFloat();
					pf.position.y = in.readFloat();
					pf.position.z = in.readFloat();
					pf.position.w = in.readFloat();
					
					this.positionframes.add( pf );
				}
			}
			
			if( Rsm.this.version_minor == 5 ){
				this.rotationframes = new java.util.ArrayList<Rsm.RsmMesh.RotationFrame>();
				
				for( int i = 0, count = in.readInt(); i < count; i++ ){
					Rsm.RsmMesh.RotationFrame rf = new Rsm.RsmMesh.RotationFrame();
					
					rf.frame = in.readInt();
					
					rf.rotation = new Rsm.RsmMesh.Vector3();
					
					rf.rotation.x = in.readFloat();
					rf.rotation.y = in.readFloat();
					rf.rotation.z = in.readFloat();
				}
			}
		}
		
		private class RotationFrame{
			private int frame;
			private Rsm.RsmMesh.Vector3 rotation;
		}
		
		private class PositionFrame{
			private int frame;
			private Rsm.RsmMesh.Vector4 position;
		}
		
		private class Surface{
			private short[] surfacevertices = new short[3];
			private short[] texturevertices = new short[3];
			private short textureid;
			private short padding;
			private int twoside;
			private int smoothgroup;
		}
		
		private class TextureVertex{
			private float u,v;
			private java.awt.Color color;
		}
		
		private class Matrix4{
			private float[][] m = new float[4][4];
		}
		
		private class Vector3{
			private float x,y,z;
		}
		
		private class Vector4{
			private float w,x,y,z;
		}
	}
}
