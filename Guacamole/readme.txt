Name: 	Christian Galante
ID:	40006461
E-MAIL: chrisgalante22@gmail.com
------------------------------------
COMP371 - Assignment # 2
------------------------------------

The program is executed by running "engine.exe" in the same directory as this file. The file structure must stay the way it is for the program to run properly.

New Features:
- Olaf's body is now made from spheres! (And he doesn't look like a penguin anymore!)
- A point light source was added to the scene which has ambient, diffuse and specular lighting.
- A snow texture was tiled on the ground plane.
- A metallic texture was added to Olaf's hat.
- A carrot texture was added to Olaf's nose.
- Added animations to Olaf's arms and feet when moving forwards or backwards. When he is moving backwards it looks like he's doing the moonwalk.

Controls:
- Press Q and E to rotate Olaf
- Use W, A, S, D to move Olaf Forward, Left, Backward, Right respectively.
- Use <- and -> to rotate the camera about the y-axis at the ceneter of the scene
- Use left mouse button to zoom in and out
- Use right mouse button to pan right and left
- Use middle mouse button to look up and down
- Press X to toggle textures.
- Press T to render Olaf as triangles
- Press L to render Olaf as lines
- Press P to render Olaf as points
- Press SPACE to re-position Olaf randomly in the scene
- Press HOME to re-orientate the camera back to its original position (Olaf stays where he is)

Missing Shadow Maps:
---------------------
After countless hours of trying to figure it out, the shadow maps were not working. I believe the problem had something to do with the FBO not producing a useable shadow map texture. The code is still in the source files, however I've overidden the shadow map when doing light calculations in the default shader. Note that the debug shaders were used to debug and try to visualise the depth buffer, these shaders were constantly being changed but always resulted in a solid colour when sampling from the shadow map.

Things I tried:
- Using different texture parameters for the shadow mapwhen initializing the FBO. (GL_NEAREST vs GL_LINEAR etc.)
- Adjusting the projection of the light source such that the near plane was as far forward and the far plane was as pulled in as possible.
- Linearizing depth values after converting the fragment positions from the light source into normalized device coordinates.

I read many forum posts of people having the same problem, and all of them forgot to clear the depth buffer before drawing their scene (which was the first thing I checked).

If you have any questions, please contact me at chrisgalante22@gmail.com.

