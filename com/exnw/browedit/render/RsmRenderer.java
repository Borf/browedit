package com.exnw.browedit.render;

import java.util.ArrayList;
import java.util.List;
import java.util.Observable;

import javax.media.opengl.GL;
import javax.media.opengl.GL4;

import com.exnw.browedit.data.Map;
import com.exnw.browedit.data.Rsm;
import com.exnw.browedit.data.Rsm.RsmMesh;
import com.exnw.browedit.data.Rsm.RsmMesh.Surface;
import com.exnw.browedit.data.Rsw;
import com.exnw.browedit.math.Vector2;
import com.exnw.browedit.math.Vector3;
import com.exnw.browedit.renderutils.Vbo;
import com.exnw.browedit.renderutils.VertexList;
import com.exnw.browedit.renderutils.vertexFormats.VertexPNT;
import com.jogamp.opengl.util.texture.Texture;

public class RsmRenderer implements Renderer{
	private Rsm rsm;
	
	private MeshRenderer root;
	private List<Texture> textures = new ArrayList<Texture>();
	
	public RsmRenderer( Rsm rsm ){
		this.rsm = rsm;
		
		for( String t : this.rsm.getTextures() )
			this.textures.add( TextureCache.getTexture(null, "data\\texture\\" + t) ); //TODO: GL null here
		
		this.root = new RsmRenderer.MeshRenderer( this.rsm.getRoot(), this.rsm );
	}
	
	public void render( GL4 gl )//, Map map, Rsw.ModelResource modelProperties )
	{
	/*	gl.glPushMatrix();
		
		gl.glTranslatef(5*map.getGnd().getWidth()+modelProperties.getPosition().getX(), -modelProperties.getPosition().getY(), -5*map.getGnd().getHeight()+modelProperties.getPosition().getZ());
		gl.glRotatef(-modelProperties.getRotation().getZ(), 0, 0, 1);
		gl.glRotatef(-modelProperties.getRotation().getX(), 1, 0, 0);
		gl.glRotatef(modelProperties.getRotation().getY(), 0, 1, 0);
		gl.glScalef(modelProperties.getScale().getX(), -modelProperties.getScale().getY(), modelProperties.getScale().getZ());
		gl.glTranslatef(-this.rsm.getRealBBRange().getX(), this.rsm.getRealBBMin().getY(), -this.rsm.getRealBBRange().getZ());
		gl.glEnable(GL.GL_TEXTURE_2D);
		
		this.root.render(gl);
		
		gl.glPopMatrix();*/
	}
	
	public void update( Observable o, Object arg ){}
	
	public class MeshRenderer{
		Rsm.RsmMesh rsmMesh;
		List<Texture> textures = new ArrayList<Texture>();
		private List<MeshRenderer> subMeshes;
		
		private ArrayList<Vbo<VertexPNT>> vbos;
		
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

		public void render(GL4 gl)
		{
/*			gl.glMultMatrixf( this.rsmMesh.getMatrix1().getData(), 0);
			gl.glPushMatrix();
			gl.glMultMatrixf( this.rsmMesh.getMatrix2().getData(), 0);
			
			if( vbos == null )
				this.generateVbos(gl);
			
			gl.glEnable(GL.GL_TEXTURE_2D);
			gl.glEnableClientState(GL.GL_VERTEX_ARRAY);             // activate vertex coords array
			gl.glEnableClientState(GL.GL_TEXTURE_COORD_ARRAY);             // activate vertex coords array

			for( int i = 0; i < this.textures.size(); i++ ){
				if(this.textures.get(i) != null)
					this.textures.get(i).bind();

				vbos.get(i).bind();
				vbos.get(i).setPointers();
				gl.glDrawArrays(GL.GL_TRIANGLES, 0, vbos.get(i).size() /BufferUtil.SIZEOF_FLOAT/8);				
			}
			gl.glDisableClientState(GL.GL_VERTEX_ARRAY);
			gl.glDisableClientState(GL.GL_TEXTURE_COORD_ARRAY);
			
			gl.glBindBuffer(GL.GL_ARRAY_BUFFER, 0);
			
			gl.glPopMatrix();
			
			for( MeshRenderer renderer : this.subMeshes ){
				gl.glPushMatrix();
				renderer.render(gl);
				gl.glPopMatrix();
			}*/
		}
		
		private void generateVbos(GL gl){
			vbos = new ArrayList<Vbo<VertexPNT>>();

			for(int i = 0; i < textures.size(); i++)
			{
				vbos.add(new Vbo<VertexPNT>());
				VertexList<VertexPNT> vertices = new VertexList<VertexPNT>();
				for(Surface surface : rsmMesh.getSurfaces())
				{
					if(surface.getTextureid() == i)
					{
						for(int ii = 0; ii < 3; ii++)
						{
							Vector3 vec = rsmMesh.getVertices().get(surface.getSurfacevertices()[ii]);
							Vector2 tex = rsmMesh.getTextureCoordinats().get(surface.getTexturevertices()[ii]).getCoodinates();
							
							vertices.add(new VertexPNT(vec, new Vector3(0,1,0), tex));
						}
					}
				}
				vbos.get(i).generate(vertices);
			}
			
		}	
	}

}
