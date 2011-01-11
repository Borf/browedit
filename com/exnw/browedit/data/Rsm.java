package com.exnw.browedit.data;

import com.exnw.browedit.math.Matrix4;
import com.exnw.browedit.math.Quaternion;
import com.exnw.browedit.math.Vector3;

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
	private Rsm.RsmMesh root;
	private java.util.List<Rsm.RsmMesh> meshes;
	
	// Rendering stuff
	private boolean animated;
	
	private Vector3 bbmin;
	private Vector3 bbmax;
	private Vector3 bbrange;
	
	private Vector3 realbbmin;
	private Vector3 realbbmax;
	private Vector3 realbbrange;
	
	private long lastTick;
	
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
			
			this.setTextures(new java.util.ArrayList<String>());
			
			for( int i = 0, count = dis.readInt(); i < count; i++ ){
				this.getTextures().add( dis.readISOString(40) );
			}
			
			String rootname = dis.readISOString( 40 );
			
			this.setMeshes( new java.util.ArrayList<Rsm.RsmMesh>() );
			for( int i = 0, count = dis.readInt(); i < count; i++ ){
				Rsm.RsmMesh mesh = new Rsm.RsmMesh( dis );
				
				if( mesh.name.equalsIgnoreCase( rootname ) )
					this.setRoot( mesh );
				
				this.getMeshes().add( mesh );
			}
			
/*			if( this.version_minor < 5 ){
				Rsm.RsmMesh mesh = this.getMeshes().get(0);
				
				mesh.positionframes = new java.util.ArrayList<Rsm.RsmMesh.PositionFrame>();
				
				for( int i = 0, count = dis.readInt(); i < count; i++ ){
					Rsm.RsmMesh.PositionFrame pf = mesh.new PositionFrame();
					
					pf.frame = dis.readInt();
					
					pf.position = new com.exnw.browedit.math.Vector4(dis);
					
					mesh.positionframes.add(pf);
				}
			}*/
			
			// Calculation for rendering process
			for( Rsm.RsmMesh mesh : this.getMeshes() ){
				for( Rsm.RsmMesh mesh2 : this.getMeshes() ){
					if( mesh != mesh2 && mesh2.getParent().equalsIgnoreCase( mesh.getName() ) ){
						mesh.subMeshes.add( mesh2 );
					}
				}
			}
			
			this.animated = false;
			
			this.bbmin = new Vector3( 999999, 999999, 999999 );
			this.bbmax = new Vector3(-999999,-999999,-999999 );
			this.getRoot().setBoundingBox( this.bbmin, this.bbmax );
			this.bbrange = new Vector3( ( this.bbmin.getX() + this.bbmax.getX() ) / 2.0f, ( this.bbmin.getY() + this.bbmax.getY() ) / 2.0f, ( this.bbmin.getZ() + this.bbmax.getZ() ) / 2.0f);

			this.realbbmax = new Vector3(-999999,-999999,-999999 );
			this.realbbmin = new Vector3( 999999, 999999, 999999 );
			
			this.getRoot().setRealBoundingBox( Matrix4.makeScale( 1, -1, 1 ), this.realbbmin, this.realbbmax );
			this.realbbrange = new Vector3( ( this.realbbmin.getX() + this.realbbmax.getX() ) / 2.0f, ( this.realbbmin.getY() + this.realbbmax.getY() ) / 2.0f, ( this.realbbmin.getZ() + this.realbbmax.getZ() ) / 2.0f );
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
	
	public Vector3 getRealBBRange(){
		return this.realbbrange;
	}
	
	public Vector3 getRealBBMin(){
		return this.realbbmin;
	}
	
	public void setMeshes(java.util.List<Rsm.RsmMesh> meshes)
	{
		this.meshes = meshes;
	}

	public java.util.List<Rsm.RsmMesh> getMeshes()
	{
		return this.meshes;
	}

	public void setRoot(Rsm.RsmMesh root)
	{
		this.root = root;
	}

	public Rsm.RsmMesh getRoot()
	{
		return this.root;
	}

	public void setTextures(java.util.List<String> textures)
	{
		this.textures = textures;
	}

	public java.util.List<String> getTextures()
	{
		return this.textures;
	}

	public class RsmMesh{
		private String name;
		private String parent;
		private java.util.List<Integer> textureids;
		
		private com.exnw.browedit.math.Matrix4 matrix;
		private com.exnw.browedit.math.Vector3 position;
		private com.exnw.browedit.math.Vector3 position2;

		private float rotationangle;
		private com.exnw.browedit.math.Vector3 rotationaxis;
		private com.exnw.browedit.math.Vector3 scale;
		
		private java.util.List<com.exnw.browedit.math.Vector3> vertices;
		private java.util.List<Rsm.RsmMesh.TextureCoordinate> textureCoordinats;
		private java.util.List<Rsm.RsmMesh.Surface> surfaces;
		private java.util.List<Rsm.RsmMesh.AnimationFrame> animationFrames;
		
		// Rendering stuff
		private Matrix4 matrix1;
		private Matrix4 matrix2;
		private Vector3 bbmin;
		private Vector3 bbmax;
		private Vector3 bbrange;
		private Vector3 realbbmin;
		private java.util.List<Rsm.RsmMesh> subMeshes;
		
		public RsmMesh(){
			this.subMeshes = new java.util.ArrayList<Rsm.RsmMesh>();
		}
		
		public RsmMesh( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			this();
			this.read( in );
		}
		
		public void read( com.exnw.browedit.io.SwappedInputStream in ) throws java.io.IOException{
			this.setName(in.readISOString(40));
			this.setParent(in.readISOString(40));
			
			this.setTextureids(new java.util.ArrayList<Integer>());			
			for( int i = 0, count = in.readInt(); i < count; i++ ){
				this.getTextureids().add( in.readInt() );
			}
			
			this.setMatrix(new com.exnw.browedit.math.Matrix4(in));
			
			this.setPosition(new com.exnw.browedit.math.Vector3(in));
			
			this.position2 = new com.exnw.browedit.math.Vector3(in);
			
			this.setRotationangle(in.readFloat());
			
			this.setRotationaxis(new com.exnw.browedit.math.Vector3(in));
			
			this.setScale(new com.exnw.browedit.math.Vector3(in));
			
			this.setVertices(new java.util.ArrayList<com.exnw.browedit.math.Vector3>());
			for( int i = 0, count = in.readInt(); i < count; i++ ){				
				this.getVertices().add( new com.exnw.browedit.math.Vector3(in) );
			}
			
			this.setTextureCoordinats(new java.util.ArrayList<Rsm.RsmMesh.TextureCoordinate>());
			for( int i = 0, count = in.readInt(); i < count; i++ ){
				Rsm.RsmMesh.TextureCoordinate tv = new Rsm.RsmMesh.TextureCoordinate();
				
				if( Rsm.this.version_minor >= 2 )
					tv.color = new java.awt.Color( in.readInt() );
				else
					tv.color = new java.awt.Color( 0xFFFFFFFF );
				
				tv.setCoodinates(new com.exnw.browedit.math.Vector2(in));
				
				this.getTextureCoordinats().add( tv );
			}
			
			this.setSurfaces(new java.util.ArrayList<Rsm.RsmMesh.Surface>());
			for( int i = 0, count = in.readInt(); i < count; i++ ){
				Rsm.RsmMesh.Surface s = new Rsm.RsmMesh.Surface();
				
				for( int j = 0; j < 3; j++ )
					s.getSurfacevertices()[j] = in.readShort();
				
				for( int j = 0; j < 3; j++ )
					s.getTexturevertices()[j] = in.readShort();
				
				s.setTextureid(in.readShort());
				s.padding = in.readShort();
				s.twoside = in.readInt();
				
				if( Rsm.this.version_minor >= 2 ){
					s.smoothgroup = in.readInt();
				}else{
					s.smoothgroup = 0;
				}
				
				this.getSurfaces().add( s );
			}
			
			this.setAnimationFrames(new java.util.ArrayList<Rsm.RsmMesh.AnimationFrame>());
			for( int i = 0, count = in.readInt(); i < count; i++ ){
				Rsm.RsmMesh.AnimationFrame rf = new Rsm.RsmMesh.AnimationFrame();
				
				rf.setTime(in.readInt());
				rf.setQuat(new com.exnw.browedit.math.Quaternion(in));
				
				this.getAnimationFrames().add(rf);
			}
		}
		
		public void setRealBoundingBox( Matrix4 mat, Vector3 realbbmin, Vector3 realbbmax ){
			Matrix4 mat1 = mat.mult( this.getMatrix1() );	
			Matrix4 mat2 = mat.mult( this.getMatrix1() ).mult( this.getMatrix2() );
			
			for( Surface surface : this.getSurfaces() ){
				for( int i = 0; i < 3; i++ ){
					Vector3 v = this.getVertices().get( surface.getSurfacevertices()[i] );
					
					v = mat2.mult(v);
					
					for( int ii = 0; ii < 3; ii++ ){
						realbbmin.getData()[ii] = Math.min( realbbmin.getData()[ii], v.getData()[ii] );
						realbbmax.getData()[ii] = Math.max( realbbmax.getData()[ii], v.getData()[ii] );
					}
				}
			}
			
			for( Rsm.RsmMesh mesh : this.subMeshes ){
				mesh.setRealBoundingBox( mat1, realbbmin, realbbmax );
			}
		}

		public void setBoundingBox( Vector3 _bbmin, Vector3 _bbmax ){
			if( this.getAnimationFrames().size() > 0 )
				Rsm.this.animated = true;
			this.bbmin = new Vector3( 999999, 999999, 999999);
			this.bbmax = new Vector3(-999999,-999999,-999999);
			
			if( !this.isRoot() ){
				this.bbmin = new Vector3(0,0,0);
				this.bbmax = new Vector3(0,0,0);
			}
			
			Matrix4 myMat = this.getMatrix();
			
			for( Surface surface : this.getSurfaces() ){
				for( int i = 0; i < 3; i++ ){
					Vector3 v = this.getVertices().get( surface.getSurfacevertices()[i] );
					
					v = myMat.mult(v);
					
					if( !this.isRoot() || this.subMeshes.size() != 0 ){
						v.add( this.getPosition() );
						v.add( this.getPosition2() );
					}
					
					for( int ii = 0; ii < 3; ii++ ){
						bbmin.getData()[ii] = Math.min( this.bbmin.getData()[ii], v.getData()[ii] );
						bbmax.getData()[ii] = Math.max( this.bbmax.getData()[ii], v.getData()[ii] );
					}
				}
			}
			
			this.bbrange = new Vector3( ( this.bbmin.getX() + this.bbmax.getX() ) / 2.0f, ( this.bbmin.getY() + this.bbmax.getY() ) / 2.0f, ( this.bbmin.getZ() + this.bbmax.getZ() ) / 2.0f );
			
			for( int i = 0; i < 3; i++ ){
				_bbmin.getData()[i] = Math.min( _bbmin.getData()[i], this.bbmin.getData()[i] );
				_bbmax.getData()[i] = Math.max( _bbmax.getData()[i], this.bbmax.getData()[i] );
			}
			
			for( Rsm.RsmMesh mesh : this.subMeshes )
				mesh.setBoundingBox( _bbmin, _bbmax );			
		}
		
		public Matrix4 getMatrix1(){
			if( this.matrix1 != null )
				return this.matrix1;
			
			this.matrix1 = new Matrix4();
			
			if( this.isRoot() ){
				if( this.subMeshes.size() > 0 ){
					this.matrix1 = this.matrix1.mult( Matrix4.makeTranslation( -Rsm.this.bbrange.getX(), -Rsm.this.bbmax.getY(), -Rsm.this.bbrange.getZ() ) );
				}else{
					this.matrix1= this.matrix1.mult( Matrix4.makeTranslation( 0, -Rsm.this.bbmax.getY() + Rsm.this.bbrange.getY(), 0 ) );
				}
			}else{
				this.matrix1 = this.matrix1.mult( Matrix4.makeTranslation( this.getPosition2().getX(), this.getPosition2().getY(), this.getPosition2().getZ() ) );
			}
				
			if( this.getAnimationFrames().size() == 0 ){
				this.matrix1 = this.matrix1.mult( Matrix4.makeRotation( (float)Math.toDegrees( this.getRotationangle() ), this.getRotationaxis().getX(), this.getRotationaxis().getY(), this.getRotationaxis().getZ() ) );
			}else{
				int current = 0;
				for( int i = 0; i < this.getAnimationFrames().size(); i++ ){
					if( this.getAnimationFrames().get( i ).getTime() > Rsm.this.lastTick ){
						current = i-1;
						break;
					}
				}
				
				if( current < 0 )
					current = 0;
				
				int next = current+1;
				if( next >= this.getAnimationFrames().size() )
					next = 0;

				float interval = ( (float) ( Rsm.this.lastTick - this.getAnimationFrames().get( current ).getTime() ) ) / ( (float) ( this.getAnimationFrames().get( next ).getTime() - this.getAnimationFrames().get( current ).getTime() ) );

				Quaternion quat = new Quaternion( this.getAnimationFrames().get( current ).getQuat(), this.getAnimationFrames().get( next ).getQuat(), interval );

				quat = quat.getNormalized();

				this.matrix1 = this.matrix1.mult( quat.getRotationMatrix() );

				Rsm.this.lastTick = ( System.currentTimeMillis() / 1 ) % this.getAnimationFrames().get( this.getAnimationFrames().size() - 1 ).getTime();
			}
			
			this.matrix1 = this.matrix1.mult( Matrix4.makeScale( this.getScale().getX(), this.getScale().getY(), this.getScale().getZ() ) );
			
			if( this.getAnimationFrames().size() != 0 ){
				Matrix4 m = this.matrix1;
				this.matrix1 = null;
				return m;
			}
			
			return this.matrix1;
		}
		
		public Matrix4 getMatrix2(){
			if( this.matrix2 != null )
				return this.matrix2;

			this.matrix2 = new Matrix4();
			
			if( this.isRoot() && this.subMeshes.size() == 0 ){
				this.matrix2 = this.matrix2.mult( Matrix4.makeTranslation( -Rsm.this.bbrange.getX(), -Rsm.this.bbrange.getY(), -Rsm.this.bbrange.getZ() ) );
			}else{ // if( !isRoot() || subMeshes.size() != 0 )
				this.matrix2 = this.matrix2.mult( Matrix4.makeTranslation( this.getPosition().getX(), this.getPosition().getY(), this.getPosition().getZ() ) );
			}
			
			this.matrix2 = this.matrix2.mult( this.getMatrix() );
				
			return this.matrix2;
		}
		
		public boolean isRoot(){
			return Rsm.this.root == this;
		}
		
		public java.util.List<Rsm.RsmMesh> getSubmeshes(){
			return this.subMeshes;
		}
		
		public void setParent(String parent)
		{
			this.parent = parent;
		}

		public String getParent()
		{
			return parent;
		}

		public void setName(String name)
		{
			this.name = name;
		}

		public String getName()
		{
			return name;
		}

		public void setSurfaces(java.util.List<Rsm.RsmMesh.Surface> surfaces)
		{
			this.surfaces = surfaces;
		}

		public java.util.List<Rsm.RsmMesh.Surface> getSurfaces()
		{
			return surfaces;
		}

		public void setVertices(java.util.List<com.exnw.browedit.math.Vector3> vertices)
		{
			this.vertices = vertices;
		}

		public java.util.List<com.exnw.browedit.math.Vector3> getVertices()
		{
			return vertices;
		}

		public void setPosition(com.exnw.browedit.math.Vector3 position)
		{
			this.position = position;
		}

		public com.exnw.browedit.math.Vector3 getPosition()
		{
			return position;
		}
		
		public com.exnw.browedit.math.Vector3 getPosition2()
		{
			return position2;
		}

		public void setPosition2(com.exnw.browedit.math.Vector3 position2)
		{
			this.position2 = position2;
		}

		public void setRotationangle(float rotationangle)
		{
			this.rotationangle = rotationangle;
		}

		public float getRotationangle()
		{
			return rotationangle;
		}

		public void setRotationaxis(com.exnw.browedit.math.Vector3 rotationaxis)
		{
			this.rotationaxis = rotationaxis;
		}

		public com.exnw.browedit.math.Vector3 getRotationaxis()
		{
			return rotationaxis;
		}

		public void setScale(com.exnw.browedit.math.Vector3 scale)
		{
			this.scale = scale;
		}

		public com.exnw.browedit.math.Vector3 getScale()
		{
			return scale;
		}

		public void setMatrix(com.exnw.browedit.math.Matrix4 matrix)
		{
			this.matrix = matrix;
		}

		public com.exnw.browedit.math.Matrix4 getMatrix()
		{
			return matrix;
		}

		public void setTextureCoordinats(java.util.List<Rsm.RsmMesh.TextureCoordinate> textureCoordinats)
		{
			this.textureCoordinats = textureCoordinats;
		}

		public java.util.List<Rsm.RsmMesh.TextureCoordinate> getTextureCoordinats()
		{
			return textureCoordinats;
		}

		public void setTextureids(java.util.List<Integer> textureids)
		{
			this.textureids = textureids;
		}

		public java.util.List<Integer> getTextureids()
		{
			return textureids;
		}

		public void setAnimationFrames(java.util.List<Rsm.RsmMesh.AnimationFrame> animationFrames)
		{
			this.animationFrames = animationFrames;
		}

		public java.util.List<Rsm.RsmMesh.AnimationFrame> getAnimationFrames()
		{
			return animationFrames;
		}

		public class AnimationFrame
		{
			private int time;
			private Quaternion quat;
			public void setQuat(Quaternion quat)
			{
				this.quat = quat;
			}
			public Quaternion getQuat()
			{
				return quat;
			}
			public void setTime(int time)
			{
				this.time = time;
			}
			public int getTime()
			{
				return time;
			}

		}

		private class RotationFrame{
			private int frame;
		}
		
		private class PositionFrame{
			private int frame;
		}
		
		public class Surface{
			private short[] surfacevertices = new short[3];
			private short[] texturevertices = new short[3];
			private short textureid;
			private short padding;
			private int twoside;
			private int smoothgroup;
			public void setSurfacevertices(short[] surfacevertices)
			{
				this.surfacevertices = surfacevertices;
			}
			public short[] getSurfacevertices()
			{
				return surfacevertices;
			}
			public void setTexturevertices(short[] texturevertices)
			{
				this.texturevertices = texturevertices;
			}
			public short[] getTexturevertices()
			{
				return texturevertices;
			}
			public void setTextureid(short textureid)
			{
				this.textureid = textureid;
			}
			public short getTextureid()
			{
				return textureid;
			}
		}
		
		public class TextureCoordinate{
			private com.exnw.browedit.math.Vector2 coodinates;
			private java.awt.Color color;
			public void setCoodinates(com.exnw.browedit.math.Vector2 coodinates)
			{
				this.coodinates = coodinates;
			}
			public com.exnw.browedit.math.Vector2 getCoodinates()
			{
				return coodinates;
			}
		}
	}
}
