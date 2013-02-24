#ifndef COLOR_MAPPER_H
#define COLOR_MAPPER_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#elif defined(_WIN32)
#include <GL\glut.h>
#include <GL\GLU.h>
#include <GL\GL.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include "segy.h"

#define seisWidth 13483
#define seisHeight 1750

class ColorMapper{
public:
    enum Color { RED, GREEN, BLUE };
    
private:
    struct seisChunkWrapper{
        GLubyte seismicChunks[14][2][1024][1024][4];
    } *seismicChunks;
    
    GLuint texNames[14][2];
    
    segy *seg;

	int screenWidth, screenHeight;
    
    void makeSeismicChunks(void);
    
    void init(void);
    
    void loadTextures();
    
    void unloadTextures();
    
    Color legendBottom;
    
    Color legendTop;
    
    bool improvedLegend;
    
    GLubyte legendResolution;

	void drawLegend(void);

	GLubyte getRed(int value);

	GLubyte getGreen(int value);

	GLubyte getBlue(int value);
    
    void fillLegend(void);
    
    void fillLegend2(void);
    
    void renderString(float x, float y, float z, void *font, char *string);
    
	GLubyte dict[256][3];
    
public:
    
    float zoomLevel;
    
    float xPos;
    
    float yPos;
    
    ColorMapper(segy *seg);
    
    void draw(void);
    
    void reshape(int w, int h);
    
    void setLegend(Color bottom, Color top, GLubyte resolution);
    
    void setUseImprovedLegend(bool choice);
};

#endif /* defined(__TPG4162_Project1__ColorMapper__) */
