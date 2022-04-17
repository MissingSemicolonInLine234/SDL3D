//this project is optimising polygon filling and removing backface culling bugs
// next project ill turn both 3D drawing and the provious
// functions of polygon filling and line drawing to a header file
// next^2 project ill add multi object capacity

//include section 
//{
#include <SDL.h>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <sstream>
#include <ctime>
#include <chrono>
// some might say that this level of random includes is inefficient
// to them i say
// suck on a tailpipe 
//}
//Const delcaration
//{
//x,y,z, xrot,yrot,zrot
std::vector<bool> offscreen;
std::vector<int> lengths;				// stores lengths of the different sections;
const int secDi[][2]={					// stores dimentions of sections and types of sections
	{3,1},								// 0=unsigned int, 1=float, 2=string
	{2,0},
	{3,0}
	};
const int SCREEN_WIDTH = 700;			//screen width
const int SCREEN_HEIGHT = 700;			//screen height
std::vector<std::array<int,2>> point;	//stores the points as drawn on screen
std::vector<std::array<float,3>> point3d;	//stores modified (by transformations) object points in 3d space
std::vector<std::array<float,3>> objPoint;	//stores the original object points
std::vector<std::array<int, 2>> lines;	//stores which points need to have lines drawn to them
std::vector<std::array<int, 3>> polygons;	//stores which points need to have lines drawn to them
float fov = 400;
float cameraSpeed = 0.3;
const float Pi = 3.14159265;
float camPos[6] ={0,0,0,0,0,0};
float pos[6]={0,0,3, 0, 0, 0};			//the x,y,z position and then x,y,z rotation

auto start = std::chrono::high_resolution_clock::now();
const int fpsCap = 30;
int fpsCapDelay=0;
const int dither = 2;

std::string charId;		//stores the Ids of the chars and thuswhat order they are in
std::vector<std::vector <std::array<int, 2>>> charPix;	//stores the Pixel arrays of the chars
//}
//function declaration
//{
void pointDraw (int a, SDL_Renderer* renderer);
void lineDraw (int a, int b, SDL_Renderer* renderer);
void polyDraw (int a, int b, int c, SDL_Renderer* renderer);
void queueClear (SDL_Event event);
void project3d ();
void translate (float x, float y, float z);
void inputHandle (SDL_Event evemt);
bool fileRead (std::string fileName);
void rotate (float xA, float yA, float zA);
bool fontRead (std::string fileName);
void letterDraw(std::string input, int xOff, int yOff, SDL_Renderer* renderer);
std::string to_string(int i);
void fpsDisplay(int count,SDL_Renderer* renderer);
float fps = 10;
//oh boy i looove void functions
//}



int main( int argc, char* args[] ){
	
	std::cout<<"Enter file name of project:\n	-";
	std::string fileName;
	std::cin >> fileName;
	fileName = "Objects\\"+fileName;
	//gets filename
	//SDL initialisation section 
	//{
	SDL_Event event;
	SDL_Renderer *renderer;
	SDL_Window *window;
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
	SDL_RenderClear(renderer);
	//}
	//loads data from file to points3D, ends program if fails
	if(fileRead(fileName)==false){return 1;}
	translate (0,0,10);
	//this is the post initialisation loop
	//all actions other than intialisation and  exit section occur here
	point3d = objPoint;
	//handles the initial handoff from the object points to the transformed points
	fontRead("Fonts\\font1.fnt");
	//reads font to memory
	int count=0;
	while(true){
		project3d();
		/*SDL_SetRenderDrawColor(renderer, 255,255,255,255);
		for(int i=0;i<lengths[0];i++){
			pointDraw(i,renderer);
			
		}*/
		
		SDL_SetRenderDrawColor(renderer, 200, 255, 200,225);
		for(int i=0;i<lengths[1];i++){
			if((offscreen[lines[i][0]]==false) or (offscreen[lines[i][1]]==false)){
				lineDraw(lines[i][0],lines[i][1],renderer);
			}
		}
		
		int cols [4]={100,150,200,250};//temporary#!#!
		SDL_SetRenderDrawColor(renderer, 70, 150, 70,225);
		for(int i=0;i<lengths[2];i++){
			if ((offscreen[polygons[i][0]]==false)or(offscreen[polygons[i][1]]==false)or(offscreen[polygons[i][2]]==false)){
				if(i==2){
					SDL_SetRenderDrawColor(renderer,255,255,255,255);
					letterDraw("A",point[polygons[i][0]][0],point[polygons[i][0]][1],renderer);
					letterDraw("B",point[polygons[i][1]][0],point[polygons[i][1]][1],renderer);
					letterDraw("C",point[polygons[i][2]][0],point[polygons[i][2]][1],renderer);
				}
				SDL_SetRenderDrawColor(renderer, cols[(i)%4], cols[(i%5)%4], cols[(i%6)%4],225);
				polyDraw(polygons[i][0],polygons[i][1],polygons[i][2],renderer);
			}
		}
		fpsDisplay(count,renderer);
		
		SDL_RenderPresent(renderer);
		queueClear(event);
		SDL_SetRenderDrawColor(renderer,0,0,0,255);
		SDL_RenderClear(renderer);
		count++;
	}//while terminates
	SDL_DestroyRenderer(renderer);
} // main terminates


//Draws the pair of point to screen, includes a reticule.
//Point declared in 2d array Point [][2]
void pointDraw (int a,SDL_Renderer *renderer){
	SDL_RenderDrawPoint(renderer, point[a][0], point[a][1]);
	//displays the point
	SDL_RenderDrawPoint(renderer, point[a][0]+2, point[a][1]+2);
	SDL_RenderDrawPoint(renderer, point[a][0]-2, point[a][1]+2);
	SDL_RenderDrawPoint(renderer, point[a][0]+2, point[a][1]-2);
	SDL_RenderDrawPoint(renderer, point[a][0]-2, point[a][1]-2);
	//displays the x shape around the point
	SDL_RenderDrawPoint(renderer, point[a][0], point[a][1]+3);
	SDL_RenderDrawPoint(renderer, point[a][0], point[a][1]-3);
	SDL_RenderDrawPoint(renderer, point[a][0]+3, point[a][1]);
	SDL_RenderDrawPoint(renderer, point[a][0]-3, point[a][1]);
	//displays the cross around the point
};

//Draws the line between two point
//Point declared in 2d array Point[][2]
void lineDraw (int a, int b, SDL_Renderer* renderer){
	
	float grad;
	if (((point[a][0]-point[b][0])*1.0)==0){
		grad = 10000000;
	}else{
		grad= ((point[a][1]-point[b][1])*1.0)/((point[a][0]-point[b][0])*1.0);
	}
	//calculates the gradient between the two point using the y = gx+c  format where grad = g
	//the if statement handles the exception case where the gradient is infinite because the line goes streight up
	float offs = (point[a][1]-grad*point[a][0]);
	//calculates the offset between the line y = gc and the origin using the format y = gx+c
	//where offs = c
	
	//sets boundries, this stops the line from rendering outside of the screen
	//{
	int bounds [2][2]={
		{std::min(point[a][0],point[b][0]),std::min(point[a][1],point[b][1])},
		{std::max(point[a][0],point[b][0]),std::max(point[a][1],point[b][1])}
	};
	
	if (bounds[0][0] < 0){bounds[0][0]=0;}
	if (bounds[0][1] < 0){bounds[0][1]=0;}
	if (bounds[1][0] > SCREEN_WIDTH){bounds[1][0]=SCREEN_WIDTH;}
	if (bounds[1][1] > SCREEN_HEIGHT){bounds[1][1]=SCREEN_HEIGHT;}
	//}
	
	for (int i = 0; i< bounds[1][0]-bounds[0][0]-1; i++){
	
		SDL_RenderDrawPoint(renderer,
		i+bounds[0][0],
		(i+bounds[0][0])*grad+offs);
		//draws the line between the two point using grad and offs.
		//draws using the x axis pixels as quanta (i.e. there is one pixel drawn along each unit of the x axis)
	}
	
	for (int i = 0; i< bounds[1][1]-bounds[0][1]-1; i++){
		SDL_RenderDrawPoint(renderer,
		((i+bounds[0][1]-offs)/grad),
		i+bounds[0][1]);
		//draws the line between the two point using grad and offs.
		//draws using the y axis pixels as quanta
	}
}

//draws a filled polygon between the three points
//point declared as a [][3][2] array
void polyDraw (int a, int b, int c, SDL_Renderer* renderer){
	//this is the x boundry within which the polygon sits.
	int bounds[2]={
		std::min(std::min(point[a][0],point[b][0]),point[c][0]),
		std::max(std::max(point[a][0],point[b][0]),point[c][0]),
	};
	
	//Calculate the three gradients and offsets of the boundry lines
	//line AB
	//{
	float gradA;
	if (((point[a][0]-point[b][0])*1.0)==0){
		gradA = 1000000000;
	}else{
		gradA= ((point[a][1]-point[b][1])*1.0)/((point[a][0]-point[b][0])*1.0);
	}
	float offsA = (point[a][1]-gradA*point[a][0]);
	//see lineDraw funct for more detail.
	//}
	
	//handles backface culling
	//{
	if	(point[a][0]==point[b][0]){ 			//handles div 0 case
		if(point[c][0]<point[a][0]){
			return;
		}
	}else if (((point[c][1] > point[c][0]*gradA+offsA)&(point[a][0]>point[b][0]))or
	((point[c][1] < point[c][0]*gradA+offsA)&(point[a][0]<point[b][0])))
	{return;}
	//}
	
	//line BC
	//{
	float gradB;
	if (((point[b][0]-point[c][0])*1.0)==0){
		gradB = 1000000000;
	}else{
		gradB= ((point[b][1]-point[c][1])*1.0)/((point[b][0]-point[c][0])*1.0);
	}
	float offsB = (point[b][1]-gradB*point[b][0]);
	//see lineDraw funct for more detail.
	//}
	//line CA
	//{
	float gradC;
	if (((point[c][0]-point[a][0])*1.0)==0){
		gradC = 1000000000;
	}else{
		gradC = ((point[c][1]-point[a][1])*1.0)/((point[c][0]-point[a][0])*1.0);
	}
	float offsC = (point[c][1]-gradC*point[c][0]);
	//see lineDraw funct for more detail.
	//}
	
	//calculate which way the boundry lines need to be flipped
	//flips are true if drawn is above line
	//{
	bool flipA = (point[c][1] > point[c][0]*gradA +offsA);
	bool flipB = (point[a][1] > point[a][0]*gradB +offsB);
	bool flipC = (point[b][1] > point[b][0]*gradC +offsC);
	//}
	
	// handles cases where the boundry if offscreen and so would cause lag
	// does this by making the max bounds at the edge of the screen
	if (bounds[0] < 0){bounds[0]=0;}
	if (bounds[1] > SCREEN_WIDTH){bounds[1]=SCREEN_WIDTH;}
	
	std::vector <int> start;
	std::vector <int> finish;
	
	//calculates the start of the line in the x direction upwards
	//also calculates the finish of the line
	float zero=0;
	if(flipC&flipA){
		for(int i=bounds[0];i<bounds[1];i++){
			start.push_back(std::max(std::max((gradA*i+offsA),(gradC*i+offsC)),zero));
			finish.push_back(std::min(gradB*i+offsB,(float)SCREEN_HEIGHT));
		}
	}else if(flipA&flipB){
		for(int i=bounds[0];i<bounds[1];i++){
			start.push_back(std::max(std::max((gradA*i+offsA),(gradB*i+offsB)),zero));
			finish.push_back(std::min(gradC*i+offsC,(float)SCREEN_HEIGHT));
		}
	}else if(flipB&flipC){
		for(int i=bounds[0];i<bounds[1];i++){
			start.push_back(std::max(std::max((gradB*i+offsB),(gradC*i+offsC)),zero));
			finish.push_back(std::min(gradA*i+offsA,(float)SCREEN_HEIGHT));
		} 
	}else if(flipC){
		for(int i=bounds[0];i<bounds[1];i++){
			start.push_back(std::max(gradC*i+offsC,zero));
			finish.push_back(std::min(std::min(gradA*i+offsA,gradB*i+offsB),(float)SCREEN_HEIGHT));
		}
	}else if(flipA){
		for(int i=bounds[0];i<bounds[1];i++){
			start.push_back(std::max(gradA*i+offsA,zero));
			finish.push_back(std::min(std::min(gradC*i+offsC,gradB*i+offsB),(float)SCREEN_HEIGHT));
		}
	}else if(flipB){
		for(int i=bounds[0];i<bounds[1];i++){
			start.push_back(std::max(gradB*i+offsB,zero));
			finish.push_back(std::min(std::min(gradC*i+offsC,gradA*i+offsA),(float)SCREEN_HEIGHT));
		}
	}
	
	
	for(int i=bounds[0];i<bounds[1];i+=dither){
		for(int j=start[i-bounds[0]];j<finish[i-bounds[0]];j+=dither){
			SDL_RenderDrawPoint(renderer,i,j-(j%dither));
		}
	}
}

//Clears the SQL queue when called.
//this function was made in the previous project but edited to take a better system of inputs
//also now takes key inputs
void queueClear (SDL_Event event){
	while( SDL_PollEvent( &event ) ){   
		switch( event.type ){
			case SDL_QUIT:
				exit(1);
				break;
			case SDL_KEYDOWN:
				inputHandle(event);
				break;
			default:
				break;
		}
	}
}

//handles keyboard input passed on from queueClear
//is only called by queueClear
void inputHandle (SDL_Event event){
	switch (event.key.keysym.sym)
				{
					//this handles WASD movement
					//{
					case SDLK_a:
///						std::cout << "left\n";
						camPos[2]-=cameraSpeed*sin(camPos[4]);
						camPos[0]-=cameraSpeed*cos(camPos[4]);
						break;
					case SDLK_d:
///						std::cout << "right\n";
						camPos[2]+=cameraSpeed*sin(camPos[4]);
						camPos[0]+=cameraSpeed*cos(camPos[4]);
						break;
					case SDLK_w:
///						std::cout << "forward\n";
						camPos[2]+=cameraSpeed*cos(-camPos[4]);
						camPos[0]+=cameraSpeed*sin(-camPos[4]);
						break;
					case SDLK_s:
///						std::cout << "backward\n";
						camPos[2]-=cameraSpeed*cos(-camPos[4]);
						camPos[0]-=cameraSpeed*sin(-camPos[4]);
						break;
					case SDLK_SPACE:
///						std::cout << "up\n";
						camPos[1]+=cameraSpeed;
						break;
					case SDLK_c:
///						std::cout << "down\n";
						camPos[1]-=cameraSpeed;
						break;
					//}
					
					//rotating the object
					//{
					case SDLK_b:
						rotate(0.2,0,0);
						break;
					case SDLK_g:
						rotate(-0.2,0,0);
						break;
					case SDLK_n:
						rotate(0,0.2,0);
						break;
					case SDLK_h:
						rotate(0,-0.2,0);
						break;
					case SDLK_m:
						rotate(0,0,0.2);
						break;
					case SDLK_j:
						rotate(0,0,-0.2);
						break;
					//}
					
					//camera rotation
					//{
					case SDLK_UP:
						camPos[3]+=0.1;
						break;
					case SDLK_DOWN:
						camPos[3]-=0.1;
						break;
					case SDLK_RIGHT:
						camPos[4]-=0.1;
						break;
					case SDLK_LEFT:
						camPos[4]+=0.1;
						break;
					case SDLK_k:
						camPos[5]+=0.1;
						break;
					case SDLK_l:
						camPos[5]-=0.1;
						break;
					//}
					
					// handles exit
					case SDLK_ESCAPE:
						std::cout << "exit\n";
						exit(1);
						break;
						
					// handles fov scaling
					//{
					case SDLK_p:
						fov+=10;
						std::cout << "fov up: " << fov << "\n";
						break;
					case SDLK_o:
						fov+=-10;
						std::cout << "fov down: " << fov << "\n";
						break;
					//}
				}
}

//takes the array points3d and projects it to 2d points 
//uses point3d  and edits point
//also centralises the cube
void project3d(){
	for(int i=0;i<lengths[0];i++){
		float x,y,z;
		z=camPos[3];
		y=camPos[4];
		x=camPos[5];
		float rotationMatrix[3][3]={
			{cos(y)*cos(x)	,	sin(z)*sin(y)*cos(x)+cos(z)*sin(x)	,	-1*cos(z)*sin(y)*cos(x)+sin(z)*sin(x)},
			{-1*cos(y)*sin(x)	,	-1*sin(z)*sin(y)*sin(x)+cos(z)*cos(x)	,	cos(z)*sin(y)*sin(x)+sin(z)*cos(x)},
			{sin(y)	,	-1*sin(z)*cos(y)	,	cos(z)*cos(y)}
		};			//the mathematical rotation matrix
		float adjPoint3d[3]={
			point3d[i][0]-camPos[0]+pos[0],
			point3d[i][1]-camPos[1]+pos[1],
			point3d[i][2]-camPos[2]+pos[2]
		};
		float rotPoint3d[3]={
			adjPoint3d[0]*rotationMatrix[0][0]+adjPoint3d[1]*rotationMatrix[1][0]+adjPoint3d[2]*rotationMatrix[2][0],
			adjPoint3d[0]*rotationMatrix[0][1]+adjPoint3d[1]*rotationMatrix[1][1]+adjPoint3d[2]*rotationMatrix[2][1],
			adjPoint3d[0]*rotationMatrix[0][2]+adjPoint3d[1]*rotationMatrix[1][2]+adjPoint3d[2]*rotationMatrix[2][2]
		};
		if ((rotPoint3d[2]<=0)or(abs(rotPoint3d[0]/rotPoint3d[2])>SCREEN_WIDTH/2)or(abs(rotPoint3d[1]/rotPoint3d[2])>SCREEN_HEIGHT/2)){
			// this handles if theyre too close to the camera and handles the div 0 case
			//also handles when the point is offscreen
			//essentially it is the min draw distance
			offscreen[i]=true;
			point[i][0]=(rotPoint3d[0]/abs(0.001))*fov+(0.5*SCREEN_WIDTH);
			point[i][1]=(rotPoint3d[1]/abs(0.001))*fov*-1+(0.5*SCREEN_HEIGHT);
		}else{
			offscreen[i]=false;
			point[i][0]=(rotPoint3d[0]/abs(rotPoint3d[2]))*fov+(0.5*SCREEN_WIDTH);
			point[i][1]=(rotPoint3d[1]/abs(rotPoint3d[2]))*fov*-1+(0.5*SCREEN_HEIGHT);
		}
///		std::cout << "\nPoint: " << i << " X=" << point[i][0] << " Y=" << point[i][0]; 
	}
}

//translates the 3d object by the specified coordinates
void translate (float x, float y, float z){
	pos[0]+=x;
	pos[1]+=y;
	pos[2]+=z;
}

//rotates object using rotation matrix
void rotate (float xA, float yA, float zA){
	pos[3]+=xA;
	pos[4]+=yA;
	pos[5]+=zA;
	float x,y,z;
	x=pos[3];
	y=pos[4];
	z=pos[5];
	float rotationMatrix[3][3]={
		{cos(y)*cos(z)	,	sin(x)*sin(y)*cos(z)+cos(x)*sin(z)	,	-1*cos(x)*sin(y)*cos(z)+sin(x)*sin(z)},
		{-1*cos(y)*sin(z)	,	-1*sin(x)*sin(y)*sin(z)+cos(x)*cos(z)	,	cos(x)*sin(y)*sin(z)+sin(x)*cos(z)},
		{sin(y)	,	-1*sin(x)*cos(y)	,	cos(x)*cos(y)}
	};			//the mathematical rotation matrix
	for(int i=0;i<lengths[0];i++){
		for(int j=0;j<3;j++){
			point3d[i][j]=
			objPoint[i][0]*rotationMatrix[0][j]+
			objPoint[i][1]*rotationMatrix[1][j]+
			objPoint[i][2]*rotationMatrix[2][j];
		}
	}		//multiplies the two together
	//#!#!#!rotation slowly streaches the shape
	//#!#! rotation includes translation
}

//reads data from a file and sets it to "point"
bool fileRead (std::string fileName){
	//*!*! Need to add return and warning when filename doesnt exist
	std::string data;
	//reads all of file to data
	//{
	std::ostringstream tempData;//i dont know what the fuck an ostringstream is but it works so i aint changing it.
	std::ifstream infile; 
	infile.open(fileName);
	//Checking if file exists
	if(infile.is_open()==false){std::cout<< "ERROR:\n	File not found\nTERMINATING\n"; return false;};
	tempData << infile.rdbuf(); 
	data = tempData.str();
	//}
	//gets rid of comments
	//{
	bool erase = false;
	for(int i =0;i<data.length()-1;i++){
		if(data[i] == '/' & data[i+1] == '*'){
			erase = true;
		}
		if(data[i] == '*' & data[i+1] == '/'){
			erase = false;
			data[i] = ' ';
			data[i+1] = ' ';
		}
		if(erase){
		data[i] = ' ';
		}
	}
	//}
	//Check proper format
	//{
	int lineCount = 1;		//what whitespace line you are on (for error message)
	int numVar = 0;			//how many variables on that code line
	int section = 0;		//what section we are in.
	for(int i =0;i<data.length();i++){
		if(data[i]=='#'){section++;}		// keeps track of the section
		if(data[i]=='\n'){lineCount++;}		// keeps track of the whitespace line
		//#! while this should work it will baloon qite quick if it isnt rewritten
		if((data[i]==' ' or data[i] =='\n' or data[i] == '	' or data[i] ==';')&(data[i-1]!=' ' & data[i-1] !='\n' & data[i-1] != '	' & data[i-1] != '#' & data[i-1] !=';')){
			numVar++; 		//increments to track the number of variables in a group
		}
		//checks number of variables
		if(data[i] ==';'){
			if(numVar == secDi[section][0]){	//checks that the number of variables in the group is correct
				numVar =0;
			}else{
				std::cout << "ERROR IN " << fileName << ":\n";
				std::cout << "	Invalid number of variables in line " << lineCount << ", section " << section+1 << ".\n";
				std::cout << "	Expected " <<secDi[section][0] <<" vaules, received " << numVar <<".\n";
				std::cout <<"	TERMINATING.\n";
				return false;					// triggered if number of variable in incorrect
			}
		}
		//checks type of variable
		if(data[i]!=' ' & data[i] !='\n' & data[i] != '	' & data[i] != '#' & data[i] !=';'){ //#! same comment as last
			if(secDi[section][1]==0){		//checks types for unsigned int
				switch (data[i]){			//#! doesnt check size restriction
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						break;
					default:		//triggers if non int value detected
std::cout << "ERROR IN " << fileName << "\n";
std::cout << "	Invalid character in line " << lineCount << ", section " << section+1 << ".\n";
std::cout << "	Type unsigned int. Values must be integars only.\n";
std::cout << "	Character '" << data[i] << "' not an integar\n";
std::cout << "		(in \"" << data[i-3] << data[i-2] << data[i-1] << data[i] << data [i+1] << data[i+2] << data[i+3] << "\")\n";
std::cout << "		     ~~~^~~~\n";
std::cout << "	TERMINATING\n";
						return false;
						break;
				}
			}
			if(secDi[section][1]==1){		//checks types for float
				switch (data[i]){			//#!doesnt check size restriction or order/number of symbols
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
					case '.':
					case '-':
						break;
					default:			//triggers if non int/number value detected
std::cout << "ERROR IN " << fileName << "\n";
std::cout << "	Invalid character in line " << lineCount << ", section " << section+1 << ".\n";
std::cout << "	Type float. Values must be integars, decimal points or negatives only.\n";
std::cout << "	Character '" << data[i] << "' not an integar\n";
std::cout << "		(in \"" << data[i-3] << data[i-2] << data[i-1] << data[i] << data [i+1] << data[i+2] << data[i+3] << "\")\n";
std::cout << "		     ~~~^~~~\n";
std::cout << "	TERMINATING\n";
						return false;
						break;
				}
			}
		}
	}
	//}
	//counts the length of sections
	//{
	//keeps count of section length
	int count=0;
	//runs through all chars
	for(int i =0;i<data.length();i++){
		switch(data[i]){
			case ';':
				count++;
				break;
			case '#':
				lengths.push_back(count);
				count = 0;
				break;
			default:
				break;
		}
	}
	//}
	//saves values to one long string vector
	//{
	std::vector<std::string> segData; //segmented data
	bool readOn=false;		//this keeps track of whether we are in whitespace or variable
	bool delayReadOn = false;	//used to detect if the state has changed
	for(int i =0;i<data.length();i++){
		if(data[i]==' ' or data[i]=='	' or data[i]=='\n' or data[i]=='#' or data[i]==';'){
			readOn=false; 	//detects if its a non value
		}else{readOn=true;}	//toggles readOn
		if(readOn==true & delayReadOn==false){
			segData.push_back("");
		}
		if(readOn){	//appends current data to the last created element if it is detected to be a value
			segData[segData.size()-1] += data[i];
		}
		delayReadOn = readOn;	//this syncs the delayed and regular readOn
	}
	/*for(int i =0; i< segData.size();i++){
		std::cout << segData[i];
	}*/
	//}
	//converts long string vector into long float vector
	//{
	std::vector<float> floatData;
	int pointOffset;		//this keeps track of whether there has been a point in the number (as this would offset the current power of ten)
	
	for(int i=0; i<segData.size(); i++){
		floatData.push_back(0);
		pointOffset =0;
		for(int j=0; j<segData[i].length();j++){
			if((int)segData[i][segData[i].length()-j-1]>=(int)'0' & (int)segData[i][segData[i].length()-j-1]<=(int)'9'){
				//if a number
				floatData[i]+= ((int)segData[i][segData[i].length()-j-1]-(int)'0')*pow(10,(j-pointOffset));
				//add num by adding in reverse while incrementing in powers of ten
			}else if (segData[i][segData[i].length()-j-1]=='-'){
				floatData[i] = floatData[i]*-1;	//handles negative numbers
			}else if (segData[i][segData[i].length()-j-1]=='.'){
				pointOffset =1+j;		//handles decimal places
				floatData[i] = floatData[i]/pow(10,j);
			}else{
				std::cout << "ERROR:\n";	// handles errors (though at this stage in the code it should be impossible to trigger)
				std::cout << "	Illegal character detected in string to float conversion section\n";
				std::cout << "	Character:\"" << segData[i][segData[i].length()-j-1] << "\" from: " << segData[i] << "\n";
				std::cout << "TERMINATING\n";
				return false;
			}
		}
	}
	/*std::cout << "\n\n\n";
	for(int i=0; i<floatData.size();i++){
		std::cout << floatData[i] << "	";
	}*/
	//}
	//assigns long float vector to the appropriate global variables
	//{
	int posDat=0;							//keeps track of where we are in the data
	for(int i=0; i<lengths[0]*3;i++){	//assigns to objPoint
		if(i%3==0){
			objPoint.push_back({0,0,0});
			point3d.push_back({0,0,0});
			point.push_back({0,0});
			offscreen.push_back(false);
///			std::cout<< "\n";
		}
		objPoint[((i-(i%3))/3)][i%3]=floatData[i];
///		std::cout << floatData[i] << " ";
	}
	posDat+=lengths[0]*3;
	for(int i=posDat; i<(lengths[1]*2)+posDat;i++){	//asigns to lines
		if(i%2==0){
			lines.push_back({0,0});
///			std::cout<< "\n";
		}
		int j=i-posDat;
		lines[((j-(j%2))/2)][j%2]=floatData[i];
///		std::cout << floatData[i+lengths[0]*3] << " ";
	}
	posDat +=lengths[1]*2;
	for(int i=posDat;i<(lengths[2]*3)+posDat;i++){
		if(i%3==0){
			polygons.push_back({0,0,0});
///			std::cout<< "\n";
		}
		int j=i-posDat;
		polygons[((j-(j%3))/3)][j%3]=floatData[i];
	}
	
	//}
	return true;
}

//draws letters to screen using the font stored in CharPix and charId
void letterDraw(std::string input, int xOff, int yOff, SDL_Renderer* renderer){
	for(int i=0;i<input.length();i++){			//repeats for each letter
		int curId;
		if(input[i]==' '){xOff+=6;}else{
		for(int j=0;j<charId.length();j++){
			if(charId[j]==input[i]){curId=j+1;}
		}
		for(int k=0; k<charPix[curId].size();k++){
			SDL_RenderDrawPoint(renderer, charPix[curId][k][0]+xOff, charPix[curId][k][1]+yOff);
		}
		}
		xOff+=6;
	}
}

//like fileRead but handles reading the font
bool fontRead (std::string fileName){
	//*!*! Need to add return and warning when filename doesnt exist
	std::string data;
	//reads all of file to data
	//{
	std::ostringstream tempData;//i dont know what the fuck an ostringstream is but it works so i aint changing it.
	std::ifstream infile; 
	infile.open(fileName);
	//Checking if file exists
	if(infile.is_open()==false){std::cout<< "ERROR:\n	File not found\nTERMINATING\n"; return false;};
	tempData << infile.rdbuf(); 
	data = tempData.str();
	//}
	//std::cout << data;
	int right=0;
	int down=0;
	std::vector<std::array<int, 2>> tempArray;
	for(int i =0;i<data.length();i++){			//fills charId and CharPix
		if(data[i]=='#'){tempArray.push_back({right,down});right++;}
		else if(data[i]=='~'){right++;}
		else if(data[i]=='\n'){right=0;down++;}
		else{down=0;right=0;
		charId+=(data[i]);
		charPix.push_back(tempArray);
		tempArray.clear();}
	}
	charPix.push_back(tempArray);
	/*for(int i=0;i<charPix.size();i++){
		for(int j=0;j<charPix[i].size();j++){
			std::cout<< charPix[i][j][0]
			<< "," <<charPix[i][j][1]<< " ";
		}
		std::cout << "\n";
	}*/
}

//why the fuck doesnt c++ MinGW have to_string
//i hate coding
//yeah i stole this
//fuck you
std::string to_string(int i){
    std::stringstream ss;
    ss<<i;
    return ss.str();
}

//displays the fps to screen
void fpsDisplay(int count,SDL_Renderer* renderer){
	time_t now;
	if(count%20==0){
		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		start = std::chrono::high_resolution_clock::now();
		long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		fps = 10000000/(microseconds*1.0);
		if(fps>fpsCap){
			fpsCapDelay++;
		}else if(fps>fpsCap*2){
			fpsCapDelay+=10;
		}else if(fps<fpsCap*0.75){
			fpsCapDelay=0;
		} else if(fpsCapDelay!=0){
			fpsCapDelay--;
		}
///		std::cout << fps <<" " << fpsCapDelay<<"\n";
	}
	SDL_SetRenderDrawColor(renderer,40,120,40,255);
	for(int i=0;i<fps;i++){
		SDL_RenderDrawPoint(renderer, 30+i*2, 15);
		SDL_RenderDrawPoint(renderer, 31+i*2, 16);
		SDL_RenderDrawPoint(renderer, 30+i*2, 17);
		SDL_RenderDrawPoint(renderer, 31+i*2, 18);
	}
		if(fpsCapDelay!=0){
		SDL_Delay(fpsCapDelay);
		SDL_SetRenderDrawColor(renderer,255,255,255,255);
		letterDraw("FPS capped", 10, 20, renderer);
		SDL_SetRenderDrawColor(renderer,40,120,40,255);
		for(int i=0;i<fpsCapDelay;i++){
			SDL_RenderDrawPoint(renderer, 30+i*2, 25);
			SDL_RenderDrawPoint(renderer, 31+i*2, 26);
			SDL_RenderDrawPoint(renderer, 30+i*2, 27);
			SDL_RenderDrawPoint(renderer, 31+i*2, 28);
		}
	}
	SDL_SetRenderDrawColor(renderer,255,255,255,255);
	SDL_RenderDrawPoint(renderer, 30+fpsCap*2, 15);
	SDL_RenderDrawPoint(renderer, 30+fpsCap*2, 16);
	SDL_RenderDrawPoint(renderer, 30+fpsCap*2, 17);
	SDL_RenderDrawPoint(renderer, 30+fpsCap*2, 18);
	
	
	letterDraw("FPS:",10,10,renderer);
	letterDraw(to_string(fps),33,10,renderer);
}
// and most of all kids, remember this 
//{
// be gay
// do drugs
// heil satan
//}