     ____                    ______    _ _ _
    |  _ \                  |  ____|  | (_) |
    | |_) |_ __ _____      _| |__   __| |_| |_
    |  _ <| '__/ _ \ \ /\ / /  __| / _` | | __|
    | |_) | | | (_) \ V  V /| |___| (_| | | |_
    |____/|_|  \___/ \_/\_/ |______\__,_|_|\__| 2.0




Browedit is used to edit maps for Ragnarok Online. This is a superbrief readme



Global Keys:

Space			brings up the radial menu
O			shows/hides objects
S			shows/hides shadows
G			shows/hides grids


Editmodes:

Texture Edit (F1)
-----------------
Used to edit textures. 
First select a texture in the texturewindow by dragging the area you want to use
After selecting a texture (or part of it), you can click in the map to paste it somewhere
then there's the following things:
Numpad +		Increase Texture Size
Numpad -		Decrease Texture Size
H			Flip texture horizontally
V			Flip texture vertically
R			Rotate texture

In the texture window is a little > button. Click it to expand this window. This will open the texture browser. 
Click on the list of categories to browse through categories. There is no option to add these textures to the map yet



Object edit (F2)
----------------
In object edit mode you can edit models, sounds, effects and lights
Click on an object to select it, drag to select multiple, doubleclick on an object to view its properties
This editmode has different tools

Move (1)
Used to move selected objects. Drag to move the object. Shift+drag to move the object up and down

Scale (2)
Used to move scale objects. Drag to scale the object. 

Rotate (3)
Used to move rotate objects. Drag to rotate the object. 



Height Edit (F3)
----------------
Leftdrag			select an area
ctrl+leftdrag		select an area in a lassoshape
rightdrag			move the selected area up and down
C					connects tiles together
R					adds a random value to the height of tiles
F					flattens an area

menu:
use the heighttools/connect to connect the raised area to the surrounding area


Detailed height Edit (F4)
-------------------------
Gives you really precise control over vertices
with the heighttools/connect menu to select a single vertex or the 4 vertices on the corner of a quad

leftdrag			moves currently highlighted vertex up or down


Wall edit (F5)
--------------
rightdrag			add a wall
ctrl+rightdrag		remove a wall

leftclick			paste selected texture on walls
left/right arrow	scale selected texture output
H					flip texture horizontally
V					flip texture vertically



Gat Edit ()
-------------
not implemented yet





=============================================================

Plugin Interface
----------------

browedit :
{
	version,	//contains browedit version
	'map' :
	{
		save(),																			//TODO
		load(),																			//TODO
		'gnd' : // API related to the floor height, textures & shadows
		{
			getWidth(),		/returns the width of the map
			getHeight(),	//returns the height of the map
		},
		'rsw' :	// API related to objects
		{
			
		}
	},
	'camera' : //camera controls
	{
	
	}
	'wm' : 																			//TODO
	{
		showMessage(text),															//TODO
	}
}