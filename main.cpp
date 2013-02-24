#ifdef __APPLE__
#include <GLUT/glut.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <GL\glut.h>
#include <GL\GLU.h>
#include <GL\GL.h>
#endif

#include <iostream>
#include <math.h>
#include <ctime>

#include "segy.h"
#include "ColorMapper.h"

using namespace std;

segy *seg;

ColorMapper *map;

void display(void)
{
    map->draw();
}

void reshape(int w, int h)
{
    map->reshape(w, h);
}

void keyboard (unsigned char key, int x, int y)
{
    static ColorMapper::Color legendTop = map->RED;
    static ColorMapper::Color legendBottom = map->GREEN;
    static bool legendType = true;
    static int resolutions[] = {4, 8, 16, 32};
    static int resolution = 1;

	switch (key) {
		case 27: // Esc
			exit(0);
			break;
		case 43: // +
			map->zoomLevel *= 1.2f;
			map->xPos *= 1.2f;
			map->yPos *= 1.2f;
			break;
		case 45: // -
			map->zoomLevel /= 1.2f;
			map->xPos /= 1.2f;
			map->yPos /= 1.2f;
			break;
        case 114: // R
            ++resolution %= 4;
            map->setLegend(legendBottom, legendTop, resolutions[resolution]);
            break;
        case 108: // L
            switch (legendBottom){
                case ColorMapper::RED:
                    if (legendTop == map->BLUE) legendBottom = map->GREEN;
                    else legendBottom = map->BLUE;
                    break;
                case ColorMapper::GREEN:
                    if (legendTop == map->BLUE) legendBottom = map->RED;
                    else legendBottom = map->BLUE;
                    break;
                case ColorMapper::BLUE:
                    if (legendTop == map->GREEN) legendBottom = map->RED;
                    else legendBottom = map->GREEN;
                    break;
            }
            map->setLegend(legendBottom, legendTop, resolutions[resolution]);
            break;
        case 111: // O
            switch (legendTop){
                case ColorMapper::RED:
                    if (legendBottom == map->BLUE) legendTop = map->GREEN;
                    else legendTop = map->BLUE;
                    break;
                case ColorMapper::GREEN:
                    if (legendBottom == map->BLUE) legendTop = map->RED;
                    else legendTop = map->BLUE;
                    break;
                case ColorMapper::BLUE:
                    if (legendBottom == map->GREEN) legendTop = map->RED;
                    else legendTop = map->GREEN;
                    break;
            }
            map->setLegend(legendBottom, legendTop, resolutions[resolution]);
            break;
        case 116: // T
            legendType = !legendType;
            map->setUseImprovedLegend(legendType);
            map->setLegend(legendBottom, legendTop, resolutions[resolution]);
            break;
		default:
			cout << key << endl;
			break;
	}
    display();
}

int xBuffer = 0;
int yBuffer = 0;

bool leftDown = false;
void mouse( int button, int state, int x, int y){
	if (button == GLUT_LEFT_BUTTON){
		xBuffer = x;
		yBuffer = y;

		if (state == GLUT_DOWN) leftDown = true;
		else leftDown = false;

	}
	else if (button == 3){
		map->zoomLevel *= 1.1f;
		map->xPos *= 1.1f;
		map->yPos *= 1.1f;
	}
	else if (button == 4){
		map->zoomLevel /= 1.1f;
		map->xPos /= 1.1f;
		map->yPos /= 1.1f;
	}
    else {
        cout << "Button: " << button << endl;
    }
    display();
}
void motion(int x, int y){
	if (leftDown){
		map->xPos += (float)(x-xBuffer);
		map->yPos -= (float)(y-yBuffer);

		xBuffer = x;
		yBuffer = y;
	}
    display();
}

int main(int argc, char** argv)
{
    cout << "Hello" << endl;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    
    seg = new segy("NVGT-88-06.sgy", 3600, 240, 13483, 1750);
	
    map = new ColorMapper(seg);
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMainLoop();
    return 0;
}