package com.exnw.browedit.render;

import java.util.ArrayList;
import java.util.List;
import java.util.Observable;

import javax.media.opengl.GL;

import com.exnw.browedit.data.Map;
import com.exnw.browedit.data.Rsm;
import com.exnw.browedit.data.Rsm.RsmMesh;
import com.exnw.browedit.data.Rsm.RsmMesh.Surface;
import com.exnw.browedit.data.Rsw;
import com.exnw.browedit.math.Matrix4;
import com.exnw.browedit.math.Vector2;
import com.exnw.browedit.math.Vector3;
import com.sun.opengl.util.texture.Texture;

public class RsmRenderer implements Renderer
{
	Rsw.ModelResource modelProperties;
	Rsm rsm;
	Map map;
	
	Vector3 bbmin;
	Vector3 bbmax;
	Vector3 bbrange;
	
	private Vector3 realbbmin;
	private Vector3 realbbmax;
	private Vector3 realbbrange;
	
	MeshRenderer root;
	List<Texture> textures = new ArrayList<Texture>();

	RsmRenderer(Rsw.ModelResource resource, Map map)
	{
		this.map = map;
		this.modelProperties = resource;
		rsm = new Rsm("data\\model\\" + modelProperties.getModelname());
		
		for(String t : rsm.getTextures())
			textures.add(TextureCache.getTexture("data\\texture\\" + t));
		
		for(Rsm.RsmMesh mesh : rsm.getMeshes())
		{
			if(mesh.getName().equals(rsm.getRoot()))
			{
				root = new MeshRenderer(mesh, rsm);
				break;
			}
		}
		
		bbmin = new Vector3( 999999, 999999, 999999);
		bbmax = new Vector3(-999999,-999999,-999999);
		root.setBoundingBox(bbmin, bbmax);
		bbrange = new Vector3((bbmin.getX()+bbmax.getX())/2.0f, (bbmin.getY()+bbmax.getY())/2.0f, (bbmin.getZ()+bbmax.getZ())/2.0f);

		realbbmax = new Vector3(-999999, -999999, -999999);
		realbbmin = new Vector3(999999, 999999, 999999);
		
		Matrix4 mat = Matrix4.makeScale(1, -1, 1);
		root.setRealBoundingBox(mat, realbbmin, realbbmax);
		realbbrange = new Vector3((realbbmin.getX()+realbbmax.getX())/2.0f, (realbbmin.getY()+realbbmax.getY())/2.0f, (realbbmin.getZ()+realbbmax.getZ())/2.0f);
		
		
	}
	public void render(GL gl)
	{
		gl.glPushMatrix();
		gl.glTranslatef(5*map.getGnd().getWidth()+modelProperties.getPosition().getX(), -modelProperties.getPosition().getY(), -5*map.getGnd().getHeight()+modelProperties.getPosition().getZ());
		gl.glRotatef(-modelProperties.getRotation().getZ(), 0, 0, 1);
		gl.glRotatef(-modelProperties.getRotation().getX(), 1, 0, 0);
		gl.glRotatef(modelProperties.getRotation().getY(), 0, 1, 0);
		gl.glScalef(modelProperties.getScale().getX(), -modelProperties.getScale().getY(), modelProperties.getScale().getZ());
		gl.glTranslatef(-realbbrange.getX(), realbbrange.getY(), -realbbrange.getZ());
		
		/*
		gl.glDisable(GL.GL_TEXTURE_2D);
		gl.glColor3f(1,0,0);
		gl.glLineWidth(4);
		gl.glBegin(GL.GL_LINE_LOOP);
		gl.glVertex3f(realbbmin.getX(), -realbbmin.getY(), realbbmin.getZ());
		gl.glVertex3f(realbbmax.getX(), -realbbmin.getY(), realbbmin.getZ());
		gl.glVertex3f(realbbmax.getX(), -realbbmin.getY(), realbbmax.getZ());
		gl.glVertex3f(realbbmin.getX(), -realbbmin.getY(), realbbmax.getZ());
		gl.glEnd();
		gl.glBegin(GL.GL_LINE_LOOP);
		gl.glVertex3f(realbbmin.getX(), -realbbmax.getY(), realbbmin.getZ());
		gl.glVertex3f(realbbmax.getX(), -realbbmax.getY(), realbbmin.getZ());
		gl.glVertex3f(realbbmax.getX(), -realbbmax.getY(), realbbmax.getZ());
		gl.glVertex3f(realbbmin.getX(), -realbbmax.getY(), realbbmax.getZ());
		gl.glEnd();
		gl.glBegin(GL.GL_LINES);
		gl.glVertex3f(realbbmin.getX(), -realbbmin.getY(), realbbmin.getZ());
		gl.glVertex3f(realbbmin.getX(), -realbbmax.getY(), realbbmin.getZ());
		gl.glVertex3f(realbbmax.getX(), -realbbmin.getY(), realbbmin.getZ());
		gl.glVertex3f(realbbmax.getX(), -realbbmax.getY(), realbbmin.getZ());
		gl.glVertex3f(realbbmax.getX(), -realbbmin.getY(), realbbmax.getZ());
		gl.glVertex3f(realbbmax.getX(), -realbbmax.getY(), realbbmax.getZ());
		gl.glVertex3f(realbbmin.getX(), -realbbmin.getY(), realbbmax.getZ());
		gl.glVertex3f(realbbmin.getX(), -realbbmax.getY(), realbbmax.getZ());
		gl.glEnd();
		gl.glColor4f(1,1,1,1);
		
		*/
		gl.glEnable(GL.GL_TEXTURE_2D);
		root.render(gl);
		
		gl.glPopMatrix();
	}
	public void update(Observable o, Object arg)
	{
		
	}
	
	
	class MeshRenderer
	{
		Rsm.RsmMesh rsmMesh;
		List<Texture> textures = new ArrayList<Texture>();
		private List<MeshRenderer> subMeshes;
		private Matrix4 matrix1;
		private Matrix4 matrix2;
		private Vector3 bbmin;
		private Vector3 bbmax;
		private Vector3 bbrange;
		private Vector3 realbbmin;
		
		public MeshRenderer(RsmMesh rsmMesh, Rsm rsm)
		{
			this.rsmMesh = rsmMesh;
			this.subMeshes = new ArrayList<MeshRenderer>();
			
			for(Rsm.RsmMesh mesh : rsm.getMeshes())
			{
				if(mesh.getParent().equals(this.rsmMesh.getName()))
				{
					this.subMeshes.add(new MeshRenderer(mesh, rsm));
				}
			}
			
			for(Integer tid : rsmMesh.getTextureids())
				textures.add(RsmRenderer.this.textures.get(tid.intValue()));
		}
		
		
		public void setRealBoundingBox(Matrix4 mat, Vector3 realbbmin,Vector3 realbbmax)
		{
			Matrix4 mat1 = mat.mult(getMatrix1());	
			Matrix4 mat2 = mat.mult(getMatrix1()).mult(getMatrix2());
			
			for(Surface surface : rsmMesh.getSurfaces())
			{
				for(int i = 0; i < 3; i++)
				{
					Vector3 v = rsmMesh.getVertices().get(surface.getSurfacevertices()[i]);
					v = mat2.mult(v);
					for(int ii = 0; ii < 3; ii++)
					{
						realbbmin.getData()[ii] = Math.min(realbbmin.getData()[ii], v.getData()[ii]);
						realbbmax.getData()[ii] = Math.max(realbbmax.getData()[ii], v.getData()[ii]);
					}
				}
			}
			for(MeshRenderer mesh : subMeshes)
				mesh.setRealBoundingBox(mat1, realbbmin, realbbmax);			
			
			
		}


		public void setBoundingBox(Vector3 _bbmin, Vector3 _bbmax)
		{
			bbmin = new Vector3( 999999, 999999, 999999);
			bbmax = new Vector3(-999999,-999999,-999999);
			
			if(!isRoot())
			{
				bbmin = new Vector3(0,0,0);
				bbmax = new Vector3(0,0,0);
			}
			
			Matrix4 myMat = rsmMesh.getMatrix();
			
			for(Surface surface : rsmMesh.getSurfaces())
			{
				for(int i = 0; i < 3; i++)
				{
					Vector3 v = rsmMesh.getVertices().get(surface.getSurfacevertices()[i]);
					v = myMat.mult(v);
					if(!isRoot() || subMeshes.size() != 0)
					{
						v.add(rsmMesh.getPosition());
						v.add(rsmMesh.getPosition2());
					}
					for(int ii = 0; ii < 3; ii++)
					{
						bbmin.getData()[ii] = Math.min(bbmin.getData()[ii], v.getData()[ii]);
						bbmax.getData()[ii] = Math.max(bbmax.getData()[ii], v.getData()[ii]);
					}
				}
			}
			bbrange = new Vector3((bbmin.getX()+bbmax.getX())/2.0f, (bbmin.getY()+bbmax.getY())/2.0f, (bbmin.getZ()+bbmax.getZ())/2.0f);
			
			for(int i = 0; i < 3; i++)
			{
				_bbmin.getData()[i] = Math.min(_bbmin.getData()[i], bbmin.getData()[i]);
				_bbmax.getData()[i] = Math.max(_bbmax.getData()[i], bbmax.getData()[i]);
			}
			
			for(MeshRenderer mesh : subMeshes)
				mesh.setBoundingBox(_bbmin, _bbmax);			
		}


		public void render(GL gl)
		{
			gl.glMultMatrixf(getMatrix1().getData(), 0);
			gl.glPushMatrix();
			gl.glMultMatrixf(getMatrix2().getData(), 0);
			
			for(RsmMesh.Surface surface : rsmMesh.getSurfaces())
			{
				textures.get(surface.getTextureid()).bind();
				gl.glBegin(GL.GL_TRIANGLES);
				Vector3 v1 = rsmMesh.getVertices().get(surface.getSurfacevertices()[0]);
				Vector3 v2 = rsmMesh.getVertices().get(surface.getSurfacevertices()[1]);
				Vector3 v3 = rsmMesh.getVertices().get(surface.getSurfacevertices()[2]);
				
				Vector2 t1 = rsmMesh.getTextureCoordinats().get(surface.getTexturevertices()[0]).getCoodinates();
				Vector2 t2 = rsmMesh.getTextureCoordinats().get(surface.getTexturevertices()[1]).getCoodinates();
				Vector2 t3 = rsmMesh.getTextureCoordinats().get(surface.getTexturevertices()[2]).getCoodinates();
				
			/*	v1 = new Vector3(v1.getX(), -v1.getY(), v1.getZ());
				v2 = new Vector3(v2.getX(), -v2.getY(), v2.getZ());
				v3 = new Vector3(v3.getX(), -v3.getY(), v3.getZ());*/
	
				gl.glTexCoord2fv(t1.getData(),0);
				gl.glVertex3fv(v1.getData(),0);
				gl.glTexCoord2fv(t2.getData(),0);
				gl.glVertex3fv(v2.getData(),0);
				gl.glTexCoord2fv(t3.getData(),0);
				gl.glVertex3fv(v3.getData(),0);
				gl.glEnd();
				
			}
			
			gl.glPopMatrix();
			for(MeshRenderer renderer : subMeshes)
			{
				gl.glPushMatrix();
				renderer.render(gl);
				gl.glPopMatrix();
			}
		}
		
		public boolean isRoot()
		{
			return true;
		}
		
		
		public Matrix4 getMatrix1()
		{
			if(matrix1 != null)
				return matrix1;
			
			matrix1 = new Matrix4();
			
			if(isRoot())
			{
				if(subMeshes.size() > 0)
					matrix1 = matrix1.mult(Matrix4.makeTranslation(-RsmRenderer.this.bbrange.getX(), -RsmRenderer.this.bbmax.getY(), -RsmRenderer.this.bbrange.getZ()));
				else
					matrix1= matrix1.mult(Matrix4.makeTranslation(0, -RsmRenderer.this.bbmax.getY()+RsmRenderer.this.bbrange.getY(), 0));
			}
			
			if(!isRoot())
				matrix1 = matrix1.mult(Matrix4.makeTranslation(rsmMesh.getPosition2().getX(), rsmMesh.getPosition2().getY(), rsmMesh.getPosition2().getZ()));
				
			if(rsmMesh.getAnimationFrames().size() == 0)
				matrix1 = matrix1.mult(Matrix4.makeRotation((float) (rsmMesh.getRotationangle()*180.0/Math.PI), rsmMesh.getRotationaxis().getX(), rsmMesh.getRotationaxis().getY(), rsmMesh.getRotationaxis().getZ()));
			else
			{
				//TODO: animate
				matrix1 = matrix1.mult(rsmMesh.getAnimationFrames().get(0).getQuat().getNormalized().getRotationMatrix());
			}
			
			matrix1 = matrix1.mult(Matrix4.makeScale(rsmMesh.getScale().getX(), rsmMesh.getScale().getY(), rsmMesh.getScale().getZ()));
			
			return matrix1;
		}
		
		public Matrix4 getMatrix2()
		{
			if(matrix2 != null)
				return matrix2;

			matrix2 = new Matrix4();
			
			if(isRoot() && subMeshes.size() == 0)
				matrix2 = matrix2.mult(Matrix4.makeTranslation(-RsmRenderer.this.bbrange.getX(), -RsmRenderer.this.bbrange.getY(), -RsmRenderer.this.bbrange.getZ()));
			
			if(!isRoot() || subMeshes.size() != 0)
				matrix2 = matrix2.mult(Matrix4.makeTranslation(rsmMesh.getPosition().getX(), rsmMesh.getPosition().getY(), rsmMesh.getPosition().getZ()));
		
			
			matrix2 = matrix2.mult(rsmMesh.getMatrix());
				
			return matrix2;
		}
		
		
	}
	
	
}
