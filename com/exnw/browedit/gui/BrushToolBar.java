package com.exnw.browedit.gui;

import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSlider;
import javax.swing.JToolBar;
import javax.swing.ListCellRenderer;
import javax.swing.UIManager;
import javax.swing.border.Border;
import javax.swing.border.EmptyBorder;

import com.exnw.browedit.data.Map;
import com.exnw.browedit.grflib.GrfLib;

public class BrushToolBar extends JToolBar
{
	/**
	 * 
	 */
	private static final long serialVersionUID = 6838263874597190071L;
	private JSlider sizeSlider;
	private JList<JCheckBox> layers;
	private JCheckBox layersEnabled[];
	
	private JLabel currentTextureLabel;
	
	
	public BrushToolBar(MainFrame mainFrame)
	{
		super(JToolBar.VERTICAL);
		setMaximumSize(new Dimension(260, 20000));
		setSize(new Dimension(260, 20000));
		setPreferredSize(new Dimension(260, 20000));
		
		JPanel brush = new JPanel();
		brush.setLayout(new BoxLayout(brush, BoxLayout.PAGE_AXIS));
		add(brush);
		
		//add(new JButton("Select"));
		brush.add(new JLabel("Brush Size"));
		brush.add(sizeSlider = new JSlider(0,256, 8));
		
		textures = new JPanel();
		textures.setLayout(new GridLayout(0, 3));
		JScrollPane scrollPane = new JScrollPane(textures);
		brush.add(scrollPane);
		
		brush.add(new JButton("Select Subbrush"));
		

		
		JPanel layer = new JPanel();
		layer.setLayout(new BoxLayout(layer, BoxLayout.PAGE_AXIS));
		add(layer);
		
		layersEnabled = new JCheckBox[]
		{ 
				new JCheckBox("Ground",true), 
				new JCheckBox("Collision",true), 
				new JCheckBox("Objects",true), 
				new JCheckBox("Effects",true), 
				new JCheckBox("Lights",true), 
				new JCheckBox("Sounds", true)
		};
		
		layers = new JList<JCheckBox>(layersEnabled);
		layers.addMouseListener(new MouseAdapter()
		{
			public void mousePressed(MouseEvent e)
			{
				int index = layers.locationToIndex(e.getPoint());
				if (index != -1)
				{
					JCheckBox checkbox = layers.getModel().getElementAt(index);
					if(e.getPoint().x <= 20)
					{
						checkbox.setSelected(!checkbox.isSelected());
						repaint();
					}
				}
		
			}
		});	
		layers.setCellRenderer(new LayerCellRenderer());
		
		layer.add(layers);
	}
	
	class LayerListItem
	{
		public String name;
		public JCheckBox checkBox;
		public LayerListItem(String name, JCheckBox checkBox)
		{
			super();
			this.name = name;
			this.checkBox = checkBox;
		}
	}
	
	protected static Border noFocusBorder = new EmptyBorder(1, 1, 1, 1);
	private JPanel textures;
	protected class LayerCellRenderer implements ListCellRenderer<JCheckBox>
	{
		public Component getListCellRendererComponent(JList<? extends JCheckBox> list, JCheckBox value,
				int index, boolean isSelected, boolean cellHasFocus)
		{
			JCheckBox checkbox = (JCheckBox) value;
			checkbox.setBackground(isSelected ? list.getSelectionBackground() : getBackground()); 
			checkbox.setForeground(isSelected ? list.getSelectionForeground() : getForeground());
			checkbox.setEnabled(list.isEnabled());
			checkbox.setFont(list.getFont());
			checkbox.setFocusPainted(false);
			checkbox.setBorderPainted(true);
			checkbox.setBorder(isSelected ? UIManager.getBorder("List.focusCellHighlightBorder") : noFocusBorder);
			return checkbox;
		}
		
	}
	
	
	
	
	
	
	public void setMap(Map currentMap)
	{
		textures.removeAll();
		for(String texture : currentMap.getGnd().getTextures())
		{
			try
			{
				int size = 76;
				ImageIcon imageIcon = new ImageIcon(ImageIO.read(GrfLib.openFile("data\\texture\\" + texture)).getScaledInstance(size, size, 0));
				final JLabel label = new JLabel(imageIcon);
				label.setBorder(BorderFactory.createEmptyBorder(1,1,1,1));
				label.addMouseListener(new MouseAdapter()
				{
					public void mouseClicked(MouseEvent e)
					{
						super.mouseClicked(e);
						if(currentTextureLabel != null)
							currentTextureLabel.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 0));
						currentTextureLabel = label;
						label.setBorder(BorderFactory.createLineBorder(Color.RED, 1));
					}
				});
				textures.add(label);
			} catch (IOException e)
			{
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
	}
}
