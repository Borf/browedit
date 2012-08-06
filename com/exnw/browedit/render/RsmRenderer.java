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
import com.exnw.browedit.math.Matrix3;
import com.exnw.browedit.math.Matrix4;
import com.exnw.browedit.math.Vector2;
import com.exnw.browedit.math.Vector3;
import com.exnw.browedit.renderutils.Shader;
import com.exnw.browedit.renderutils.Vbo;
import com.exnw.browedit.renderutils.VertexList;
import com.exnw.browedit.renderutils.vertexFormats.VertexPNT;
import com.jogamp.common.nio.Buffers;
import com.jogamp.opengl.util.texture.Texture;

public class RsmRenderer {// implements Renderer{
	private Rsm rsm;
	
	private MeshRenderer root;
	private List<Texture> textures = new ArrayList<Texture>();
	
	public RsmRenderer( Rsm rsm, GL4 gl ){
		this.rsm = rsm;
		
		for( String t : this.rsm.getTextures() )
			this.textures.add( TextureCache.getTexture(gl, "data\\texture\\" + t) ); //TODO: GL null here
		
		this.root = new RsmRenderer.MeshRenderer( this.rsm.getRoot(), this.rsm );
	}
	
	public void render( GL4 gl, Shader shader, Map map, Rsw.ModelResource modelProperties )
	{
		
		Matrix4 modelMatrix = Matrix4.makeScale(1, 1, -1);
		
		
		modelMatrix = modelMatrix.mult(Matrix4.makeTranslation(5*map.getGnd().getWidth()+modelProperties.getPosition().getX(), -modelProperties.getPosition().getY(), -5*map.getGnd().getHeight()+modelProperties.getPosition().getZ()));
		modelMatrix = modelMatrix.mult(Matrix4.makeRotation(-modelProperties.getRotation().getZ(), 0, 0, 1));
		modelMatrix = modelMatrix.mult(Matrix4.makeRotation(-modelProperties.getRotation().getX(), 1, 0, 0));
		modelMatrix = modelMatrix.mult(Matrix4.makeRotation(modelProperties.getRotation().getY(), 0, 1, 0));
		modelMatrix = modelMatrix.mult(Matrix4.makeScale(modelProperties.getScale().getX(), -modelProperties.getScale().getY(), modelProperties.getScale().getZ()));
		modelMatrix = modelMatrix.mult(Matrix4.makeTranslation(-this.rsm.getRealBBRange().getX(), this.rsm.getRealBBMin().getY(), -this.rsm.getRealBBRange().getZ()));
		
		this.root.render(gl, shader, modelMatrix);
		
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

		public void render(GL4 gl, Shader shader, Matrix4 modelMatrix)
		{
			modelMatrix = modelMatrix.mult(this.rsmMesh.getMatrix1());
			Matrix4 currentModelMatrix = modelMatrix.mult(this.rsmMesh.getMatrix2());
			
			if( vbos == null )
				this.generateVbos(gl, shader);
			

			shader.getUniform("modelMatrix", gl).set(currentModelMatrix, gl);
			Matrix4 modelViewMatrix = currentModelMatrix;//shader.getUniform("viewMatrix").valueMatrix4.mult(currentModelMatrix);
			Matrix3 normalMatrix = new Matrix3(
						modelViewMatrix.getValue(0,0), modelViewMatrix.getValue(0,1), modelViewMatrix.getValue(0,2),
						modelViewMatrix.getValue(1,0), modelViewMatrix.getValue(1,1), modelViewMatrix.getValue(1,2),
						modelViewMatrix.getValue(2,0), modelViewMatrix.getValue(2,1), modelViewMatrix.getValue(2,2));
			normalMatrix = normalMatrix.invert().transpose();		
			shader.getUniform("normalMatrix", gl).set(normalMatrix, gl);
			
			
			for( int i = 0; i < this.textures.size(); i++ ){
				if(this.textures.get(i) != null)
					this.textures.get(i).bind(gl);

				vbos.get(i).bind();
				gl.glDrawArrays(GL.GL_TRIANGLES, 0, vbos.get(i).size() / Buffers.SIZEOF_FLOAT/8);				
			}
			
			for( MeshRenderer renderer : this.subMeshes ){
				renderer.render(gl, shader, modelMatrix);
			}
		}
		
		private void generateVbos(GL gl, Shader shader){
			vbos = new ArrayList<Vbo<VertexPNT>>();

			for(int i = 0; i < textures.size(); i++)
			{
				vbos.add(new Vbo<VertexPNT>());
				VertexList<VertexPNT> vertices = new VertexList<VertexPNT>();
				for(Surface surface : rsmMesh.getSurfaces())
				{
					if(surface.getTextureid() == i)
					{
						Vector3 normal = rsmMesh.getVertices().get(surface.getSurfacevertices()[0]).sub(rsmMesh.getVertices().get(surface.getSurfacevertices()[1])).cross(rsmMesh.getVertices().get(surface.getSurfacevertices()[0]).sub(rsmMesh.getVertices().get(surface.getSurfacevertices()[2])));
						
						for(int ii = 0; ii < 3; ii++)
						{
							Vector3 vec = rsmMesh.getVertices().get(surface.getSurfacevertices()[ii]);
							Vector2 tex = rsmMesh.getTextureCoordinats().get(surface.getTexturevertices()[ii]).getCoodinates();
							
							vertices.add(new VertexPNT(vec, normal, tex));
						}
					}
				}
				vbos.get(i).generate(vertices, shader);
			}
			
		}

		public void destroy(GL4 gl)
		{
			for(Vbo<VertexPNT> vbo : vbos)
				vbo.destroy(gl);
			for(MeshRenderer renderer : this.subMeshes)
				renderer.destroy(gl);
		}	
	}

	public void destroy(GL4 gl)
	{
		if(root != null)
			root.destroy(gl);		
	}

}
