/*
maze.c

A simple maze generation program by Kyle J Aleshire
February 9, 2010

*/


#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define INTSIZE 4
#define WINDOWSIZE 500
#define CELLNUMBER (height * width)
#define PADDING 3.6
#define NORTH 1
#define EAST 0
#define SOUTH 3
#define WEST 2

typedef struct wall {
	int active;
	float x1, y1, x2, y2;
} wall_t;

typedef struct cell {
	wall_t *north, *east, *south, *west;
} cell_t;

typedef struct rodent {
	int direction, location_x, location_y;
} rodent_t;

cell_t **mazecell;
wall_t **vertwall, **horiwall;
rodent_t *rat;
int height, width, roots, *sets, egress[2][2];
float draw_offset;
GLfloat draw_offset, ratv[][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
GLubyte rati[4] = {0, 2, 1, 3};

void init(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-2.0, 2.0, -2.0, 2.0, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(0.0, 0.0, 0.0);
}

void line(float x1, float y1, float x2, float y2){
	glBegin(GL_LINES);
		glColor3f(0.0, 0.0, 0.0);
		glVertex2f(x1, y1);
		glVertex2f(x2, y2);
	glEnd();
}

void rodent(){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(draw_offset * (GLfloat) width / -2.0 + ((GLfloat) rat->location_x + 0.5) * draw_offset,
		draw_offset * (GLfloat) height / -2.0 + ((GLfloat) rat->location_y + 0.5) * draw_offset, 0.0);

	glRotatef(rat->direction * 90, 0.0, 0.0, 1.0);

	glVertexPointer(3, GL_FLOAT, 0, ratv);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, rati);
	glLoadIdentity();

	glutPostRedisplay();
}

void maze(){
	int i;
	for(i = 0; i < CELLNUMBER + width; i++){
		if(horiwall[i]->active == 1){
			line(horiwall[i]->x1, horiwall[i]->y1, horiwall[i]->x2, horiwall[i]->y2);
		}
	}
	for(i = 0; i < CELLNUMBER + height; i++){
		if(vertwall[i]->active == 1){
			line(vertwall[i]->x1, vertwall[i]->y1, vertwall[i]->x2, vertwall[i]->y2);
		}
	}
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	maze();
	rodent();

	glutSwapBuffers();
}

void reshape(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h)
		glOrtho(-2.0, 2.0, -2.0*(float)h/(float)w, 2.0*(float)h/(float)w, -10.0, 10.0);
	else
		glOrtho(-2.0*(float)w/(float)h, 2.0*(float)w/(float)h, -2.0, 2.0, -10.0, 10.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void arrowkeys(int key, int x, int y){
	switch(key){
		case GLUT_KEY_RIGHT:
			rat->direction = EAST;
			if(mazecell[rat->location_y * width + rat->location_x]->east->active == 0){
				rat->location_x++;
			}
			break;
		case GLUT_KEY_UP:
			rat->direction = NORTH;
			if(mazecell[rat->location_y * width + rat->location_x]->north->active == 0){
				rat->location_y++;
			}
			break;
		case GLUT_KEY_LEFT:
			rat->direction = WEST;
			if(mazecell[rat->location_y * width + rat->location_x]->west->active == 0){
				rat->location_x--;
			}
			break;
		case GLUT_KEY_DOWN:
			rat->direction = SOUTH;
			if(mazecell[rat->location_y * width + rat->location_x]->south->active == 0){
				rat->location_y--;
			}
			break;
	}
	glutPostRedisplay();
}

void mem_init(){
	int i, j, cellsize = sizeof(cell_t), wallsize = sizeof(wall_t);

	rat = malloc(sizeof(rodent_t));
	for(i = 0; i < width + 1; i++)
		for(j = 0; j < height; j++)
			vertwall[height * i + j] = malloc(wallsize);
	for(i = 0; i < width; i++)
		for(j = 0; j < height + 1; j++)
			horiwall[width * j + i] = malloc(wallsize);
	for(i = 0; i < width; i++)
		for(j = 0; j < height; j++)
			mazecell[width * j + i] = malloc(cellsize);

}

void grid_init(){
	int i, j, offset;

	draw_offset = PADDING / width;
	if(draw_offset > PADDING / (float) height)
		draw_offset  = PADDING / (float) height;

	for(i = 0; i < CELLNUMBER; i++)
		sets[i]= -1;
	roots = CELLNUMBER;

	for(i = 0; i < width + 1; i++){
		for(j = 0; j < height; j++){
			offset = height * i + j;
			vertwall[offset]->active = 1;
			vertwall[offset]->x1 = vertwall[offset]->x2 = ((draw_offset * (float) width) / -2.0) + ((float) i * draw_offset);
			vertwall[offset]->y1 = ((draw_offset * (float) height) / -2.0) + ((float) j * draw_offset);
			vertwall[offset]->y2 = ((draw_offset * (float) height) / -2.0) + ((float) j * draw_offset) + draw_offset;
		}
	}

	for(i = 0; i < width; i++){
		for(j = 0; j < height + 1; j++){
			offset = width * j + i;
			horiwall[offset]->active = 1;
			horiwall[offset]->x1 = ((draw_offset * (float) width) / -2.0) + ((float) i * draw_offset);
			horiwall[offset]->y1 = horiwall[offset]->y2 = ((draw_offset * (float) height) / -2.0) + ((float) j *draw_offset);
			horiwall[offset]->x2 = ((draw_offset * (float) width) / -2.0) + ((float) i * draw_offset) + draw_offset;
		}
	}

	for(i = 0; i < width; i++){
		for(j = 0; j < height; j++){
			offset = width * j + i;

			mazecell[offset]->north = horiwall[width * (j + 1) + i];
			mazecell[offset]->east = vertwall[height * (i + 1) + j];
			mazecell[offset]->south = horiwall[width * j + i];
			mazecell[offset]->west = vertwall[height * i + j];
		}
	}
}

int findroot(int index){
	if(sets[index] < 0) return index;
	else{
		sets[index] = findroot(sets[index]);
		return sets[index];
	}
}

void unitesets(int origin, int dest){
	if(sets[dest] < sets[origin]) {
		sets[origin] = dest;
	}
	else {
		if(sets[origin] == sets[dest])
			sets[origin]--;
		sets[dest] = origin;
	}
	roots--;
}

void maze_init(){

	int i, xrand, yrand, direction, origin_o, destination_o;
	srand(time(NULL));

	while(roots > 1){
		xrand = rand() % width;
		yrand = rand() % height;
		direction = rand() % 4;

		origin_o = yrand * width + xrand;

		switch(direction){
			case NORTH:
				if(yrand < height - 1){
					destination_o = (yrand + 1) * width + xrand;
					if(findroot(origin_o) != findroot(destination_o)){
						unitesets(findroot(origin_o), findroot(destination_o));
						mazecell[origin_o]->north->active = 0;
					}
				}
				break;
			case EAST:
				if(xrand < width - 1){
					destination_o = origin_o + 1;
					if(findroot(origin_o) != findroot(destination_o)){
						unitesets(findroot(origin_o), findroot(destination_o));
						mazecell[origin_o]->east->active = 0;
					}
				}
				break;
			case SOUTH:
				if(yrand > 0){
					destination_o = (yrand - 1) * width + xrand;
					if(findroot(origin_o) != findroot(destination_o)){
						unitesets(findroot(origin_o), findroot(destination_o));
						mazecell[origin_o]->south->active = 0;
					}
				}
				break;
			case WEST:
				if(xrand > 0){
					destination_o = origin_o - 1;
					if(findroot(origin_o) != findroot(destination_o)){
						unitesets(findroot(origin_o), findroot(destination_o));
						mazecell[origin_o]->west->active = 0;
					}
				}
				break;
		}
	}

	egress[0][0] = rand() % 4;
	egress[0][1] = rand();
	do{
		egress[1][0] = rand() % 4;
		egress[1][1] = rand();
	} while(egress[0][0] == egress[1][0] && egress[0][1] == egress[1][1]);

	for(i = 0; i < 2; i++){
		switch(egress[i][0]){
			case NORTH:
				egress[i][1] %= width;
				mazecell[CELLNUMBER - width + egress[i][1]]->north->active = 0;
				break;
			case EAST:
				egress[i][1] %= height;
				mazecell[(width * egress[i][1]) + width - 1]->east->active = 0;
				break;
			case SOUTH:
				egress[i][1] %= width;
				mazecell[egress[i][1]]->south->active = 0;
				break;
			case WEST:
				egress[i][1] %= height;
				mazecell[width * egress[i][1]]->west->active = 0;
				break;
		}
	}
}

void rodent_init(){
	rat->direction = (egress[0][0] + 2) % 4;

	switch(egress[0][0]){
		case NORTH:
			rat->location_x = egress[0][1];
			rat->location_y = height - 1;
			break;
		case EAST:
			rat->location_x = width - 1;
			rat->location_y = egress[0][1];
			break;
		case SOUTH:
			rat->location_x = egress[0][1];
			rat->location_y = 0;
			break;
		case WEST:
			rat->location_x = 0;
			rat->location_y = egress[0][1];
			break;
	}

	ratv[0][0] = 0.3 * draw_offset;
	ratv[0][1] = ratv[1][1] = 0.0;
	ratv[1][0] = -0.1 * draw_offset;
	ratv[2][0] = ratv[3][0] = -0.3 * draw_offset;
	ratv[2][1] = 0.3 * draw_offset;
	ratv[3][1] = -0.3 * draw_offset;
}

void menu(int option){
	switch(option){
		case 0:
			exit(0);
			break;
	}
}

int main(int argc, char *argv[]){
	if (argc < 3) {
		fprintf(stderr, "usage:\tmaze <width> <height>\n");
		exit(1);
	}

	width = atoi(argv[1]);
	height = atoi(argv[2]);

	vertwall = malloc((width + 1) * height * sizeof(void *));
	horiwall = malloc(width * (height + 1) * sizeof(void *));

	mazecell = malloc(width * height * sizeof(void *));

	sets = malloc(width * height * sizeof(void *));

	mem_init();
	grid_init();
	maze_init();

	rodent_init();

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("3D Rat Maze");

	glutSpecialFunc(arrowkeys);

	glutCreateMenu(menu);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glEnable(GL_DEPTH_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	init();

	glutMainLoop();
}