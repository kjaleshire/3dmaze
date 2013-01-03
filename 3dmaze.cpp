/*
maze.c

A simple maze generation program by Kyle J Aleshire
February 9, 2010

*/

#ifdef __APPLE__ & __MACH__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <time.h>

using namespace std;

#define TRUE 1
#define FALSE 0
#define INTSIZE 4
#define WINDOWSIZE 500
#define CELLNUMBER (height * width)
#define PADDING 3.6
#define NORTH 1
#define EAST 0
#define SOUTH 3
#define WEST 2

class Wall {
  	bool isactive;
		bool isdrawable;
		GLfloat x1, y1, x2, y2;
	public:
		Wall(GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4){
			isdrawable = TRUE;
			isactive = TRUE;
			x1 = v1;
			y1 = v2;
			x2 = v3;
			y2 = v4;
		}
		void draw(){
			if(isdrawable){
				glBegin(GL_LINES);
					glVertex2f(x1, y1);
					glVertex2f(x2, y2);
				glEnd();
			}
		}
		void unsetactive(){
			isactive = FALSE;
		}
		void unsetdrawable(){
			isdrawable = FALSE;
		}
		bool drawable(){
			return isdrawable;
		}
		bool active(){
			return isactive;
		}
} ;

class Cell {
		Wall *east, *north, *west, *south;
	public:
		Cell(Wall *e, Wall *n, Wall *w, Wall *s){
			east = e;
			north = n;
			west = w;
			south = s;
		}
		Wall* geteast(){
			return east;
		}
		Wall* getnorth(){
			return north;
		}
		Wall* getwest(){
			return west;
		}
		Wall* getsouth(){
			return south;
		}
} ;

class Disjointset{
		int roots, *sets;
	public:
		Disjointset(int width, int height){
			int i;
			sets = new int[CELLNUMBER];
			for(i = 0; i < CELLNUMBER; i++)
				sets[i] = -1;
			roots = CELLNUMBER;
		}
		~Disjointset(){
			delete[] sets;
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
		int getroots(){
			return roots;
		}
};

class Maze{
		GLfloat draw_offset;
		Disjointset *set;
		int width, height, egress[2][2];
	public:
		Cell **cells;
		Wall **verticals, **horizontals;
		Maze(int w, int h){
			int i, j, offset, xrand, yrand, direction, origin_o, destination_o;
			
			width = w;
			height = h;
			
			cells = new Cell * [CELLNUMBER];
	
			verticals = new Wall * [(width + 1) * height];
			horizontals = new Wall * [width * (height + 1)];
	
			set = new Disjointset(width, height);
			
			draw_offset = PADDING / width;
			if(draw_offset > PADDING / (GLfloat) height)
				draw_offset = PADDING / (GLfloat) height;

			for(i = 0; i < width + 1; i++){
				for(j = 0; j < height; j++){
					offset = height * i + j;
					verticals[offset] = new Wall(((draw_offset * (GLfloat) width) / -2.0) + ((GLfloat) i * draw_offset),
												((draw_offset * (GLfloat) height) / -2.0) + ((GLfloat) j * draw_offset),
												((draw_offset * (GLfloat) width) / -2.0) + ((GLfloat) i * draw_offset), 
												((draw_offset * (GLfloat) height) / -2.0) + ((GLfloat) j * draw_offset) + draw_offset);
				}
			}

			for(i = 0; i < width; i++){
				for(j = 0; j < height + 1; j++){
					offset = width * j + i;
					horizontals[offset] = new Wall(((draw_offset * (GLfloat) width) / -2.0) + ((GLfloat) i * draw_offset),
													((draw_offset * (GLfloat) height) / -2.0) + ((GLfloat) j *draw_offset),
													((draw_offset * (GLfloat) width) / -2.0) + ((GLfloat) i * draw_offset) + draw_offset,
													((draw_offset * (GLfloat) height) / -2.0) + ((GLfloat) j *draw_offset));
				}
			}
			
			for(i = 0; i < width; i++)
				for(j = 0; j < height; j++)
					cells[width * j + i] = new Cell(verticals[height * (i + 1) + j], horizontals[width * (j + 1) + i], verticals[height * i + j], horizontals[width * j + i]);
			
			srand(time(NULL));

			while(set->getroots() > 1){
				xrand = rand() % width;
				yrand = rand() % height;
				direction = rand() % 4;

				origin_o = yrand * width + xrand;

				switch(direction){
					case EAST:
						if(xrand < width - 1){
							destination_o = origin_o + 1;
							if(set->findroot(origin_o) != set->findroot(destination_o)){
								set->unitesets(set->findroot(origin_o), set->findroot(destination_o));
								cells[origin_o]->geteast()->unsetactive();
								cells[origin_o]->geteast()->unsetdrawable();
							} 
						}
						break;
					case NORTH:
						if(yrand < height - 1){
							destination_o = (yrand + 1) * width + xrand;
							if(set->findroot(origin_o) != set->findroot(destination_o)){
								set->unitesets(set->findroot(origin_o), set->findroot(destination_o));
								cells[origin_o]->getnorth()->unsetactive();
								cells[origin_o]->getnorth()->unsetdrawable();
							}
						}
						break;
					case WEST:
						if(xrand > 0){
							destination_o = origin_o - 1;
							if(set->findroot(origin_o) != set->findroot(destination_o)){
								set->unitesets(set->findroot(origin_o), set->findroot(destination_o));
								cells[origin_o]->getwest()->unsetactive();
								cells[origin_o]->getwest()->unsetdrawable();
							} 
						}
						break;
					case SOUTH:
						if(yrand > 0){
							destination_o = (yrand - 1) * width + xrand;
							if(set->findroot(origin_o) != set->findroot(destination_o)){
								set->unitesets(set->findroot(origin_o), set->findroot(destination_o));
								cells[origin_o]->getsouth()->unsetactive();
								cells[origin_o]->getsouth()->unsetdrawable();
							} 
						}
						break;
				}
			}
			
			makeegress();
		}
		
		void draw(){
			int i;
			for(i = 0; i < CELLNUMBER + width; i++){
				horizontals[i]->draw();
			}
			for(i = 0; i < CELLNUMBER + height; i++){
				verticals[i]->draw();
			}
			glutPostRedisplay();
		}
		int entranceo(){
			return egress[0][0];
		}
		int entranced(){
			return egress[0][1];
		}
		int exito(){
			return egress[1][0];
		}
		int exitd(){
			return egress[1][1];
		}
		GLfloat getdrawoffset(){
			return draw_offset;
		}
		int getwidth(){
			return width;
		}
		int getheight(){
			return height;
		}
	private:
		void makeegress(){
			int i;
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
						cells[CELLNUMBER - width + egress[i][1]]->getnorth()->unsetdrawable();
						break;
					case EAST:
						egress[i][1] %= height;
						cells[(width * egress[i][1]) + width - 1]->geteast()->unsetdrawable();
						break;
					case SOUTH:
						egress[i][1] %= width;
						cells[egress[i][1]]->getsouth()->unsetdrawable();
						break;
					case WEST:
						egress[i][1] %= height;
						cells[width * egress[i][1]]->getwest()->unsetdrawable();
						break;
				}
			}
		}
} ;

class Rodent {
		int direction, x, y;
		GLfloat vertices[4][3];
		GLubyte indices[4];
		Maze *mm;
	public:
		Rodent(Maze *n){
			mm = n;
			indices[0] = 0;
			indices[1] = 2;
			indices[2] = 1;
			indices[3] = 3;
			direction = (mm->entranced() + 2) % 4;

			switch(mm->entranced()){
				case EAST:
					x = mm->getwidth() - 1;
					y = mm->entranceo();
					break;
				case NORTH:
					x = mm->entranceo();
					y = mm->getheight() - 1;
					break;
				case WEST:
					x = 0;
					y = mm->entranceo();
					break;
				case SOUTH:
					x = mm->entranceo();
					y = 0;
					break;
			}

			vertices[0][0] = vertices[2][1] = 0.3 * mm->getdrawoffset();
			vertices[0][1] = vertices[1][1] = 0.0;
			vertices[1][0] = -0.1 * mm->getdrawoffset();
			vertices[2][0] = vertices[3][0] = vertices[3][1] = -0.3 * mm->getdrawoffset();
			vertices[0][2] = vertices[1][2] = vertices[2][2] = vertices[3][2] = 0.0;
			
		}
		void draw(){
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glTranslatef(mm->getdrawoffset() * (GLfloat) mm->getwidth() / -2.0 + ((GLfloat) x + 0.5) * mm->getdrawoffset(),
				mm->getdrawoffset() * (GLfloat) mm->getheight() / -2.0 + ((GLfloat) y + 0.5) * mm->getdrawoffset(), 0.0);

			glRotatef(direction * 90, 0.0, 0.0, 1.0);

			glVertexPointer(3, GL_FLOAT, 0, vertices);
			glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, indices);
			glLoadIdentity();

			glutPostRedisplay();
		}
		int getx(){
			return x;
		}
		int gety(){
			return y;
		}
		int getd(){
			return direction;
		}
		void setx(GLfloat newx){
			x = newx;
		}
		void sety(GLfloat newy){
			y = newy;
		}
		void setd(int d){
			direction = d;
		}
} ;

Maze *m;
Rodent *r;

void init(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrtho(-2.0, 2.0, -2.0, 2.0, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(0.0, 0.0, 0.0);
}

void display(){	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m->draw();
	r->draw();

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
			r->setd(EAST);
			if(!m->cells[r->gety() * m->getwidth() + r->getx()]->geteast()->active()){
				r->setx(r->getx() + 1);
			}
			break;
		case GLUT_KEY_UP:
			r->setd(NORTH);
			if(!m->cells[r->gety() * m->getwidth() + r->getx()]->getnorth()->active()){
				r->sety(r->gety() + 1);
			}
			break;
		case GLUT_KEY_LEFT:
			r->setd(WEST);
			if(!m->cells[r->gety() * m->getwidth() + r->getx()]->getwest()->active()){
				r->setx(r->getx() - 1);
			}
			break;
		case GLUT_KEY_DOWN:
			r->setd(SOUTH);
			if(!m->cells[r->gety() * m->getwidth() + r->getx()]->getsouth()->active()){
				r->sety(r->gety() - 1);
			}
			break;
	}
	glutPostRedisplay();
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
	
	int width = atoi(argv[1]);
	int height = atoi(argv[2]);
	
	m = new Maze(width, height);	
	r = new Rodent(m);

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
