package com.exnw.browedit.render;

import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Observable;

import javax.media.opengl.GL;

import com.exnw.browedit.data.Map;
import com.exnw.browedit.data.Rsm;
import com.exnw.browedit.data.Rsm.RsmMesh;
import com.exnw.browedit.data.Rsm.RsmMesh.Surface;
import com.exnw.browedit.data.Rsw;
import com.exnw.browedit.math.Vector2;
import com.exnw.browedit.math.Vector3;
import com.sun.opengl.util.BufferUtil;
import com.sun.opengl.util.texture.Texture;

public class RsmRenderer implements Renderer
{
	private static java.util.Map<String,Rsm> loadedmodels = new java.util.HashMap<String, Rsm>();
	
	private static Rsm loadModel( String model ){
		if( RsmRenderer.loadedmodels.containsKey( model ) ){
			return RsmRenderer.loadedmodels.get( model );
		}else{
			Rsm rsm = new Rsm("data\\model\\" + model );
			RsmRenderer.loadedmodels.put( model, rsm );
			return rsm;
		}
	}
	
	private Rsw.ModelResource modelProperties;
	private Rsm rsm;
	private Map map;
	
	private MeshRenderer root;
	private List<Texture> textures = new ArrayList<Texture>();

	RsmRenderer( Rsw.ModelResource resource, Map map ){
		this.map = map;
		this.modelProperties = resource;
		this.rsm =  RsmRenderer.loadModel( this.modelProperties.getModelname() );
		
		for( String t : this.rsm.getTextures() )
			this.textures.add( TextureCache.getTexture("data\\texture\\" + t) );
		
		this.root = new RsmRenderer.MeshRenderer( this.rsm.getRoot(), this.rsm );
	}
	
	public void render(GL gl)
	{
		gl.glPushMatrix();
		gl.glTranslatef(5*map.getGnd().getWidth()+modelProperties.getPosition().getX(), -modelProperties.getPosition().getY(), -5*map.getGnd().getHeight()+modelProperties.getPosition().getZ());
		gl.glRotatef(-modelProperties.getRotation().getZ(), 0, 0, 1);
		gl.glRotatef(-modelProperties.getRotation().getX(), 1, 0, 0);
		gl.glRotatef(modelProperties.getRotation().getY(), 0, 1, 0);
		gl.glScalef(modelProperties.getScale().getX(), -modelProperties.getScale().getY(), modelProperties.getScale().getZ());
		gl.glTranslatef(-this.rsm.getRealBBRange().getX(), this.rsm.getRealBBMin().getY(), -this.rsm.getRealBBRange().getZ());
		gl.glEnable(GL.GL_TEXTURE_2D);
		root.render(gl);
		gl.glPopMatrix();
	}
	
	public void update( Observable o, Object arg ){}
	
	class MeshRenderer{
		Rsm.RsmMesh rsmMesh;
		List<Texture> textures = new ArrayList<Texture>();
		private List<MeshRenderer> subMeshes;
		
		private IntBuffer vbos;
		private int[] polyCount;
		
		public MeshRenderer(RsmMesh rsmMesh, Rsm rsm)
		{
			this.rsmMesh = rsmMesh;
			this.subMeshes = new ArrayList<MeshRenderer>();
			
			for( Rsm.RsmMesh mesh : this.rsmMesh.getSubmeshes() ){
				this.subMeshes.add( new RsmRenderer.MeshRenderer( mesh, rsm ) );
			}
			
			for( Integer tid : this.rsmMesh.getTextureids() )
				this.textures.add( RsmRenderer.this.textures.get( tid.intValue() ) );
		}

		public void render(GL gl)
		{
			gl.glMultMatrixf( this.rsmMesh.getMatrix1().getData(), 0);
			gl.glPushMatrix();
			gl.glMultMatrixf( this.rsmMesh.getMatrix2().getData(), 0);
			
			if( vbos == null )
				this.generateVbos(gl);
			
			gl.glEnable(GL.GL_TEXTURE_2D);
			gl.glEnableClientState(GL.GL_VERTEX_ARRAY);             // activate vertex coords array
			gl.glEnableClientState(GL.GL_TEXTURE_COORD_ARRAY);             // activate vertex coords array

			for( int i = 0; i < this.textures.size(); i++ ){
				this.textures.get(i).bind();

				gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbos.get(2*i));         // for vertex coordinates
				gl.glVertexPointer(3, GL.GL_FLOAT, 0, 0);

				gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbos.get(2*i+1));         // for texture coordinates
				gl.glTexCoordPointer(2, GL.GL_FLOAT, 0, 0);

				gl.glDrawArrays(GL.GL_TRIANGLES, 0, polyCount[i]);				
			}
			gl.glDisableClientState(GL.GL_VERTEX_ARRAY);
			gl.glDisableClientState(GL.GL_TEXTURE_COORD_ARRAY);
			
			gl.glBindBuffer(GL.GL_ARRAY_BUFFER, 0);
			
			gl.glPopMatrix();
			
			for( MeshRenderer renderer : this.subMeshes ){
				gl.glPushMatrix();
				renderer.render(gl);
				gl.glPopMatrix();
			}
		}
		
		private void generateVbos(GL gl){
			vbos = IntBuffer.allocate(textures.size()*2);
			gl.glGenBuffers(textures.size()*2, vbos);		// vertices, texturecoordinats
 			polyCount = new int[textures.size()];
			for(int i = 0; i < textures.size(); i++)
			{
				ArrayList<Float> vertices = new ArrayList<Float>();
				ArrayList<Float> texCoords = new ArrayList<Float>();
				for(Surface surface : rsmMesh.getSurfaces())
				{
					if(surface.getTextureid() == i)
					{
						for(int ii = 0; ii < 3; ii++)
						{
							Vector3 vec = rsmMesh.getVertices().get(surface.getSurfacevertices()[ii]);
							for(int iii = 0; iii < 3; iii++)
								vertices.add(vec.getData()[iii]);

							Vector2 tex = rsmMesh.getTextureCoordinats().get(surface.getTexturevertices()[ii]).getCoodinates();
							for(int iii = 0; iii < 2; iii++)
								texCoords.add(tex.getData()[iii]); 
						}
					}
				}

				FloatBuffer vertexBuf = FloatBuffer.allocate(vertices.size());
				for(int ii = 0; ii < vertices.size(); ii++)
					vertexBuf.put(ii, vertices.get(ii).floatValue());

				gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbos.get(i*2));
				gl.glBufferData(GL.GL_ARRAY_BUFFER, vertexBuf.limit()*BufferUtil.SIZEOF_FLOAT, vertexBuf, GL.GL_STATIC_DRAW);
				
				FloatBuffer texCoordBuf = FloatBuffer.allocate(texCoords.size());
				for(int ii = 0; ii < texCoords.size(); ii++)
					texCoordBuf.put(ii, texCoords.get(ii).floatValue());
				
				gl.glBindBuffer(GL.GL_ARRAY_BUFFER, vbos.get(i*2+1));
				gl.glBufferData(GL.GL_ARRAY_BUFFER, texCoordBuf.limit()*BufferUtil.SIZEOF_FLOAT, texCoordBuf, GL.GL_STATIC_DRAW);		
				
				polyCount[i] = vertexBuf.limit()/3;				
			}
			
		}	
	}
}
