
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


// Nag³ówki
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <SFML/System/Time.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Define
#define ILOSC_WIERZCHOLKOW 3
#define PI 3.14159265359

// Kody shaderów
const GLchar* vertexSource = R"glsl(
#version 150 core
in vec3 position;
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 proj;	
in vec3 color;

out vec3 Color;

in vec2 aTexCoord;
out vec2 TexCoord;

in vec3 aNormal;
out vec3 Normal;
out vec3 FragPos;

void main(){
	TexCoord = aTexCoord;
	//Color = color;
	Normal=aNormal;
	gl_Position = proj * view * model * vec4(position, 1.0);
	FragPos = vec3(model * vec4(position, 1.0));
}
)glsl";

const GLchar* fragmentSource = R"glsl(
#version 150 core
in vec3 Color;
out vec4 outColor;
in vec2 TexCoord;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;


in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 camPos;
void main()
{
	//outColor=mix(texture(texture1,TexCoord),texture(texture2,TexCoord),0.5);

	//Jako ze w pliku .obj sa dane dotyczace tekstury, a nie same wierzcholki, to musimy ustawic teraz kolor wyjsciowy.

	//outColor=texture(texture1,TexCoord);
	//outColor = vec4(Color, 1.0);

	//ustawienie jednolitego koloru - bialy
	//outColor = vec4(1.0, 1.0, 1.0, 1.0);

	//ambient
	float ambientStrength = 0.1;
	vec3 ambientlightColor = vec3(1.0,1.0,1.0);
	vec4 ambient = ambientStrength * vec4(ambientlightColor,1.0);

	//diffuse
	float diffuseStrength = 1.0;
	vec3 diffuselightColor = vec3(1.0,1.0,1.0);
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffvec = diff * diffuselightColor * diffuseStrength;
	vec4 diffuse = vec4(diffvec,1.0);

	//specular
	float specularStrength = 1.0;
	vec3 specularlightColor = vec3(1.0,1.0,1.0);
	vec3 viewDir = normalize(camPos - FragPos);
	vec3 reflectDir = reflect(-lightDir,norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
	vec4 specular = specularStrength * spec * vec4(specularlightColor,1.0);

	//distance
	float dist = distance(lightPos,FragPos);
	dist = (50-dist)/50;
	dist = max(dist,0.0);
	
	outColor = (ambient + dist * diffuse + dist * spec) * texture(texture1,TexCoord);

}
)glsl";

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);



float obrot = 0;
float dist = 14.5;
int tryb = 2;

bool firstMouse = true;
int ostatniX, ostatniY;
float yaw = -90;
float pitch = 0;

void kostka(int buffer) {
	int punkty = 36;

	float vertices[] = {


	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  -1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  -0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  -0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  -0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f


	};

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * punkty * 8, vertices, GL_STATIC_DRAW);
}

void ustawKamereKlawisze(GLint _uniView, float czas) {

	float cameraSpeed = 0.000002f * czas;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		cameraPos += cameraSpeed * cameraFront;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		//Przesun w lewo
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		//obrot -= cameraSpeed;
		//cameraFront.x = sin(obrot);
		//cameraFront.z = -cos(obrot);

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		//Przesun w prawo
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		//obrot += cameraSpeed;
		//cameraFront.x = sin(obrot);
		//cameraFront.z = -cos(obrot);
	}

	glm::mat4 view;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glUniformMatrix4fv(_uniView, 1, GL_FALSE, glm::value_ptr(view));

}

void ustawKamereMysz(GLint _uniView, float elapsedTime, const sf::Window& window) {
	sf::Vector2i localPosition = sf::Mouse::getPosition(window);
	sf::Vector2i position;

	//Jakaœ zmienna do relokacji by sie przydala
	bool relokacja = false;

	//Ograniczenie ruchu kamery do okienka
	//Dla wspolrzednej X
	if (localPosition.x <= 0) {
		position.x = window.getSize().x;
		position.y = localPosition.y;
		relokacja = true;
	}
	if (localPosition.x >= window.getSize().x - 1) {
		position.x = 0;
		position.y = localPosition.y;
		relokacja = true;
	}
	//Dla wspolrzednej Y
	if (localPosition.y <= 0) {
		position.y = window.getSize().y;
		position.x = localPosition.x;
		relokacja = true;
	}
	if (localPosition.y >= window.getSize().y - 1) {
		position.y = 0;
		position.x = localPosition.x;
		relokacja = true;
	}
	if (relokacja) {
		sf::Mouse::setPosition(position, window);
		firstMouse = true;
	}
	localPosition = sf::Mouse::getPosition(window);
	if (firstMouse) {
		ostatniX = localPosition.x;
		ostatniY = localPosition.y;
		firstMouse = false;
	}
	float xoffset = localPosition.x - ostatniX;
	float yoffset = localPosition.y - ostatniY;
	ostatniX = localPosition.x;
	ostatniY = localPosition.y;

	float czulosc = 0.001f;
	float szybkoscKamery = 0.002f * elapsedTime;
	xoffset *= czulosc;
	yoffset *= czulosc;
	yaw += xoffset * szybkoscKamery;
	pitch -= yoffset * szybkoscKamery;

	//Ograniczenie katowe
	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

	glm::mat4 view;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glUniformMatrix4fv(_uniView, 1, GL_FALSE, glm::value_ptr(view));
}

void StereoProjection(GLuint shaderProgram_, float _left, float _right, float _bottom, float _top, float _near, float _far, float _zero_plane, float _dist, float _eye)
{
	//    Perform the perspective projection for one eye's subfield.
	//    The projection is in the direction of the negative z-axis.
	//            _left=-6.0;
	//            _right=6.0;
	//            _bottom=-4.8;
	   //             _top=4.8;
	//    [default: -6.0, 6.0, -4.8, 4.8]
	//    left, right, bottom, top = the coordinate range, in the plane of zero parallax setting,
	//         which will be displayed on the screen.
	//         The ratio between (right-left) and (top-bottom) should equal the aspect
	//    ratio of the display.
	   //                  _near=6.0;
	   //                  _far=-20.0;
	//    [default: 6.0, -6.0]
	//    near, far = the z-coordinate values of the clipping planes.
	   //                  _zero_plane=0.0;
	//    [default: 0.0]
	//    zero_plane = the z-coordinate of the plane of zero parallax setting.
	//    [default: 14.5]
	  //                     _dist=10.5;
	//   dist = the distance from the center of projection to the plane of zero parallax.
	//    [default: -0.3]
	  //                 _eye=-0.3;
	//    eye = half the eye separation; positive for the right eye subfield,
	//    negative for the left eye subfield.

	float   _dx = _right - _left;
	float   _dy = _top - _bottom;

	float   _xmid = (_right + _left) / 2.0;
	float   _ymid = (_top + _bottom) / 2.0;

	float   _clip_near = _dist + _zero_plane - _near;
	float   _clip_far = _dist + _zero_plane - _far;

	float  _n_over_d = _clip_near / _dist;

	float   _topw = _n_over_d * _dy / 2.0;
	float   _bottomw = -_topw;
	float   _rightw = _n_over_d * (_dx / 2.0 - _eye);
	float   _leftw = _n_over_d * (-_dx / 2.0 - _eye);

	// Create a fustrum, and shift it off axis
	glm::mat4 proj = glm::frustum(_leftw, _rightw, _bottomw, _topw, _clip_near, _clip_far);

	proj = glm::translate(proj, glm::vec3(-_xmid - _eye, -_ymid, 0));

	GLint uniProj = glGetUniformLocation(shaderProgram_, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
}

//funkcja ladujaca plik .obj do pamieci programu. Na razie obsluguje tylko wczytywanie samego modelu
void loadModelFromOBJFile(const char* nazwaPliku, GLuint vertexArrayObject, GLuint vertexBufferObject, GLuint elementBufferObject, int& punkty) {

	//Obsluguje na ten moment tylko wczytywanie wierzcholkow i trojkatow.
	//Dodac wiecej zmiennych, jesli trzeba bedzie wczytac wiecej danych.

	int indeksWierzcholkow = 0; //v
	int indeksTrojkatow = 0; //f

	int iloscWierzcholkow = 0;
	int iloscTrojkatow = 0;

	std::fstream plik;
	plik.open(nazwaPliku);
	if (!plik.is_open()) {
		std::cout << "Blad podczas ladowania pliku .obj " << std::endl;
		return;
	}
	std::string stringOutput;

	if (plik.is_open()) {
		while (!plik.eof()) {
			plik >> stringOutput;

			//Dodac kolejne if'y jesli wczytujemy wiecej danych.
			if (stringOutput == "v") {
				indeksWierzcholkow++;
			}
			if (stringOutput == "f") {
				indeksTrojkatow++;
			}
		}
	}
	plik.close();
	plik.open(nazwaPliku);

	//dane dla wierzcholkow MUSZA byc typu float!
	GLfloat* vertex = new GLfloat[indeksWierzcholkow * 3];

	//Dane dla trojkatow MUSZA byc typu int! Zmiana na float spowoduje blad przy rysowaniu obiektu.
	//W pliku .obj widac nawet, iz dane to liczby calkowite
	GLint* triangle = new GLint[indeksTrojkatow * 3];

	if (plik.is_open()) {
		while (!plik.eof()) {
			plik >> stringOutput;

			//Dodac kolejne if'y jesli wczytujemy wiecej danych.
			if (stringOutput == "v") {
				plik >> vertex[iloscWierzcholkow];
				vertex[iloscWierzcholkow] -= 1;
				plik >> vertex[iloscWierzcholkow + 1];
				vertex[iloscWierzcholkow] -= 1;
				plik >> vertex[iloscWierzcholkow + 2];
				vertex[iloscWierzcholkow] -= 1;
				iloscWierzcholkow += 3;
			}
			if (stringOutput == "f") {
				plik >> triangle[iloscTrojkatow];
				plik >> triangle[iloscTrojkatow + 1];
				plik >> triangle[iloscTrojkatow + 2];
				iloscTrojkatow += 3;
			}
			stringOutput.clear();
		}
	}
	plik.close();

	//do vao
	//glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	//do vbo
	//glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * indeksWierzcholkow * 3, vertex, GL_STATIC_DRAW);

	//do ebo
	//glGenBuffers(1, &elementBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * indeksTrojkatow * 3, triangle, GL_STATIC_DRAW);

	punkty = indeksTrojkatow * 3;//do wygenerowania obiektow w gl

	//Wyswietlenie danych do debugowania z Blenderem
	std::cout << "Indeks wierzcholkow: " << indeksWierzcholkow << std::endl;
	std::cout << "Indeks trojkatow: " << indeksTrojkatow << std::endl;
	std::cout << "Ilosc wierzcholkow: " << iloscWierzcholkow << std::endl;
	std::cout << "Ilosc danych do trojkatow: " << iloscTrojkatow << std::endl;

	delete[] vertex;
	delete[] triangle;
}

//funkcja ladujaca plik .obj z zajec. Wieksze wsparcie wczytywanych danych ni¿ funkcja "loadModelFromOBJFile".
bool LoadModelOBJNormalsCoord(int& punkty_, const char* filename, int buffer, int* size)
{
	int vert_num = 0;
	int triangles = 0;
	int normals = 0;
	int coord_num = 0;
	//int size_num = 0;



	std::ifstream myReadFile;
	myReadFile.open(filename);
	std::string output;
	if (myReadFile.is_open()) {
		while (!myReadFile.eof()) {
			myReadFile >> output;
			if (output == "v") vert_num++;
			if (output == "f") triangles++;
			if (output == "vn") normals++;
			if (output == "vt") coord_num++;
			//if (output == "o") size_num++;
		}
	}

	myReadFile.close();
	myReadFile.open(filename);

	//std::cout << "ilosc obiektow = " << size_num << std::endl;

	//size[size_size] = *new int;
	float** vert;
	vert = new float* [vert_num]; //przydzielenie pamiêci na w wierszy

	for (int i = 0; i < vert_num; i++)
		vert[i] = new float[3];


	int** trian;
	trian = new int* [triangles]; //przydzielenie pamiêci na w wierszy

	for (int i = 0; i < triangles; i++)
		trian[i] = new int[9];

	float** norm;
	norm = new float* [normals]; //przydzielenie pamiêci na w wierszy

	for (int i = 0; i < normals; i++)
		norm[i] = new float[3];

	float** coord;
	coord = new float* [coord_num]; //przydzielenie pamiêci na w wierszy

	for (int i = 0; i < coord_num; i++)
		coord[i] = new float[2];

	int licz_vert = 0;
	int licz_triang = 0;
	int licz_normals = 0;
	int licz_coord = 0;
	int object = 0;
	int tmp = 0;
	bool first = 1;

	while (!myReadFile.eof()) {
		output = "";
		myReadFile >> output;
		if (output == "vn") { myReadFile >> norm[licz_normals][0]; myReadFile >> norm[licz_normals][1]; myReadFile >> norm[licz_normals][2]; licz_normals++; }

		if (output == "vt") { myReadFile >> coord[licz_coord][0]; myReadFile >> coord[licz_coord][1]; licz_coord++; }
		if (output == "o" && !first)
		{
			size[object] = tmp * 3;
			tmp = 0;
			object++;
		}
		if (output == "v") {
			//tmp++;
			myReadFile >> vert[licz_vert][0];
			myReadFile >> vert[licz_vert][1];
			myReadFile >> vert[licz_vert][2];
			licz_vert++;
		}
		if (output == "f") {
			first = 0;
			tmp++;
			for (int i = 0; i < 9; i += 3)
			{
				std::string s;
				myReadFile >> s;
				std::stringstream ss(s);

				std::vector <std::string> el;
				std::string item;


				while (getline(ss, item, '/')) {
					el.push_back(item);
				}
				trian[licz_triang][i] = std::stoi(el[0]);
				trian[licz_triang][i + 1] = std::stoi(el[1]);
				trian[licz_triang][i + 2] = std::stoi(el[2]);


			}
			licz_triang++;
		}
	}
	size[object] = tmp * 3;
	tmp = 0;
	object++;

	GLfloat* vertices = new GLfloat[triangles * 24];

	int vert_current = 0;

	for (int i = 0; i < triangles; i++)
	{
		vertices[vert_current] = vert[trian[i][0] - 1][0];
		vertices[vert_current + 1] = vert[trian[i][0] - 1][1];
		vertices[vert_current + 2] = vert[trian[i][0] - 1][2];
		vertices[vert_current + 3] = norm[trian[i][2] - 1][0];
		vertices[vert_current + 4] = norm[trian[i][2] - 1][1];
		vertices[vert_current + 5] = norm[trian[i][2] - 1][2];
		vertices[vert_current + 6] = coord[trian[i][1] - 1][0];
		vertices[vert_current + 7] = coord[trian[i][1] - 1][1];

		vertices[vert_current + 8] = vert[trian[i][3] - 1][0];
		vertices[vert_current + 9] = vert[trian[i][3] - 1][1];
		vertices[vert_current + 10] = vert[trian[i][3] - 1][2];
		vertices[vert_current + 11] = norm[trian[i][5] - 1][0];
		vertices[vert_current + 12] = norm[trian[i][5] - 1][1];
		vertices[vert_current + 13] = norm[trian[i][5] - 1][2];
		vertices[vert_current + 14] = coord[trian[i][4] - 1][0];
		vertices[vert_current + 15] = coord[trian[i][4] - 1][1];

		vertices[vert_current + 16] = vert[trian[i][6] - 1][0];
		vertices[vert_current + 17] = vert[trian[i][6] - 1][1];
		vertices[vert_current + 18] = vert[trian[i][6] - 1][2];
		vertices[vert_current + 19] = norm[trian[i][8] - 1][0];
		vertices[vert_current + 20] = norm[trian[i][8] - 1][1];
		vertices[vert_current + 21] = norm[trian[i][8] - 1][2];
		vertices[vert_current + 22] = coord[trian[i][7] - 1][0];
		vertices[vert_current + 23] = coord[trian[i][7] - 1][1];

		vert_current += 24;
	}

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * triangles * 24, vertices, GL_STATIC_DRAW);

	punkty_ = triangles * 3;

	delete vertices;

	//Wyswietlenie danych do debugowania z Blenderem
	std::cout << "Indeks wierzcholkow: " << vert_num << std::endl;
	std::cout << "Indeks trojkatow: " << triangles << std::endl;
	std::cout << "Ilosc wierzcholkow: " << licz_vert << std::endl;
	std::cout << "Ilosc danych do trojkatow: " << licz_triang << std::endl;


	for (int i = 0; i < vert_num; i++)
		delete[] vert[i];
	delete[] vert;

	for (int i = 0; i < triangles; i++)
		delete[] trian[i];
	delete[] trian;

	for (int i = 0; i < normals; i++)
		delete[] norm[i];
	delete[] norm;

	for (int i = 0; i < coord_num; i++)
		delete[] coord[i];
	delete[] coord;

	return 0;
}

int main()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;

	// Okno renderingu
	sf::Window window(sf::VideoMode(800, 800, 32), "OpenGL", sf::Style::Titlebar | sf::Style::Close, settings);
	//sf::Window window(sf::VideoMode(1920,1060,32), "OpenGL", sf::Style::Fullscreen | sf::Style::Close, settings);

	window.setMouseCursorVisible(false);
	window.setMouseCursorGrabbed(true);

	// Inicjalizacja GLEW
	glewExperimental = GL_TRUE;
	glewInit();


	// Utworzenie VAO (Vertex Array Object) - odkomentowac w wypadku tworzenia modelu recznie.
	GLuint vao;
	glGenVertexArrays(1, &vao);
	/*
	glBindVertexArray(vao);
	*/


	// Utworzenie VBO (Vertex Buffer Object)
	// i skopiowanie do niego danych wierzcho³kowych
	//odkomentowac w wypadku tworzenia modelu recznie.
	GLuint vbo;
	glGenBuffers(1, &vbo);


	// Utworzenie EBO (Element Buffer Object) - do zadanka z importowaniem modelu
	// Zakomentowac w przypadku tworzenia modelu recznie.
	GLuint ebo;
	glGenBuffers(1, &ebo);

	/*
	//Tablica ze wspolrzednymi dla wierzcholkow trojkata
	GLfloat vertices[] = {
	0.0f, 0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, 0.0f, 0.0f, 1.0f
	};
	*/


	//Wywolanie okregu dla wczesniejszego zadania
	//okrag(ILOSC_WIERZCHOLKOW, vbo);

	//Funkcja tworzaca kostke. Odkomentowac w wypadku tworzenia modelu recznie
	//kostka(vbo);

	int punkty = 0;

	int* size = new int[8];

	//Funkcja ³aduj¹ca model do programu. - same wierzcholki
	//loadModelFromOBJFile("scene.obj", vao, vbo, ebo, punkty);
	//Wywo³anie funkcji drugiej
	LoadModelOBJNormalsCoord(punkty, "testflip.obj", vbo, size);
	//grupa - test

	// Utworzenie i skompilowanie shadera wierzcho³ków
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	GLint vertexStatus;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexStatus);
	char vertexInfoLog[256];

	if (!vertexStatus) {
		glGetShaderInfoLog(vertexShader, 256, NULL, vertexInfoLog);
		std::cout << "ERROR IN VERTEX SHADER: \n" << vertexInfoLog << std::endl;
	}
	else {
		std::cout << "vertexShader skompilowany poprawnie." << std::endl;
	}

	// Utworzenie i skompilowanie shadera fragmentów
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	GLint fragmentStatus;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentStatus);
	char fragmentInfoLog[256];

	if (!fragmentStatus) {
		glGetShaderInfoLog(fragmentShader, 256, NULL, fragmentInfoLog);
		std::cout << "ERROR IN FRAGMENT SHADER: \n" << fragmentInfoLog << std::endl;
	}
	else {
		std::cout << "fragmentShader skompilowany poprawnie." << std::endl;
	}

	// Zlinkowanie obu shaderów w jeden wspólny program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// Przed uruchomieniem programu dodatkowa weryfikacja

	GLint success;
	char programInfoLog[256];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(shaderProgram, 256, NULL, programInfoLog);
		std::cout << "PROGRAM HAS ENCOUNTERED AN ERROR: \n" << programInfoLog << std::endl;
	}
	else {
		std::cout << "Program skompilowany poprawnie." << std::endl;
	}



	// Specifikacja formatu danych wierzcho³kowych
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	//glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0); //wersja dla funkcji wczytujacej same wierzcholki
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0); //wersja dla funkcji wczytujacej dane o mapowaniu tekstury

	GLint NorAttrib = glGetAttribLocation(shaderProgram, "aNormal");
	glEnableVertexAttribArray(NorAttrib);
	glVertexAttribPointer(NorAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	//GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	//glEnableVertexAttribArray(colAttrib);
	//glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	GLint TexCoord = glGetAttribLocation(shaderProgram, "aTexCoord");
	glEnableVertexAttribArray(TexCoord);
	glVertexAttribPointer(TexCoord, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	glm::vec3 lightPos(2.2f, 2.0f, 2.0f);
	GLint uniLightPos = glGetUniformLocation(shaderProgram, "lightPos");
	glUniform3fv(uniLightPos, 1, glm::value_ptr(lightPos));
	GLint uniCamPos = glGetUniformLocation(shaderProgram, "camPos");
	glUniform3fv(uniCamPos, 1, glm::value_ptr(cameraPos));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	GLint uniTrans = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(model));

	/*
	glm::mat4 view;
	view = glm::lookAt(
		glm::vec3(0, 0, 5),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	*/
	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	/*
	glUniformMatrix4fv(uniView, 1 ,GL_FALSE, glm::value_ptr(view));
	*/

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.06f, 100.0f);
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));


	GLint primitive = GL_LINES; //domyœlne ustawienie rysowania, to rysowanie za pomoc¹ trójk¹tów
	GLint mouse_x = 0;
	GLint mouse_y = 0;
	GLint points = 0;
	// Rozpoczêcie pêtli zdarzeñ
	bool running = true;
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	sf::Clock zegar;
	sf::Time czas;

	window.setFramerateLimit(60);

	//Czesc kodu odpowiedzialna za wczytywanie tekstur i operacje na nich. Odkomentuj potem.


	//Potrzeba wczytac nastepujace tekstury:
	// 1. Tekstura dla malpki
	// 2. Tekstura stolika
	// 3. Tekstura dla czterech krzesel
	// 4. Tekstura dla torusa

	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("Kula1.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture." << std::endl;
	}
	stbi_image_free(data);

	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("Kula2.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture." << std::endl;
	}
	stbi_image_free(data);
	//obraz.bmp

	unsigned int texture3;
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("blank.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture." << std::endl;
	}
	stbi_image_free(data);

	unsigned int texture4;
	glGenTextures(1, &texture4);
	glBindTexture(GL_TEXTURE_2D, texture4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("Torus_texture_diffuse.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture." << std::endl;
	}
	stbi_image_free(data);


	glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture3"), 2);
	glUniform1i(glGetUniformLocation(shaderProgram, "texture4"), 3);

	int suma = 0;
	int licznik = 0;
	while (running) {
		czas = zegar.restart();
		licznik++;
		float maxKlatki = 1000000 / czas.asMicroseconds();
		if (licznik > maxKlatki) {
			window.setTitle(std::to_string(maxKlatki));
			licznik = 0;
		}

		sf::Event windowEvent;
		while (window.pollEvent(windowEvent)) {
			switch (windowEvent.type) {
			case sf::Event::Closed:
				running = false;
				break;

			case sf::Event::KeyPressed:
				switch (windowEvent.key.code) {
				case sf::Keyboard::Escape:
					running = false;
					break;

				case sf::Keyboard::Num1:
					primitive = GL_POINTS;
					break;

				case sf::Keyboard::Num2:
					primitive = GL_LINES;
					break;

				case sf::Keyboard::Num3:
					primitive = GL_LINE_STRIP;
					break;

				case sf::Keyboard::Num4:
					primitive = GL_LINE_LOOP;
					break;

				case sf::Keyboard::Num5:
					primitive = GL_TRIANGLES;
					break;

				case sf::Keyboard::Num6:
					primitive = GL_TRIANGLE_STRIP;
					break;

				case sf::Keyboard::Num7:
					primitive = GL_TRIANGLE_FAN;
					break;

				case sf::Keyboard::Num8:
					primitive = GL_QUADS;
					break;

				case sf::Keyboard::Num9:
					primitive = GL_QUAD_STRIP;
					break;

				case sf::Keyboard::Num0:
					primitive = GL_POLYGON;
					break;
				case sf::Keyboard::G:
					dist += 0.01;
					std::cout << "Dist: " << dist << std::endl;
					break;
				case sf::Keyboard::H:
					dist -= 0.01;
					std::cout << "Dist: " << dist << std::endl;
					break;
				case sf::Keyboard::V:
					tryb = 0;
					break;
				case sf::Keyboard::B:
					tryb = 1;
					break;
				case sf::Keyboard::N:
					tryb = 2;
					break;
				}


				/*
			case sf::Event::MouseMoved:
				if (windowEvent.mouseMove.y > mouse_y) {
					points++;
					mouse_y = windowEvent.mouseMove.y;
				}
				else {
					points--;
					mouse_y = windowEvent.mouseMove.y;
				}
				okrag(points, vbo);
				std::cout << points << std::endl;
				break;*/

			case sf::Event::MouseMoved:
				ustawKamereMysz(uniView, czas.asMicroseconds(), window);
				break;

			}



		}

		ustawKamereKlawisze(uniView, czas.asMicroseconds());
		//ustawKamere(uniView);
		// Nadanie scenie koloru czarnego
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		switch (tryb) {
			//Jesli nie wczytujesz modelu z pliku, to musisz odkomentowac glDrawArrays i zakomentowac glDrawElements.
		case 0:
			glViewport(0, 0, window.getSize().x, window.getSize().y);
			glDrawBuffer(GL_BACK_LEFT);
			StereoProjection(shaderProgram, -6, 6, -4.8, 4.8, 12.99, -100, 0, dist, -0.05);
			glColorMask(true, false, false, false);
			//glDrawArrays(primitive, 0, 24);
			glDrawElements(primitive, punkty, GL_UNSIGNED_INT, 0);

			glClear(GL_DEPTH_BUFFER_BIT);
			glDrawBuffer(GL_BACK_RIGHT);
			StereoProjection(shaderProgram, -6, 6, -4.8, 4.8, 12.99, -100, 0, dist, 0.05);
			glColorMask(false, false, true, false);
			//glDrawArrays(primitive, 0, 24);
			glDrawElements(primitive, punkty, GL_UNSIGNED_INT, 0);
			glColorMask(true, true, true, true);
			//window.display();

			break;
		case 1:
			glViewport(0, 0, window.getSize().x / 2, window.getSize().y);
			StereoProjection(shaderProgram, -6, 6, -4.8, 4.8, 12.99, -100, 0, 13, -0.05);
			//glDrawArrays(primitive, 0, 24);
			glDrawElements(primitive, punkty, GL_UNSIGNED_INT, 0);

			glViewport(window.getSize().x / 2, 0, window.getSize().x / 2, window.getSize().y);
			StereoProjection(shaderProgram, -6, 6, -4.8, 4.8, 12.99, -100, 0, 13, 0.05);
			//glDrawArrays(primitive, 0, 24);
			glDrawElements(primitive, punkty, GL_UNSIGNED_INT, 0);
			//window.display();

			break;
		case 2:
			suma = 0;
			glActiveTexture(GL_TEXTURE0);
			glViewport(0, 0, window.getSize().x, window.getSize().y);


			glBindTexture(GL_TEXTURE_2D, texture2);
			glDrawArrays(primitive, suma, size[0]);
			std::cout << size[0] << std::endl;
			suma += size[0];


			glBindTexture(GL_TEXTURE_2D, texture1);
			glDrawArrays(primitive, suma, size[1]);
			std::cout << size[1] << std::endl;
			suma += size[1];

			glBindTexture(GL_TEXTURE_2D, texture3);
			//glBindTexture(GL_TEXTURE_2D, texture3);
			glDrawArrays(primitive, suma, size[2]);
			std::cout << size[2] << std::endl;
			//suma += size[2];
			/*
			//glBindTexture(GL_TEXTURE_2D, texture3);
			glDrawArrays(primitive, suma, size[3]);
			std::cout << size[3] << std::endl;
			suma += size[3];

			//glBindTexture(GL_TEXTURE_2D, texture3);
			glDrawArrays(primitive, suma, size[4]);
			std::cout << size[4] << std::endl;
			suma += size[4];

			glBindTexture(GL_TEXTURE_2D, texture1);
			glDrawArrays(primitive, suma, size[5]);
			std::cout << size[5] << std::endl;
			suma += size[5];


			glBindTexture(GL_TEXTURE_2D, texture4);
			glDrawArrays(primitive, suma, size[6]);
			std::cout << size[6] << std::endl;

			//glBindTexture(GL_TEXTURE_2D, texture1);
			//glDrawArrays(primitive, 0, punkty);
			//glDrawElements(primitive, punkty, GL_UNSIGNED_INT, 0);
			//window.display();
			*/
			std::cout << "\nOstateczna ilosc wierzcholkow: " << suma << std::endl;
			break;
		default:
			glDrawBuffer(GL_BACK_LEFT);
			StereoProjection(shaderProgram, -6, 6, -4.8, 4.8, 12.99, -100, 0, 13, -0.1);
			glColorMask(true, false, false, false);
			//glDrawArrays(primitive, 0, 36);
			glDrawElements(primitive, punkty, GL_UNSIGNED_INT, 0);

			glDrawBuffer(GL_BACK_RIGHT);
			glClear(GL_DEPTH_BUFFER_BIT);
			StereoProjection(shaderProgram, -6, 6, -4.8, 4.8, 12.99, -100, 14.5, 13, 0.1);
			glColorMask(false, false, true, false);
			//glDrawArrays(primitive, 0, 36);
			glDrawElements(primitive, punkty, GL_UNSIGNED_INT, 0);
			glColorMask(true, true, true, true);
			break;
		}




		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture1);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, texture2);
		/*

		window.display();



		*/
		//glViewport(0, 0, screenwidth, screenheight);

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(primitive, 0, 36);
		/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glDrawArrays(GL_TRIANGLES, 0, 12);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glDrawArrays(GL_TRIANGLES, 12, 24);
		*/
		// Narysowanie trójk¹ta na podstawie 3 wierzcho³ków
		//glDrawArrays(primitive, 0, 36);

		// Wymiana buforów tylni/przedni
		window.display();

	}
	// Kasowanie programu i czyszczenie buforów
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
	// Zamkniêcie okna renderingu
	window.close();
	return 0;
}
