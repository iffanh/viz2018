#include "dgl.h"
#include <stdlib.h>
#include <math.h>

// TPG4162 SAMPLE CODE INSPIRED BY EXERCISE 3 TO ILLUSTRATE THE USE OF TEXTURE FOR COLOR MAPPING OF A RESULT DISTRIBUTION

void discreteColorMapTexture();

/***************************************************************************************************************
 * Macro for linear conversion from temperature t ∈ [0, 100] to texture coordinate s ∈ [0.0, 1.0].
 * Color map formula of curriculum gives an integer index ∈ [1, 4] while we are interested in s ∈ [0.0, 1.0]
 * => divide formula by 'n':  s = (n / n) * (Si - min) / (max - min) = 1.0 (Si - 0) / (100 - 0) = Si / 100
 * Verification of legend levels: Si = {10, 50, 100} ⇔ s = {0.1, 0.5, 1.0}
 ***************************************************************************************************************/
#define TEMP_TO_TEXCOORD(Si) (Si/100.0)

// Scalar Texturing: First see discreteColorMapTexture() below for the scalar result texture. 
void main()
{
	dglWindowOpen("Textured Color Mapping of Results", 900, 750, false);    // create a GL-enabled window of given pixel size

	discreteColorMapTexture();		// set up the one-dimensional texture for the color legend of the result distribution

	glEnableClientState(GL_VERTEX_ARRAY);

	static float vertexData[] = 
	{ //  X      Y     Z     TEMP conversion to texcoord
		-0.8f, -0.8f, 0.0f, TEMP_TO_TEXCOORD(10),
		 0.0f, -0.8f, 0.0f, TEMP_TO_TEXCOORD(50),
		 0.0f,  0.8f, 0.0f, TEMP_TO_TEXCOORD(100),
		-0.8f,  0.8f, 0.0f, TEMP_TO_TEXCOORD(50),
		 0.8f, -0.8f, 0.0f, TEMP_TO_TEXCOORD(10),
		 0.8f,  0.8f, 0.0f, TEMP_TO_TEXCOORD(50)
	};

	glVertexPointer  (3, GL_FLOAT, 4 * sizeof(float), vertexData);
	glTexCoordPointer(1, GL_FLOAT, 4 * sizeof(float), &vertexData[3]);

	static unsigned int indices[] = 
	{
		0, 1, 2, 3,		// first  quad
		1, 4, 5, 2,		// second quad
	};


	while( !dglGetAsyncKeyState(DGLK_ESCAPE) )		// keep on till user presses ESC
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_QUADS, 4 * 2, GL_UNSIGNED_INT, indices);
		dglSwapBuffers();	
	}

	dglWindowClose();
}





/***************************************************************************************************************
 * Set up the discrete, one-dimensional texture for the color legend of the result distribution spanning four 
 * levels by colors blue, green, yellow, and red. Red signals the highest temperatures, c.f. ranges below.
 * The color legend ranges from low value to high value, i.e. texture coordinate 1.0 represents temperature 100
 ***************************************************************************************************************/
void discreteColorMapTexture()
{
	static float fColorMapTexture[] = 
	{  
		// scalar texture just 4 texels => texture coord lookups listed
		0.0,   0.0,   1.0,		// blue   <=> tex.coord [0.00, 0.25> <=> temp [ 0, 25>
		0.0,   1.0,   0.0,		// green  <=> tex.coord [0.25, 0.50> <=> temp [25, 50>
		1.0,   1.0,   0.0,		// yellow <=> tex.coord [0.50, 0.75> <=> temp [50, 75>
		1.0,   0.0,   0.0		// red    <=> tex.coord [0.75, 1.00] <=> temp [75, 100]
	};

	GLuint colorMapTextureID;								
	glGenTextures(1, &colorMapTextureID);
	glBindTexture(GL_TEXTURE_1D, colorMapTextureID);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 4,  0, GL_RGB, GL_FLOAT, fColorMapTexture);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);

	glEnable(GL_TEXTURE_1D);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}










