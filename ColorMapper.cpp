#include "ColorMapper.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#elif defined(_WIN32)
#include <GL\glut.h>
#include <GL\GLU.h>
#include <GL\GL.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctime>

#include "segy.h"

ColorMapper::ColorMapper(segy *segArg){
    zoomLevel = 256.0f;
    xPos = -500.0f;
    yPos = 100.0f;
    improvedLegend = true;
    
    // Default legend
    legendBottom = GREEN;
    legendTop = RED;
    legendResolution = 8;
    
    seg = segArg;
    
    seismicChunks = new seisChunkWrapper(); // Allocate matrix for pixel data
    
    fillLegend2(); // Generate legend with improved legend generator
    makeSeismicChunks(); // Fill in pixel data with generated legend
    
    init();
    
    delete seismicChunks; // Deallocate matrix for pixel data
}

// Generate matrices for the textures
void ColorMapper::makeSeismicChunks(void){
	char** dataPoints = seg->getDataPoints(); // Get a 2D matrix of the data points from the segy file
    
    // Loop through all data points, making jumps of 1024 to fill in the
    // 1024x1024 textures
	for (int i = 0; i < seisWidth; i += 1024) for (int x = i; x < i + 1024; ++x){
		for (int j = 0; j < seisHeight; j += 1024) for (int y = j; y < j + 1024; y++){
			if (x < seisWidth && y < seisHeight){
				int value = 127 - (int)dataPoints[x][y];
				
                // Filling in values from the previously generated dictionary
                seismicChunks->seismicChunks[i/1024][j/1024][y-j][x-i][0] = dict[value][0];
                seismicChunks->seismicChunks[i/1024][j/1024][y-j][x-i][1] = dict[value][1];
                seismicChunks->seismicChunks[i/1024][j/1024][y-j][x-i][2] = dict[value][2];
				
				seismicChunks->seismicChunks[i/1024][j/1024][y-j][x-i][3] = (GLubyte) 255;
			}
			else {
                // Out of bounds, filling in black
				seismicChunks->seismicChunks[i/1024][j/1024][y-j][x-i][0] = 0;
				seismicChunks->seismicChunks[i/1024][j/1024][y-j][x-i][1] = 0;
				seismicChunks->seismicChunks[i/1024][j/1024][y-j][x-i][2] = 0;
				seismicChunks->seismicChunks[i/1024][j/1024][y-j][x-i][3] = 255;
			}
		}
	}
}

// Fill a dictionary with the color values to be used to map to the data point values.
// This is the old legend generator which crossfades two colors simultaneously, see
// fillLegend2 for the improved version
void ColorMapper::fillLegend(){
	int legendSteps = 256 / legendResolution; // Range of each legend part
    int colorSteps = legendSteps - 1; // Color difference for each step
    
    for (int value = 0; value < 256; ++value){
        int red, green, blue;
        if (legendBottom == RED){
            for (int i = 1; i < legendResolution + 1; ++i){
                if (value <= i * legendSteps){
                    red = (i - 1) * colorSteps;
                    break;
                }
            }
            if (red > 255) red = 255;
            
            if (legendTop == GREEN){
                green = 255 - red;
                blue = 0;
            }
            else if (legendTop == BLUE){
                blue = 255 - red;
                green = 0;
            }
        }
        else if (legendBottom == GREEN){
            for (int i = 1; i < legendResolution + 1; ++i){
                if (value <= i * legendSteps){
                    green = (i - 1) * colorSteps;
                    break;
                }
            }
            if (green > 255) green = 255;
            
            if (legendTop == RED){
                red = 255 - green;
                blue = 0;
            }
            else if (legendTop == BLUE){
                blue = 255 - green;
                red = 0;
            }
        }
        else if (legendBottom == BLUE){
            for (int i = 1; i < legendResolution + 1; ++i){
                if (value <= i * legendSteps){
                    blue = (i - 1) * colorSteps;
                    break;
                }
            }
            if (blue > 255) blue = 255;
            
            if (legendTop == GREEN){
                green = 255 - blue;
                red = 0;
            }
            else if (legendTop == RED){
                red = 255 - blue;
                green = 0;
            }
        }
        
        dict[value][0] = red;
        dict[value][1] = green;
        dict[value][2] = blue;
    }
}

// Fill a dictionary with the color values to be used to map to the data point values.
// This legend generator crossfades the two colors, but makes sure that the middle color
// is a mix of the two extremes.
void ColorMapper::fillLegend2(){
	int legendSteps = 256 / legendResolution;
    
    for (int value = 0; value < 256; ++value){
        int cut = value - 128; // Used to calculate the incline of the top color of the legend
        int flip = 128 - value;// Used to calculate the decline of the bottom color of the legend
        int red = 0;
        int green = 0;
        int blue = 0;
        if (legendTop == RED){
            if (value < 128){
                red = 255;
            }
            else {
                red = 255 - (cut + legendSteps - (cut % legendSteps)) * 2;
                if (red < 0) red = 0;
            }
        }
        else if (legendTop == GREEN){
            if (value < 128){
                green = 255;
            }
            else {
                green = 255 - (cut + legendSteps - (cut % legendSteps)) * 2;
                if (green < 0) green = 0;
            }
        }
        else if (legendTop == BLUE){
            if (value < 128){
                blue = 255;
            }
            else {
                blue = 255 - (cut + legendSteps - (cut % legendSteps)) * 2;
                if (blue < 0) blue = 0;
            }
        }
        
        if (legendBottom == RED){
            if (value > 128){
                red = 255;
            }
            else {
                red = 255 - (flip + legendSteps - (flip % legendSteps)) * 2;
                if (red < 0) red = 0;
            }
        }
        else if (legendBottom == GREEN){
            if (value > 128){
                green = 255;
            }
            else {
                green = 255 - (flip + legendSteps - (flip % legendSteps)) * 2;
                if (green < 0) green = 0;
            }
        }
        else if (legendBottom == BLUE){
            if (value > 128){
                blue = 255;
            }
            else {
                blue = 255 - (flip + legendSteps - (flip % legendSteps)) * 2;
                if (blue < 0) blue = 0;
            }
        }
        
        dict[value][0] = red;
        dict[value][1] = green;
        dict[value][2] = blue;
    }
}

// Get the red part of the color mapped to "value"
GLubyte ColorMapper::getRed(int value){
	if (value < 0 || value > 255) throw 505;
    
    return dict[value][0];
}

// Get the green part of the color mapped to "value"
GLubyte ColorMapper::getGreen(int value){
	if (value < 0 || value > 255) throw 505;
    
    return dict[value][1];
}

// Get the blue part of the color mapped to "value"
GLubyte ColorMapper::getBlue(int value){
	if (value < 0 || value > 255) throw 505;
    
    return dict[value][2];
}

// Basic OpenGL initialization code
void ColorMapper::init(void)
{
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    loadTextures();
}

// Load textures from pixel data
void ColorMapper::loadTextures(){
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    for (int x = 0; x < 14; ++x){
        for (int y = 0; y < 2; ++y){
            glGenTextures(1, &texNames[x][y]);
            glBindTexture(GL_TEXTURE_2D, texNames[x][y]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                         1024, 1024,
                         0, GL_RGBA, GL_UNSIGNED_BYTE,
                         seismicChunks->seismicChunks[x][y]);
            
            // Generate mipmaps
            gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, 1024, 1024, GL_RGBA, GL_UNSIGNED_BYTE, seismicChunks->seismicChunks[x][y]);
        }
    }
}

// Unload all textures
void ColorMapper::unloadTextures(){
    for (int x = 0; x < 14; ++x){
        for (int y = 0; y < 2; ++y){
            glDeleteTextures(1, &texNames[x][y]);
        }
    }
}

void ColorMapper::draw(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

	drawLegend();

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    glScalef(zoomLevel, zoomLevel, 0.0f); // Scale to zoomLevel
    glTranslatef(xPos/zoomLevel, yPos/zoomLevel, 0.0f); // Translate coordinates and account for zoomLevel

    // Iterate through the textures
    for (int y = 0; y < 2; y++){
        if (y == 1) glTranslatef(-14.0f, -1.0f, 0.0f); // Translate back 
        for (int x = 0; x < 14; x++){
            glTranslatef(1.0f, 0.0f, 0.0f); // Move right
            glBindTexture(GL_TEXTURE_2D, texNames[x][y]);
            
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(0.0f, 0.5f, 0.0f);
            
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(0.0f, -0.5f, 0.0f);
            
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(1.0f, -0.5f, 0.0f);
            
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(1.0f, 0.5f, 0.0f);
            glEnd();
        }
    }

    glutSwapBuffers();
    glDisable(GL_TEXTURE_2D);
}

// Render a string using glut font specified
void ColorMapper::renderString(float x, float y, float z, void *font, char *string){
    glRasterPos3f(x, y, z);
    
    for (char *c = string; *c != '\0'; ++c){
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }
}

// Drawing legend on the left side of the screen
void ColorMapper::drawLegend(void){
	glTranslatef(-screenWidth/2.0f, screenHeight/2.0f, 0.0f); // Translate to top left of screen

	int legendSteps = 256 / legendResolution;
	for (int i = 0; i < legendResolution; ++i){
		int yOffset = legendSteps * i * 2;
        
        int value = ((i+1) * legendSteps) - 1;
        
        // Generate string of the range
        char strBuf[12];
        int strLen = sprintf(strBuf, "%i - %i", i > 0 ? i*legendSteps -127 : -128, i < legendResolution - 1 ? (i+1) * legendSteps - 128 : 127);
        
        // Write range centered with black letters
        glColor3ub(0, 0, 0);
        renderString(11.0f + (4 * 11) - (strLen * 4), -25.0f -yOffset - (13 * (16 / legendResolution)), 0.0f, GLUT_BITMAP_8_BY_13, strBuf);
        
		glColor3ub(getRed(value), getGreen(value), getBlue(value));
		glBegin(GL_QUADS);
            glVertex3f(11.0f, -11.0f -yOffset - (i == 0 ? 0.0f : 1.0f), 0.0f);
			glVertex3f(11.0f, -11.0f -yOffset - legendSteps * 2, 0.0f);
			glVertex3f(109.0f, -11.0f -yOffset - legendSteps * 2, 0.0f);
			glVertex3f(109.0f, -11.0f -yOffset - (i == 0 ? 0.0f : 1.0f), 0.0f);
		glEnd();
	}
	
    // Black background/border of legend
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
		glVertex3f(10.0f, -10.0f, 0.0f);
		glVertex3f(10.0f, -524.0f, 0.0f);
		glVertex3f(110.0f, -524.0f, 0.0f);
		glVertex3f(110.0f, -10.0f, 0.0f);
	glEnd();

	glTranslatef(screenWidth/2.0f, -screenHeight/2.0f, 0.0f); // Translate back to middle of screen
}

void ColorMapper::reshape(int w, int h)
{
	screenWidth = w;
	screenHeight = h;
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-w/2, w/2, -h/2, h/2); // Set to actual window pixel size
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    draw();
}

// Used to change the current legend used
void ColorMapper::setLegend(Color bottom, Color top, GLubyte resolution){
    if (bottom == top){
        throw 505;
    }
    
    legendBottom = bottom;
    legendTop = top;
    legendResolution = resolution;

	
    seismicChunks = new seisChunkWrapper(); // Allocate matrices for color values
    
    if (improvedLegend) fillLegend2(); // Use improved legend generator
    else fillLegend(); // Use old legend generator
    makeSeismicChunks(); // Generate pixel data
    
    unloadTextures(); // Unload old textures
    loadTextures(); // Load new textures from generated pixel data
    
    delete seismicChunks; // Deallocate pixel data
}

// Choose which legend generator to use
void ColorMapper::setUseImprovedLegend(bool choice){
    improvedLegend = choice;
}
