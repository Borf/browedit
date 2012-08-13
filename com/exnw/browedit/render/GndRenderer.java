package com.exnw.browedit.render;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.List;
import java.util.Observable;

import javax.media.opengl.GL;
import javax.media.opengl.GL4;

import com.exnw.browedit.data.Gnd;
import com.exnw.browedit.data.Gnd.GndCell;
import com.exnw.browedit.data.Gnd.Surface;
import com.exnw.browedit.math.Vector2;
import com.exnw.browedit.math.Vector3;
import com.exnw.browedit.renderutils.Shader;
import com.exnw.browedit.renderutils.Vbo;
import com.exnw.browedit.renderutils.VertexList;
import com.exnw.browedit.renderutils.vertexFormats.VertexPNCTT;
import com.jogamp.common.nio.Buffers;
import com.jogamp.opengl.util.texture.Texture;
import com.jogamp.opengl.util.texture.awt.AWTTextureIO;

public class GndRenderer implements Renderer
{
	Gnd gnd;

	ArrayList<Texture> textures = null;
	Texture shadows = null;
	Texture colorLightmap = null;
	ArrayList<Vbo<VertexPNCTT>> vbos = null;
	final static int TEXTURESIZE = 2048;

	public GndRenderer(Gnd gnd)
	{
		this.gnd = gnd;
	}

	public void render(GL4 gl, Shader shader)
	{
		if (textures == null)
			this.loadTextures(gl);
		if (vbos == null)
			this.generateVbos(shader);

		
		gl.glEnable(GL4.GL_CULL_FACE);
		
		gl.glActiveTexture(GL4.GL_TEXTURE2);
		colorLightmap.bind(gl);

		gl.glActiveTexture(GL4.GL_TEXTURE1);
		shadows.bind(gl);

		gl.glActiveTexture(GL4.GL_TEXTURE0);

		
		shader.getUniform("ground", gl).set(true, gl);
		
		for (int i = 0; i < textures.size(); i++)
		{
			gl.glActiveTexture(GL4.GL_TEXTURE0);
			textures.get(i).bind(gl);

			if(vbos.get(i) == null)
				continue;
			vbos.get(i).bind();
			gl.glDrawArrays(GL4.GL_TRIANGLES, 0, vbos.get(i).size()	/ Buffers.SIZEOF_FLOAT / 14); //TODO: replace with quads

		}
		shader.getUniform("ground", gl).set(false, gl);

		gl.glBindBuffer(GL.GL_ARRAY_BUFFER, 0);
		gl.glDisable(GL4.GL_CULL_FACE);

	}

	private Color mix(Color[] colors)
	{
		int r = 0, g = 0, b = 0, c = 0;
		for (Color color : colors)
		{
			if (color == null)
				continue;
			r += color.getRed();
			g += color.getGreen();
			b += color.getBlue();
			c++;
		}
		return new Color(r / c, g / c, b / c);
	}

	private Color getColor(int x, int y)
	{
		if (x < 0 || y < 0 || x >= gnd.getWidth() || y >= gnd.getHeight())
			return null;
		GndCell cell = gnd.getCell(x, y);
		int[] surfaces = cell.getSurface();
		if (surfaces[0] != -1)
		{
			return gnd.getSurfaces().get(surfaces[0]).getColor();
		}
		return null;
	}

	private void generateVbos(Shader shader)
	{
		vbos = new ArrayList<Vbo<VertexPNCTT>>();

		for (int i = 0; i < textures.size(); i++)
		{
			vbos.add(new Vbo<VertexPNCTT>());

			VertexList<VertexPNCTT> vertices = new VertexList<VertexPNCTT>();

			for (int x = 0; x < gnd.getWidth(); x++)
			{
				for (int y = 0; y < gnd.getHeight(); y++)
				{
					GndCell cell = gnd.getCell(x, y);
					int[] surfaces = cell.getSurface();
					if (surfaces[0] != -1)
					{
						Surface surface = gnd.getSurfaces().get(surfaces[0]);
						if (surface.getTextureID() == i)
						{
							int lightmap = surface.getLightmapID();

							float onePixel = (1.0f / (TEXTURESIZE / 8.0f)) / 8.0f;
							float tx1 = (lightmap % (TEXTURESIZE / 8))
									* (1.0f / (TEXTURESIZE / 8.0f));
							float ty1 = (lightmap / (TEXTURESIZE / 8))
									* (1.0f / (TEXTURESIZE / 8.0f));
							float tx2 = tx1 + 1.0f / (TEXTURESIZE / 8.0f);
							float ty2 = ty1 + 1.0f / (TEXTURESIZE / 8.0f);
							tx1 += onePixel;
							ty1 += onePixel;
							tx2 -= onePixel;
							ty2 -= onePixel;

							Color c1 = mix(new Color[] { getColor(x, y), getColor(x, y + 1), getColor(x - 1, y), getColor(x - 1, y + 1) });
							Color c2 = mix(new Color[] { getColor(x, y), getColor(x, y - 1), getColor(x - 1, y), getColor(x - 1, y - 1) });
							Color c3 = mix(new Color[] { getColor(x, y), getColor(x, y - 1), getColor(x + 1, y), getColor(x + 1, y - 1) });
							Color c4 = mix(new Color[] { getColor(x, y), getColor(x, y + 1), getColor(x + 1, y), getColor(x + 1, y + 1) });
							
							Vector3 tr = new Vector3(10.0f * x, -cell.getHeight()[2], 10.0f * (gnd.getHeight() - y) - 10);
							Vector3 tl = new Vector3(10.0f * x,-cell.getHeight()[0], 10.0f * (gnd.getHeight() - y));
							Vector3 bl = new Vector3(10.0f * x + 10, -cell.getHeight()[1],10.0f * (gnd.getHeight() - y));
							Vector3 br = new Vector3(10.0f * x + 10, -cell.getHeight()[3],10.0f * (gnd.getHeight() - y) - 10);
							
							Vector3 n1 = tl.sub(bl).cross(tl.sub(tr)).normalized();
							Vector3 n2 = br.sub(tr).cross(br.sub(bl)).normalized();						
							
							// tr
							vertices.add(new VertexPNCTT(
									tr,
									n1, 
									c1, 
									new Vector2(surface.getU()[2],surface.getV()[2]), 
									new Vector2(tx1, ty2)));
							// tl
							vertices.add(new VertexPNCTT(
									tl, 
									n1, 
									c2, 
									new Vector2(surface.getU()[0],surface.getV()[0]), new Vector2(tx1, ty1)));

							// bl
							vertices.add(new VertexPNCTT(
									bl,
									n1, 
									c3, 
									new Vector2(surface.getU()[1],surface.getV()[1]), 
									new Vector2(tx2, ty1)));

							// bl
							vertices.add(new VertexPNCTT(
									bl,
									n2, 
									c3, 
									new Vector2(surface.getU()[1],surface.getV()[1]), 
									new Vector2(tx2, ty1)));
							
							// br
							vertices.add(new VertexPNCTT(
									br,
									n2, 
									c4, 
									new Vector2(surface.getU()[3],surface.getV()[3]), 
									new Vector2(tx2, ty2)));
							// tr
							vertices.add(new VertexPNCTT(
									tr, 
									n2, 
									c1, 
									new Vector2(surface.getU()[2],surface.getV()[2]), new Vector2(tx1, ty2)));
						}
					}
					if (surfaces[2] != -1)
					{
						Surface surface = gnd.getSurfaces().get(surfaces[2]);
						if (surface.getTextureID() == i)
						{
							GndCell otherCell = gnd.getCell(x + 1, y);
							int lightmap = surface.getLightmapID();

							float onePixel = (1.0f / (TEXTURESIZE / 8.0f)) / 8.0f;
							float tx1 = (lightmap % (TEXTURESIZE / 8))
									* (1.0f / (TEXTURESIZE / 8.0f));
							float ty1 = (lightmap / (TEXTURESIZE / 8))
									* (1.0f / (TEXTURESIZE / 8.0f));
							float tx2 = tx1 + 1.0f / (TEXTURESIZE / 8.0f);
							float ty2 = ty1 + 1.0f / (TEXTURESIZE / 8.0f);
							tx1 += onePixel;
							ty1 += onePixel;
							tx2 -= onePixel;
							ty2 -= onePixel;
							
							vertices.add(new VertexPNCTT(
									new Vector3(10.0f * x + 10.0f,-otherCell.getHeight()[2],10.0f * (gnd.getHeight() - y) - 10),
									new Vector3(0, 1, 0), 
									surface.getColor(), 
									new Vector2(surface.getU()[3], surface.getV()[3]),
									new Vector2(tx1, ty2)));

							vertices.add(new VertexPNCTT(
									new Vector3(10.0f * x + 10.0f,-otherCell.getHeight()[0], 10.0f * (gnd.getHeight() - y)), 
									new Vector3(0,1, 0), 
									surface.getColor(), 
									new Vector2(surface.getU()[2], surface.getV()[2]),
									new Vector2(tx2, ty2)));

							vertices.add(new VertexPNCTT(
									new Vector3(10.0f * x + 10.0f, -cell.getHeight()[1],10.0f * (gnd.getHeight() - y)),
									new Vector3(0, 1, 0), 
									surface.getColor(), 
									new Vector2(surface.getU()[0], surface.getV()[0]),
									new Vector2(tx2, ty1)));
							
							vertices.add(new VertexPNCTT(
									new Vector3(10.0f * x + 10.0f, -cell.getHeight()[1],10.0f * (gnd.getHeight() - y)),
									new Vector3(0, 1, 0), 
									surface.getColor(), 
									new Vector2(surface.getU()[0], surface.getV()[0]),
									new Vector2(tx2, ty1)));
							
							vertices.add(new VertexPNCTT(
									new Vector3(10.0f * x + 10.0f,-cell.getHeight()[3],10.0f * (gnd.getHeight() - y) - 10),
									new Vector3(0, 1, 0), 
									surface.getColor(), 
									new Vector2(surface.getU()[1], surface.getV()[1]),
									new Vector2(tx1, ty1)));

							vertices.add(new VertexPNCTT(
									new Vector3(10.0f * x + 10.0f,-otherCell.getHeight()[2],10.0f * (gnd.getHeight() - y) - 10),
									new Vector3(0, 1, 0), 
									surface.getColor(), 
									new Vector2(surface.getU()[3], surface.getV()[3]),
									new Vector2(tx1, ty2)));
						}
					}
					if (surfaces[1] != -1) // front surfaces
					{
						Surface surface = gnd.getSurfaces().get(surfaces[1]);
						if (surface.getTextureID() == i)
						{
							GndCell otherCell = gnd.getCell(x, y + 1);
							int lightmap = surface.getLightmapID();

							float onePixel = (1.0f / (TEXTURESIZE / 8.0f)) / 8.0f;
							float tx1 = (lightmap % (TEXTURESIZE / 8)) * (1.0f / (TEXTURESIZE / 8.0f));
							float ty1 = (lightmap / (TEXTURESIZE / 8)) * (1.0f / (TEXTURESIZE / 8.0f));
							float tx2 = tx1 + 1.0f / (TEXTURESIZE / 8.0f);
							float ty2 = ty1 + 1.0f / (TEXTURESIZE / 8.0f);
							tx1 += onePixel;
							ty1 += onePixel;
							tx2 -= onePixel;
							ty2 -= onePixel;
							
							vertices.add(new VertexPNCTT(
									new Vector3(10.0f * x,-otherCell.getHeight()[0],10.0f * (gnd.getHeight() - y) - 10),
									new Vector3(0, 1, 0), 
									surface.getColor(), 
									new Vector2(surface.getU()[2], surface.getV()[2]),
									new Vector2(tx1, ty2)));

							vertices.add(new VertexPNCTT(
									new Vector3(10.0f * x,-cell.getHeight()[2], 10.0f * (gnd.getHeight() - y) - 10),
									new Vector3(0, 1, 0), 
									surface.getColor(), new Vector2(surface.getU()[0], surface.getV()[0]),
									new Vector2(tx1, ty1)));

							vertices.add(new VertexPNCTT(
									new Vector3(10.0f * x + 10,-cell.getHeight()[3], 10.0f * (gnd.getHeight() - y) - 10),
									new Vector3(0, 1, 0), 
									surface.getColor(), new Vector2(surface.getU()[1], surface.getV()[1]),
									new Vector2(tx2, ty1)));

							vertices.add(new VertexPNCTT(
									new Vector3(10.0f * x + 10,-cell.getHeight()[3], 10.0f * (gnd.getHeight() - y) - 10),
									new Vector3(0, 1, 0), 
									surface.getColor(), 
									new Vector2(surface.getU()[1],surface.getV()[1]),
									new Vector2(tx2, ty1)));

							vertices.add(new VertexPNCTT(
									new Vector3(10.0f * x + 10,-otherCell.getHeight()[1],10.0f * (gnd.getHeight() - y) - 10),
									new Vector3(0, 1, 0), 
									surface.getColor(), 
									new Vector2(surface.getU()[3], surface.getV()[3]),
									new Vector2(tx2, ty2)));
							vertices.add(new VertexPNCTT(
									new Vector3(10.0f * x,-otherCell.getHeight()[0],10.0f * (gnd.getHeight() - y) - 10),
									new Vector3(0, 1, 0), 
									surface.getColor(), 
									new Vector2(surface.getU()[2], surface.getV()[2]),
									new Vector2(tx1, ty2)));
						}
					}
				}
			}
			if(vertices.size() != 0)
				vbos.get(i).generate(vertices, shader);
			else
				vbos.set(i, null);
		}

	}

	private void loadTextures(GL4 gl)
	{
		{
			textures = new ArrayList<Texture>();
			List<String> TextureFileNames = gnd.getTextures();
			for (String s : TextureFileNames)
			{
				Texture texture = TextureCache.getTexture(gl, "data\\texture\\" + s);
				// TODO: fix borders
				texture.setTexParameteri(gl, GL.GL_TEXTURE_WRAP_S, GL.GL_MIRRORED_REPEAT);
				texture.setTexParameteri(gl, GL.GL_TEXTURE_WRAP_T, GL.GL_MIRRORED_REPEAT);

				textures.add(texture);
			}

			{
				BufferedImage image = new BufferedImage(TEXTURESIZE,
						TEXTURESIZE, BufferedImage.TYPE_INT_ARGB);
				int x = 0;
				int y = 0;
				for (Gnd.Lightmap lightmap : gnd.getLightmaps())
				{
					byte[][] brightness = lightmap.getBrightness();
					for (int xx = x; xx < x + 8; xx++)
					{
						for (int yy = y; yy < y + 8; yy++)
						{
							int c = 0xff000000;
							c |= brightness[yy - y][xx - x] & 0xff;
							c |= (brightness[yy - y][xx - x] & 0xff) << 8;
							c |= (brightness[yy - y][xx - x] & 0xff) << 16;

							image.setRGB(xx, yy, c);

						}
					}
					x += 8;
					if (x >= TEXTURESIZE)
					{
						x = 0;
						y += 8;
					}
				}
				
				shadows = AWTTextureIO.newTexture(gl.getGLProfile(), image, false);

				shadows.setTexParameteri(gl, GL.GL_TEXTURE_MAG_FILTER, GL.GL_LINEAR);
				shadows.setTexParameteri(gl, GL.GL_TEXTURE_MIN_FILTER, GL.GL_LINEAR);
			}
			{
				BufferedImage image = new BufferedImage(TEXTURESIZE,
						TEXTURESIZE, BufferedImage.TYPE_INT_ARGB);
				int x = 0;
				int y = 0;
				for (Gnd.Lightmap lightmap : gnd.getLightmaps())
				{
					Color[][] colors = lightmap.getColor();
					for (int xx = x; xx < x + 8; xx++)
					{
						for (int yy = y; yy < y + 8; yy++)
						{
							image.setRGB(xx, yy, colors[yy - y][xx - x]
									.getRGB() | 0xff000000);
						}
					}
					x += 8;
					if (x >= TEXTURESIZE)
					{
						x = 0;
						y += 8;
					}
				}
				colorLightmap = AWTTextureIO.newTexture(gl.getGLProfile(), image, false);
				colorLightmap.setTexParameteri(gl, GL.GL_TEXTURE_MAG_FILTER, GL.GL_LINEAR);
				colorLightmap.setTexParameteri(gl, GL.GL_TEXTURE_MIN_FILTER, GL.GL_LINEAR);
			}

		}
	}

	public void update(Observable arg0, Object arg1)
	{

	}

	public void destroy(GL4 gl)
	{
		for(Vbo<VertexPNCTT> vbo : vbos)
			vbo.destroy(gl);
		shadows.destroy(gl);
		colorLightmap.destroy(gl);
		textures.clear();
	}

}
