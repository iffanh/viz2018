/*
* TPG4162 Project 1 framework
* Author: Einar Baumann
*
* Based on a Lighthouse3D tutorial:
*  http://www.lighthouse3d.com/tutorials/glut-tutorial/the-code-so-far-v/
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
using namespace std;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GL/glut.h"
#endif

#include <iostream>
using namespace std;


/*
*|------------------------------------------------------|
*|                   INSTRUCTIONS                       |
*|======================================================|
*| The functions you will need to edit are as follows:  |
*|                                                      |
*|   * readData() - This function should read the data  |
*|        from the SEGY file and store them in the      |
*|        'data' array.                                 |
*|                                                      |
*|   * createRGBColorMapping() - This function should   |
*|        convert the values in the 'data' array into   |
*|        RGB values (3 color values pr. data point)    |
*|        and store them in the 'RGBArray' matrix.      |
*|                                                      |
*|   * buildTextures() - Create textures from the RGB   |
*|        data. This is also where you apply filters    |
*|        and create mipmaps.                           |
*|                                                      |
*|   * drawTiles() - Draw the tiles with textures       |
*|        applied on the screen. You will also need to  |
*|        consider panning and zooming in this function.|
*|                                                      |
*|   * processNormalKeys() and pressKey() - This is     |
*|        where you should manipulate the global x,y,z  |
*|        variables to achieve panning and zooming      |
*|        effects.                                      |
*|------------------------------------------------------|
*/

const int TRACES = 13483;
const int SAMPLES = 1750;
const int TEX_WIDTH = 1024;//2048;// 16384; //1024;
const int TEX_HEIGHT = 1024;//2048; //1024;
const int TEX_COLS = ceil((TRACES/(double)TEX_WIDTH));
const int TEX_ROWS = ceil((SAMPLES/(double)TEX_HEIGHT));
const int NUMBER_OF_TEXTURES =  TEX_COLS * TEX_ROWS;

char data[TRACES][SAMPLES]; // Stores data read from file
static GLubyte RGBArray[28][TEX_WIDTH*TEX_HEIGHT*3]; // Holds RGB values for textures. Each row holds the values for one texture.
GLuint textures[28]; // Holds handles to textures

// Used when panning and zooming
float x = 0.0;
float y = 0.0;
float z = 0.0;

float inc = 0.1;

/*
*|------------------------------------------------------|
*| Read data from the SEGY file and store it in the     |
*| 'data' matrix.                                       |
*|------------------------------------------------------|
*/


void readData(){
    FILE* pFile = fopen("NVGT-88-06.sgy", "rb");   // fopen() fills in the FILE structure and returns a pointer to the data
    fseek(pFile, 3600, SEEK_SET);           // skip file header
    
    for (int i = 0; i < 13483; i++)
    {
        fseek(pFile, 240, SEEK_CUR);        // loop over number of traces
                                            // skip 240B trace header
        for (int j = 0; j < 1750; j++)      // loop through the 1750 samples
        {
            fread(&data[i][j], 1, 1, pFile); // read each sample (1 byte each)
            //cout << matrix[i][j];
        }
    }
    fclose(pFile);
}

/*
*|------------------------------------------------------|
*| Convert the data in the 'data' array into RGB values |
*| and store them in the 'RGBArray' matrix.             |
*|======================================================|
*| Currently this function maps all positive elements   |
*| to white and negative elements to black. Change it   |
*| to use a color scale, taking into consideration the  |
*| magnitude of the values in addition to the sign.     |
*|------------------------------------------------------|
*/
void CreateRGBColorMapping() {
    char element;         // Temporarily holds 'active' data element
    int textureNr = 0;    // Texture counter
    int pixelCounter;     // Pixel counter (0 to TEX_WIDTH*TEX_HEIGH)

    for(int row=0; row<TEX_ROWS; row++) // Loop over texture rows
    {
        for(int col=0; col<TEX_COLS; col++) // Loop over texture columns
        {
            pixelCounter = 0; // Set to zero at beginning of each new texture tile
            for(int i=col*TEX_WIDTH; i<TEX_WIDTH*(col+1); i++)    // i = Trace counter for the data
            {
                for(int j=row*TEX_HEIGHT; j<TEX_HEIGHT*(row+1); j++)  // j = Sample counter for the data
                {
                    if( (i>(TRACES-1)) || (j>(SAMPLES-1)) )   // Setting color to black if outside data range
                    {
                        RGBArray[textureNr][3*pixelCounter]   = 0;
                        RGBArray[textureNr][3*pixelCounter+1] = 0;
                        RGBArray[textureNr][3*pixelCounter+2] = 0;
                    }
                    else
                    {
                        
                        element = data[i][j];
                        // //The Red-White-Blue color scheme
                        // if( element >= 64)         
                        // {
                        //     RGBArray[textureNr][3*pixelCounter]   = 255;
                        //     RGBArray[textureNr][3*pixelCounter+1] = 0;
                        //     RGBArray[textureNr][3*pixelCounter+2] = 0;
                        // }
                        // else if( element >= -16 )    
                        // {
                        //     RGBArray[textureNr][3*pixelCounter]   = 255;
                        //     RGBArray[textureNr][3*pixelCounter+1] = 255;
                        //     RGBArray[textureNr][3*pixelCounter+2] = 255;
                        // }
                        // else if( element < -16 )   
                        // {
                        //     RGBArray[textureNr][3*pixelCounter]   = 0;
                        //     RGBArray[textureNr][3*pixelCounter+1] = 0;
                        //     RGBArray[textureNr][3*pixelCounter+2] = 255;
                        // }

                        // White-Grey-Black color scheme
                        if( element >= 32)         
                        {
                            RGBArray[textureNr][3*pixelCounter]   = 255;
                            RGBArray[textureNr][3*pixelCounter+1] = 255;
                            RGBArray[textureNr][3*pixelCounter+2] = 255;
                        }
                        else if( element >= -32 )    
                        {
                            RGBArray[textureNr][3*pixelCounter]   = 192;
                            RGBArray[textureNr][3*pixelCounter+1] = 192;
                            RGBArray[textureNr][3*pixelCounter+2] = 192;
                        }
                        else if( element < -32 )   
                        {
                            RGBArray[textureNr][3*pixelCounter]   = 0;
                            RGBArray[textureNr][3*pixelCounter+1] = 0;
                            RGBArray[textureNr][3*pixelCounter+2] = 0;
                        }
                    }
                    pixelCounter++; // Pixel color has been set; incrementing to next pixel
                }
            }
            textureNr +=1;  // All pixels have been set for texture; incrementing to next
        }
    }
}

/*
*|------------------------------------------------------|
*| Create textures from the values in the 'RGBData'     |
*| matrix.                                              |
*|======================================================|
*| Things you will need to change:                      |
*|   * Experiment with different filters.               |
*|                                                      |
*|   * Try replacing glTexImage2D with a call to        |
*|     to generate textures with different miplevels.   |
*|------------------------------------------------------|
*/
void buildTextures() {
    glGenTextures(NUMBER_OF_TEXTURES, textures); // Generate texture handles
    for (int i=0; i<NUMBER_OF_TEXTURES; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, textures[i]);

        // Filters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // MODIFY THIS: Try other filters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // MODIFY THIS: Try other filters

        glTexImage2D(
                  GL_TEXTURE_2D,            // target texture
                  0,                        // level of detail. 0 is base image level; level n is the n'th mipmap reduction
                  GL_RGB,                   // internal format; i.e. number of color components in texture
                  TEX_HEIGHT, TEX_WIDTH,    // height, width of texture image
                  0,                        // border (MUST be 0)
                  GL_RGB,                   // format of pixel data.
                  GL_UNSIGNED_BYTE,         // data type of the pixel data
                  &RGBArray[i]              // data source
        );
    }

}

/*
*|------------------------------------------------------|
*| Draw tiles with the textures applied on the screen.  |
*|======================================================|
*| Things you will need to change:                      |
*|   * glTranslatef should move each tile so that they  |
*|     are all next to eachother but dont overlap. This |
*|     function call should also take into consideration|
*|     the values of the global x,y,z variables to      |
*|     to handle zooming and panning.                   |
*|
*|   * The index into textures (which is currently 0)   |
*|     in the glBindTexture() function should be changed|
*|     to bind the correct texture for the row and      |
*|     column currently being handled by the loops.     |
*|------------------------------------------------------|
*/
void drawTiles() {
    int count = 0;
    for (int row=0; row<TEX_ROWS; row++)
    {
        for (int col=0; col<TEX_COLS; col++)
        {
            const float vertexCoords[] = {
                -1.0f, -1.0f, 0.0f,
                 1.0f, -1.0f, 0.0f,
                 1.0f,  1.0f, 0.0f,
                -1.0f,  1.0f, 0.0f,
            };

            const float textureCoords[] = {
                0.0f, 0.0f,
                1.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 1.0f
            };

            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, vertexCoords);
            glTexCoordPointer(2, GL_FLOAT, 0, textureCoords);

            glLoadIdentity();
            glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
            glTranslatef(x + -(TEX_ROWS - 1) + row*2, y + -(TEX_COLS - 1) + col*2, z - 10.0f);  // MODIFY THIS
            glBindTexture(GL_TEXTURE_2D, textures[count]); // MODIFY THE INDEX INTO textures
            glDrawArrays(GL_QUADS, 0, 4);
            //}
            count += 1;
        }
    }
}

/*
*|------------------------------------------------------|
*| The following two functions handle keypresses.       |
*| Pressing 'i' should zoom in; pressing 'o' should     |
*| zoom out; and pressing arrow up/down/left/right      |
*| should pan (move) up/down/left/right.                |
*| Modify the global x,y,z variables to achieve this.   |
*|------------------------------------------------------|
*/
void processNormalKeys(unsigned char key, int xx, int yy) {
	switch (key) {
		case 105:
            z += inc;
            printf("i pressed.\n");
			break;
        case 111:
            z -= inc;
            printf("o pressed.\n");
			break;
	}
}

void pressKey(int key, int xx, int yy) {

	switch (key) {
		case GLUT_KEY_UP:
            x += inc;
            printf("Up pressed.\n");
            break;
		case GLUT_KEY_DOWN:
            x -= inc;
            printf("Down pressed.\n");
            break;
        case GLUT_KEY_LEFT:
            y += inc;
            printf("Left pressed.\n");
            break;
        case GLUT_KEY_RIGHT:
            y -= inc;
            printf("Right pressed.\n");
            break;
	}
}


/*
*|----------------------------------------------------------|
*| Called by glut to display/redisplay stuff in the window. |
*| It is registered as a callback in the main function.     |
*|----------------------------------------------------------|
*/
void renderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Color and Depth Buffers
	glLoadIdentity(); // Reset transformations

    glMatrixMode(GL_MODELVIEW);  // get back to modelview mode
    gluLookAt(	0.0, 0.0, 10.0,   // Eye position (x, y, z)
			    0.0, -10.0, 0.0,   // Look at position (x, y, z)
			    0.0, 1.0, 0.0);  // Up-vector (x, y, z)

    drawTiles();                 // Draw the wanted shape
	//glMatrixMode(GL_MODELVIEW);  // get back to modelview mode
	glutSwapBuffers();           // Display our matrix (i.e. the figure)
}


/*
*|----------------------------------------------------------|
*| Called by glut to whenever the window is resized         |
*| It is registered as a callback in the main function.     |
*|----------------------------------------------------------|
*/
void changeSize(int w, int h) {

	float ratio =  w * 1.0 / h;

	glMatrixMode(GL_PROJECTION); // Use the Projection Matrix
	glLoadIdentity(); // Reset Matrix
	glViewport(0, 0, w, h); // Set the viewport to be the entire window
	gluPerspective(60.0f, ratio, 0.1f, 100.0f); // Set the correct perspective.
	glMatrixMode(GL_MODELVIEW); // Get Back to the Modelview
}


/*
*|---------------|
*| Main function |
*|---------------|
*/
int main(int argc, char **argv) {

	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1600,640);
	glutCreateWindow("Project 1");

	// register glut callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

	// Register keyboard functions
    glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressKey);

	// OpenGL initialization
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    readData();
    CreateRGBColorMapping();
	buildTextures();

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}
