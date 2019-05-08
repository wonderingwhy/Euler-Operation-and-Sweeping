#include <cstdio>
#include <iostream>
#include "gl/glut.h"  
using namespace std;
GLdouble _mouseX = 0.0;
GLdouble _mouseY = 0.0;
bool _mouseLeftDown = false;
bool _mouseRightDown = false;
GLdouble _cameraDistance = -25.0;
GLdouble _cameraAngleX = 0.0;
GLdouble _cameraAngleY = 0.0;
struct Solid *solid;

void scale_change(int w, int h) {
	if (h == 0) {
		h = 1;
	}
	GLdouble ratio = 1.0 * w / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45, ratio, 10, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}
void mouse_down(int button, int state, int x, int y) {
	_mouseX = x;
	_mouseY = y;
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			_mouseLeftDown = true;
		}
		else if (state == GLUT_UP) {
			_mouseLeftDown = false;
		}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		_mouseRightDown = !_mouseRightDown;
		glutPostRedisplay();
	}
}
void mouse_move(int x, int y) {
	if (_mouseLeftDown) {
		_cameraAngleY += (x - _mouseX) / 2;
		_cameraAngleX += (y - _mouseY) / 2;
		_mouseX = x;
		_mouseY = y;
	}
	glutPostRedisplay();
}

struct Solid {
	int id;
	struct Solid *prev_solid, *next_solid;
	struct Face *solid_face;
	struct Edge *solid_edge;
	Solid() :prev_solid(nullptr), next_solid(nullptr), solid_face(nullptr), solid_edge(nullptr){}
	Solid(int id) :prev_solid(nullptr), next_solid(nullptr), solid_face(nullptr), solid_edge(nullptr), id(id){}
	void print();
};
struct Face {
	int id;
	struct Face *prev_face, *next_face;
	struct Solid *face_solid;
	struct Loop *face_loop;
	Face() :prev_face(nullptr), next_face(nullptr), face_solid(nullptr), face_loop(nullptr){}
	Face(int id) :prev_face(nullptr), next_face(nullptr), face_solid(nullptr), face_loop(nullptr), id(id){}
	void print();
	GLfloat normal[3];
	void get_normal();
};
struct Loop {
	int id;
	struct Loop *prev_loop, *next_loop;
	struct Face *loop_face;
	struct HalfEdge *loop_halfedge;
	Loop() :prev_loop(nullptr), next_loop(nullptr), loop_face(nullptr), loop_halfedge(nullptr){}
	Loop(int id) :prev_loop(nullptr), next_loop(nullptr), loop_face(nullptr), loop_halfedge(nullptr), id(id){}
	void print();
};
struct HalfEdge {
	int id;
	struct HalfEdge *prev_halfedge, *next_halfedge, *brother_halfedge;
	struct Vertex *from_vertex, *to_vertex;
	struct Loop *halfedge_loop;
	struct Edge *halfedge_edge;
	HalfEdge() :prev_halfedge(nullptr), next_halfedge(nullptr), brother_halfedge(nullptr), from_vertex(nullptr), to_vertex(nullptr), halfedge_loop(nullptr), halfedge_edge(nullptr){}
	HalfEdge(int id) :prev_halfedge(nullptr), next_halfedge(nullptr), brother_halfedge(nullptr), from_vertex(nullptr), to_vertex(nullptr), halfedge_loop(nullptr), halfedge_edge(nullptr), id(id){}
	HalfEdge(struct Edge *edge, int id) :prev_halfedge(nullptr), next_halfedge(nullptr), brother_halfedge(nullptr), from_vertex(nullptr), to_vertex(nullptr), halfedge_loop(nullptr), halfedge_edge(edge), id(id){}
};
struct Edge {
	int id;
	struct Edge *prev_edge, *next_edge;
	struct HalfEdge *edge_halfedge0, *edge_halfedge1;
	Edge() :prev_edge(nullptr), next_edge(nullptr), edge_halfedge0(nullptr), edge_halfedge1(nullptr){}
	Edge(struct HalfEdge *edge_halfedge0, struct HalfEdge *edge_halfedge1, int id) :prev_edge(nullptr), next_edge(nullptr), edge_halfedge0(edge_halfedge0), edge_halfedge1(edge_halfedge1), id(id){}
};
struct Vertex {
	int id;
	struct Vertex *prev_vertex;
	struct Vertex *next_vertex;
	GLdouble point[3];
	Vertex() :prev_vertex(nullptr), next_vertex(nullptr) {}
	Vertex(GLdouble _point[3], int id) :prev_vertex(nullptr), next_vertex(nullptr), id(id) {
		point[0] = _point[0];
		point[1] = _point[1];
		point[2] = _point[2];
	}
};
void debug() {
	puts("********************");
	solid->print();
	for (struct Face *face = solid->solid_face; face != nullptr; face = face->next_face) {
		face->print();
		for (struct Loop *loop = face->face_loop; loop != nullptr; loop = loop->next_loop) {
			loop->print();
		}
	}
	puts("********************");
}
void Face::get_normal() {
	Vertex *vertex0, *vertex1, *vertex2;
	HalfEdge *halfedge = face_loop->loop_halfedge;
	vertex0 = halfedge->from_vertex;
	halfedge = halfedge->next_halfedge;
	vertex1 = halfedge->from_vertex;
	halfedge = halfedge->next_halfedge;
	vertex2 = halfedge->from_vertex;
	GLdouble a[3], b[3];
	a[0] = vertex1->point[0] - vertex0->point[0];
	a[1] = vertex1->point[1] - vertex0->point[1];
	a[2] = vertex1->point[2] - vertex0->point[2];

	b[0] = vertex0->point[0] - vertex2->point[0];
	b[1] = vertex0->point[1] - vertex2->point[1];
	b[2] = vertex0->point[2] - vertex2->point[2];
	normal[0] = a[1] * b[2] - a[2] * b[1];
	normal[1] = a[2] * b[0] - a[0] * b[2];
	normal[2] = a[0] * b[1] - a[1] * b[0];
}
void Solid::print() {
	printf("solid_face:");
	for (struct Face *face = solid->solid_face; face != nullptr; face = face->next_face) {
		printf("%d ", face->id);
	}
	printf("\n");
	printf("solid_edge:");
	for (struct Edge *edge = solid_edge; edge != nullptr; edge = edge->next_edge) {
		printf("%d ", edge->id);
	}
	printf("\n");
}
void Face::print() {
	printf("face_loop:");
	for (struct Loop *loop = face_loop; loop != nullptr; loop = loop->next_loop) {
		printf("%d ", loop->id);
	}
	printf("\n");
}
void Loop::print() {
	printf("loop_halfedge:");
	int idx;
	if (loop_halfedge != nullptr) {
		idx = loop_halfedge->id;
		printf("%d ", loop_halfedge->id);
		printf("(%d, %d) ", loop_halfedge->from_vertex->id, loop_halfedge->to_vertex->id);
		for (struct HalfEdge *halfedge = loop_halfedge->next_halfedge; halfedge->id != idx; halfedge = halfedge->next_halfedge) {
			printf("%d ", halfedge->id);
			printf("(%d, %d) ", halfedge->from_vertex->id, halfedge->to_vertex->id);
		}
	}
	printf("\n");
}
int solid_num = 0;
int face_num = 0;
int loop_num = 0;
int halfedge_num = 0;
int edge_num = 0;
int vertex_num = 0;
void add_edge_to_solid(Edge *edge, Solid *solid) {
	if (solid->solid_edge == nullptr) {
		solid->solid_edge = edge;
	}
	else {
		Edge *temp_edge = solid->solid_edge;
		while (temp_edge->next_edge != nullptr) {
			temp_edge = temp_edge->next_edge;
		}
		temp_edge->next_edge = edge;
		edge->prev_edge = temp_edge;
	}
}
void add_face_to_solid(Face *face, Solid *solid) {
	face->face_solid = solid;
	if (solid->solid_face == nullptr) {
		solid->solid_face = face;
	}
	else {
		Face *temp_face = solid->solid_face;
		while (temp_face->next_face != nullptr) {
			temp_face = temp_face->next_face;
		}
		temp_face->next_face = face;
		face->prev_face = temp_face;
	}
}
void add_loop_to_face(Loop *loop, Face *face) {
	loop->loop_face = face;
	if (face->face_loop == nullptr) {
		face->face_loop = loop;
	}
	else {
		Loop *temp_loop = face->face_loop;
		while (temp_loop->next_loop != nullptr) {
			temp_loop = temp_loop->next_loop;
		}
		temp_loop->next_loop = loop;
		loop->prev_loop = temp_loop;
	}
}
void delete_edge_from_solid(Edge *edge, Solid *solid) {
	Edge *prev_edge = edge->prev_edge, *next_edge = edge->next_edge;
	if (prev_edge == nullptr && next_edge == nullptr) {
		solid->solid_edge = nullptr;
		return;
	}
	if (prev_edge == nullptr) {
		solid->solid_edge = next_edge;
		return;
	}
	if (next_edge == nullptr) {
		prev_edge->next_edge = nullptr;
		return;
	}
	prev_edge->next_edge = next_edge;
	next_edge->prev_edge = prev_edge;
}
void delete_face_from_solid(Face *face, Solid *solid) {
	Face *prev_face = face->prev_face, *next_face = face->next_face;
	if (prev_face == nullptr && next_face == nullptr) {
		solid->solid_face = nullptr;
		return;
	}
	if (prev_face == nullptr) {
		solid->solid_face = next_face;
		return;
	}
	if (next_face == nullptr) {
		prev_face->next_face = nullptr;
		return;
	}
	prev_face->next_face = next_face;
	next_face->prev_face = prev_face;
}
Solid *mvsf(GLdouble point[3], Vertex &vertex) {
	Solid *solid = new Solid(solid_num++);
	Face *face = new Face(face_num++);
	Loop *loop = new Loop(loop_num++);
	vertex = *(new Vertex(point, vertex_num++));
	solid->solid_face = face;
	face->face_solid = solid;
	face->face_loop = loop;
	loop->loop_face = face;
	return solid;
}
Vertex *mev(Vertex *vertex0, GLdouble point[3], Loop *loop) {
	Solid *solid = loop->loop_face->face_solid;
	HalfEdge *halfedge0 = new HalfEdge(halfedge_num++);
	HalfEdge *halfedge1 = new HalfEdge(halfedge_num++);
	Edge *edge = new Edge(halfedge0, halfedge1, edge_num++);
	Vertex *vertex1 = new Vertex(point, vertex_num++);
	halfedge0->halfedge_edge = edge;
	halfedge1->halfedge_edge = edge;
	halfedge0->halfedge_loop = halfedge1->halfedge_loop = loop;
	halfedge0->from_vertex = halfedge1->to_vertex = vertex0;
	halfedge1->from_vertex = halfedge0->to_vertex = vertex1;
	halfedge0->brother_halfedge = halfedge1;
	halfedge1->brother_halfedge = halfedge0;

	halfedge0->next_halfedge = halfedge1;
	halfedge1->prev_halfedge = halfedge0;
	if (loop->loop_halfedge == nullptr) {
		halfedge1->next_halfedge = halfedge0;
		halfedge0->prev_halfedge = halfedge1;
		loop->loop_halfedge = halfedge0;
	}
	else {
		HalfEdge *temp_halfedge;
		for (temp_halfedge = loop->loop_halfedge;
			temp_halfedge->next_halfedge->from_vertex->id != vertex0->id;
			temp_halfedge = temp_halfedge->next_halfedge);
		halfedge0->prev_halfedge = temp_halfedge;
		halfedge1->next_halfedge = temp_halfedge->next_halfedge;
		halfedge0->prev_halfedge->next_halfedge = halfedge0;
		halfedge1->next_halfedge->prev_halfedge = halfedge1;
	}
	add_edge_to_solid(edge, loop->loop_face->face_solid);
	printf("///mev %d %d\n", vertex0->id, vertex1->id);
	debug();
	return vertex1;
}
Loop *mef(Vertex *vertex0, Vertex *vertex1, Loop *loop0) {
	HalfEdge *halfedge0, *halfedge1, *temp_halfedge0, *temp_halfedge1;
	Edge *edge;
	Loop *loop1;
	loop1 = new Loop(loop_num++);
	halfedge0 = new HalfEdge(halfedge_num++);
	halfedge1 = new HalfEdge(halfedge_num++);
	edge = new Edge(halfedge0, halfedge1, edge_num++);
	halfedge0->from_vertex = vertex0;
	halfedge0->to_vertex = vertex1;
	halfedge1->from_vertex = vertex1;
	halfedge1->to_vertex = vertex0;
	halfedge0->brother_halfedge = halfedge1;
	halfedge0->halfedge_edge = edge;
	halfedge1->halfedge_edge = edge;

	for (temp_halfedge0 = loop0->loop_halfedge;
		temp_halfedge0->next_halfedge->from_vertex->id != vertex0->id;
		temp_halfedge0 = temp_halfedge0->next_halfedge);
	for (temp_halfedge1 = loop0->loop_halfedge;
		temp_halfedge1->next_halfedge->from_vertex->id != vertex1->id;
		temp_halfedge1 = temp_halfedge1->next_halfedge);
	for (temp_halfedge1 = temp_halfedge1->next_halfedge;
		temp_halfedge1->next_halfedge->from_vertex->id != vertex1->id;
		temp_halfedge1 = temp_halfedge1->next_halfedge);

	halfedge0->next_halfedge = temp_halfedge1->next_halfedge;
	halfedge0->next_halfedge->prev_halfedge = halfedge0;
	halfedge1->next_halfedge = temp_halfedge0->next_halfedge;
	halfedge1->next_halfedge->prev_halfedge = halfedge1;

	halfedge0->prev_halfedge = temp_halfedge0;
	halfedge0->prev_halfedge->next_halfedge = halfedge0;
	halfedge1->prev_halfedge = temp_halfedge1;
	halfedge1->prev_halfedge->next_halfedge = halfedge1;


	halfedge0->halfedge_loop = loop0;
	halfedge1->halfedge_loop = loop1;
	loop0->loop_halfedge = halfedge0->next_halfedge->next_halfedge;
	loop1->loop_halfedge = halfedge1;

	Face *face;
	face = new Face(face_num++);
	add_loop_to_face(loop1, face);
	add_face_to_solid(face, loop0->loop_face->face_solid);
	add_edge_to_solid(edge, loop0->loop_face->face_solid);
	printf("///mef %d %d\n", vertex0->id, vertex1->id);
	debug();
	return loop1;
}
HalfEdge *find_halfedge(Vertex *vertex0, Vertex *vertex1, Loop *loop) {
	if (loop->loop_halfedge == nullptr) {
		return nullptr;
	}
	int idx = loop->loop_halfedge->id;
	int id0 = vertex0->id;
	int id1 = vertex1->id;
	HalfEdge *halfedge;
	for (halfedge = loop->loop_halfedge->next_halfedge; halfedge->from_vertex->id != id0 || halfedge->to_vertex->id != id1; halfedge = halfedge->next_halfedge);
	return halfedge;
}
Loop *kemr(Vertex *vertex0, Vertex *vertex1, Loop *loop0) {
	HalfEdge *halfedge0 = find_halfedge(vertex0, vertex1, loop0);
	HalfEdge *halfedge1 = halfedge0->brother_halfedge;
	Edge *edge = halfedge0->halfedge_edge;

	Face *face = loop0->loop_face;
	Loop *loop1 = new Loop(loop_num++);
	HalfEdge *halfedge0_prev = halfedge0->prev_halfedge, *halfedge0_next = halfedge0->next_halfedge,
		*halfedge1_prev = halfedge1->prev_halfedge, *halfedge1_next = halfedge1->next_halfedge;
	halfedge0->prev_halfedge->next_halfedge = halfedge1_next;
	halfedge1->prev_halfedge->next_halfedge = halfedge0_next;
	halfedge0->next_halfedge->prev_halfedge = halfedge1_prev;
	halfedge1->next_halfedge->prev_halfedge = halfedge0_prev;
	delete_edge_from_solid(edge, solid);

	loop1->loop_halfedge = halfedge0->next_halfedge;
	add_loop_to_face(loop1, face);
	printf("///kemr %d %d\n", vertex0->id, vertex1->id);
	debug();
	return loop1;
}
void kfmrh(Loop *loop0, Loop *loop1) {
	Face *face0 = loop0->loop_face;
	Face *face1 = loop1->loop_face;
	add_loop_to_face(loop1, face0);
	delete_face_from_solid(face1, solid);
}
Vertex *copy_vertex(Vertex *vertex) {
	Vertex *new_vertex = new Vertex();
	new_vertex->id = vertex->id;
	new_vertex->next_vertex = vertex->next_vertex;
	new_vertex->point[0] = vertex->point[0];
	new_vertex->point[1] = vertex->point[1];
	new_vertex->point[2] = vertex->point[2];
	new_vertex->prev_vertex = vertex->prev_vertex;
	return new_vertex;
}
void sweep(GLdouble vector[3]) {
	for (Loop *loop = solid->solid_face->face_loop; loop != nullptr; loop = loop->next_loop) {
		HalfEdge *halfedge = loop->loop_halfedge;
		int idx = halfedge->from_vertex->id;
		Vertex vertex[100];
		int cnt = 0;
		do {
			GLdouble point[3];
			Vertex *vertex0 = halfedge->from_vertex;
			point[0] = vertex0->point[0] + vector[0];
			point[1] = vertex0->point[1] + vector[1];
			point[2] = vertex0->point[2] + vector[2];
			vertex[cnt++] = *copy_vertex(mev(vertex0, point, loop));
			halfedge = halfedge->next_halfedge;
		} while (halfedge->from_vertex->id != idx);
		for (int i = 0; i < cnt; ++i) {
			mef(copy_vertex(vertex + i), copy_vertex(vertex + (i + 1) % cnt), loop);
		}
	}
}

void CALLBACK vertexCallback(GLvoid *vertex) {
	const GLdouble *pointer = (GLdouble *)vertex;
	glColor3d(1.0f, 1.0f, 1.0f);
	glVertex3dv(pointer);
}
void CALLBACK beginCallback(GLenum which) {
	glBegin(which);
}
void CALLBACK endCallback() {
	glEnd();
}
void init() {
	glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	GLfloat ambientLight[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat light_position[] = { 100.0f, 100.0f, 0.0f, 1.0f };
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	//glEnable(GL_COLOR_MATERIAL);
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	//glMateriali(GL_FRONT, GL_SHININESS, 128);
	//glEnable(GL_NORMALIZE);
}
void display() {
	init();
	glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(0, 0, _cameraDistance);
	glRotatef(_cameraAngleX, 1, 0, 0);
	glRotatef(_cameraAngleY, 0, 1, 0);

	if (_mouseRightDown == false) {
		GLUtesselator *tobj = gluNewTess();
		if (!tobj) { return; }
		gluTessCallback(tobj, GLU_TESS_VERTEX, (void (CALLBACK *)())vertexCallback);
		gluTessCallback(tobj, GLU_TESS_BEGIN, (void (CALLBACK *)())beginCallback);
		gluTessCallback(tobj, GLU_TESS_END, (void (CALLBACK *)())endCallback);
		for (Face *face = solid->solid_face; face != nullptr; face = face->next_face) {
			gluTessBeginPolygon(tobj, nullptr);
			face->get_normal();
			glNormal3f(face->normal[0], face->normal[1], face->normal[2]);
			for (Loop *loop = face->face_loop; loop != nullptr; loop = loop->next_loop) {
				gluTessBeginContour(tobj);
				HalfEdge *halfedge = loop->loop_halfedge;
				int idx = halfedge->id;
				do {
					gluTessVertex(tobj, halfedge->from_vertex->point, halfedge->from_vertex->point);
					halfedge = halfedge->next_halfedge;
				} while (idx != halfedge->id);
				gluTessEndContour(tobj);
			}
			gluTessEndPolygon(tobj);


		}
		gluDeleteTess(tobj);
	}
	else {
		Edge *edge = solid->solid_edge;
		while (edge != nullptr) {
			glBegin(GL_LINES);
			Vertex *v1 = edge->edge_halfedge0->from_vertex;
			Vertex *v2 = edge->edge_halfedge0->to_vertex;
			glVertex3f(v1->point[0], v1->point[1], v1->point[2]);
			glVertex3f(v2->point[0], v2->point[1], v2->point[2]);
			edge = edge->next_edge;
			glEnd();
		}
	}
	
	glPopMatrix();
	glutSwapBuffers();
}
Vertex vertex[10000];
int main(int argc, char** argv) {
	freopen("input.txt", "r", stdin);
	int n, m, offset = 0;
	GLdouble point[3];
	scanf("%d", &n);
	for (int i = 0; i < n; ++i) {
		scanf("%d", &m);
		Loop *loop0, *loop1, *loop2;
		for (int j = 0; j < m; ++j) {
			scanf("%lf %lf %lf", &point[0], &point[1], &point[2]);
			if (i == 0 && j == 0) {
				solid = mvsf(point, vertex[0]);
				loop0 = solid->solid_face->face_loop;
			}
			else if (i == 0) {
				vertex[offset + j] = *mev(vertex + offset + j - 1, point, loop0);
			}
			else if (j == 0) {
				vertex[offset + j] = *mev(vertex, point, loop1);
			}
			else {
				vertex[offset + j] = *mev(vertex + offset + j - 1, point, loop1);
			}
		}
		if (i == 0) {
			loop1 = mef(vertex + m - 1, vertex, loop0);
		}
		else {
			Loop *loop2 = mef(vertex + offset + m - 1, vertex + offset, loop1);
			kemr(vertex, vertex + offset, loop1);
			kfmrh(loop0, loop2);
		}
		offset += m;
	}
	GLdouble vector[3];
	scanf("%lf %lf %lf", &vector[0], &vector[1], &vector[2]);
	sweep(vector);
	debug();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(720, 540);
	glutCreateWindow("OpenGL");
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
	glutDisplayFunc(display);
	glutMouseFunc(mouse_down);
	glutMotionFunc(mouse_move);
	glutReshapeFunc(scale_change);
	glutMainLoop();
	return 0;
}