#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;
using namespace sf;

const float f_Width = 320 * 4;
const float f_Height = 200 * 4;
const int i_Block_Size = 64;
const int i_Map_Size = 16;
const float f_FOV = 60;
const float f_Distance_To_Projection_Plane = (f_Width / 2) / tan(30 * M_PI / 180);
const float f_Angle_Shift = f_FOV / f_Width;
const float f_Speed = 1.5;
const float f_Mouse_Intensity = 2;

const int i_arr_Map[i_Map_Size][i_Map_Size] = 
{
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

Vector2f Find_First_Collision_Point_Horizontal(Vector2f f_Player, Vector2f *f_Horizontal_Collision_Point, float f_Current_Angle)
{
	if (f_Current_Angle < 180) f_Horizontal_Collision_Point->y = floor(f_Player.y / i_Block_Size) * i_Block_Size - 1;
	else f_Horizontal_Collision_Point->y = floor(f_Player.y / i_Block_Size) * i_Block_Size + i_Block_Size;

	f_Horizontal_Collision_Point->x = f_Player.x + (f_Player.y - f_Horizontal_Collision_Point->y) / tan(f_Current_Angle * M_PI / 180);

	return *f_Horizontal_Collision_Point;
}

Vector2f Find_Horizontal_Delta(Vector2f *f_Horizontal_Delta, float f_Current_Angle)
{
	if (f_Current_Angle < 180)
	{
		f_Horizontal_Delta->x = i_Block_Size / tan(f_Current_Angle * M_PI / 180);
		f_Horizontal_Delta->y = -i_Block_Size;
	}
	else
	{
		f_Horizontal_Delta->x = -i_Block_Size / tan(f_Current_Angle * M_PI / 180);
		f_Horizontal_Delta->y = i_Block_Size;
	}

	return *f_Horizontal_Delta;
}

bool Check_Horizontal_Collision(Vector2f f_Horizontal_Collision_Point)
{
	Vector2i Horizontal_Cell(0, 0);

	Horizontal_Cell.x = f_Horizontal_Collision_Point.x / i_Block_Size;
	Horizontal_Cell.y = floor(f_Horizontal_Collision_Point.y / i_Block_Size);

	if (i_arr_Map[Horizontal_Cell.y][Horizontal_Cell.x]) return true;
	else return false;
}

Vector2f Find_First_Collision_Point_Vertical(Vector2f f_Player, Vector2f *f_Vertical_Collision_Point, float f_Current_Angle)
{
	if (f_Current_Angle < 90 || f_Current_Angle > 270) f_Vertical_Collision_Point->x = floor(f_Player.x / i_Block_Size) * i_Block_Size + i_Block_Size;
	else f_Vertical_Collision_Point->x = floor(f_Player.x / i_Block_Size) * i_Block_Size - 1;

	f_Vertical_Collision_Point->y = f_Player.y + (f_Player.x - f_Vertical_Collision_Point->x) * tan(f_Current_Angle * M_PI / 180);

	return *f_Vertical_Collision_Point;
}

Vector2f Find_Vertical_Delta(Vector2f *f_Vertical_Delta, float f_Current_Angle)
{
	if (f_Current_Angle < 90 || f_Current_Angle > 270)
	{
		f_Vertical_Delta->x = i_Block_Size;
		f_Vertical_Delta->y = -i_Block_Size * tan(f_Current_Angle * M_PI / 180);
	}
	else
	{
		f_Vertical_Delta->x = -i_Block_Size;
		f_Vertical_Delta->y = i_Block_Size * tan(f_Current_Angle * M_PI / 180);
	}

	return *f_Vertical_Delta;
}

bool Check_Vertical_Collision(Vector2f f_Vertical_Collision_Point)
{
	Vector2i Vertical_Cell(0, 0);

	Vertical_Cell.x = floor(f_Vertical_Collision_Point.x / i_Block_Size);
	Vertical_Cell.y = f_Vertical_Collision_Point.y / i_Block_Size;

	if (i_arr_Map[Vertical_Cell.y][Vertical_Cell.x]) return true;
	else return false;
}

void Find_Distance_To_Wall(Vector2f f_Player, Vector2f f_Horizontal_Collision_Point, Vector2f f_Vertical_Collision_Point, Vector2f f_Horizontal_Delta, Vector2f f_Vertical_Delta, float f_Current_Angle, float f_Main_Angle, int i_Counter, vector <float> &Wall_Height, vector <float> &Ray_Length)
{
	bool b_Hit_Wall = false;

	float f_Distance_To_Horizontal_Wall = i_Block_Size * i_Map_Size + 1, f_Distance_To_Vertical_Wall = i_Block_Size * i_Map_Size + 1;

	if (f_Current_Angle != 0 || f_Current_Angle != 180)
	{
		Find_First_Collision_Point_Horizontal(f_Player, &f_Horizontal_Collision_Point, f_Current_Angle);
		Find_Horizontal_Delta(&f_Horizontal_Delta, f_Current_Angle);
		
		while (!b_Hit_Wall && f_Horizontal_Collision_Point.x <= i_Block_Size * i_Map_Size && f_Horizontal_Collision_Point.x >= 0 && f_Horizontal_Collision_Point.y <= i_Block_Size * i_Map_Size && f_Horizontal_Collision_Point.y >= 0)
		{
			if (Check_Horizontal_Collision(f_Horizontal_Collision_Point)) b_Hit_Wall = true;
			else f_Horizontal_Collision_Point += f_Horizontal_Delta;
		}

		f_Distance_To_Horizontal_Wall = sqrt(pow(f_Player.x - f_Horizontal_Collision_Point.x, 2) + pow(f_Player.y - f_Horizontal_Collision_Point.y, 2));

		//if (i_Counter == 160) cout << "f_Horizontal_Collision_Point = " << f_Horizontal_Collision_Point.x << ", " << f_Horizontal_Collision_Point.y << "(" << floor(f_Horizontal_Collision_Point.x / i_Block_Size) << ", " << floor(f_Horizontal_Collision_Point.y / i_Block_Size) << ", " << b_Hit_Wall << ", " << f_Distance_To_Horizontal_Wall << ")";
	}

	b_Hit_Wall = false;

	if (f_Current_Angle != 90 || f_Current_Angle != 270)
	{
		Find_First_Collision_Point_Vertical(f_Player, &f_Vertical_Collision_Point, f_Current_Angle);
		Find_Vertical_Delta(&f_Vertical_Delta, f_Current_Angle);

		while (!b_Hit_Wall && f_Vertical_Collision_Point.x <= i_Block_Size * i_Map_Size && f_Vertical_Collision_Point.x >= 0 && f_Vertical_Collision_Point.y <= i_Block_Size * i_Map_Size && f_Vertical_Collision_Point.y >= 0)
		{
			if (Check_Vertical_Collision(f_Vertical_Collision_Point)) b_Hit_Wall = true;
			else f_Vertical_Collision_Point += f_Vertical_Delta;
		}

		f_Distance_To_Vertical_Wall = sqrt(pow(f_Player.x - f_Vertical_Collision_Point.x, 2) + pow(f_Player.y - f_Vertical_Collision_Point.y, 2));

		//if (i_Counter == 160) cout << " f_Vertical_Collision_Point = " << f_Vertical_Collision_Point.x << ", " << f_Vertical_Collision_Point.y << "(" << floor(f_Vertical_Collision_Point.x / i_Block_Size) << ", " << floor(f_Vertical_Collision_Point.y / i_Block_Size) << ", " << b_Hit_Wall << ", " << f_Distance_To_Vertical_Wall << ")";
	}

	if (f_Distance_To_Horizontal_Wall < f_Distance_To_Vertical_Wall)
	{
		Wall_Height[i_Counter] = (i_Block_Size / f_Distance_To_Horizontal_Wall * f_Distance_To_Projection_Plane) / cos((f_Current_Angle - f_Main_Angle) * M_PI / 180);
		Ray_Length[i_Counter] = f_Distance_To_Horizontal_Wall;
		//if (i_Counter == 160) cout << " H" << endl;
	} else
	{
		Wall_Height[i_Counter] = (i_Block_Size / f_Distance_To_Vertical_Wall * f_Distance_To_Projection_Plane) / cos((f_Current_Angle - f_Main_Angle) * M_PI / 180);
		Ray_Length[i_Counter] = f_Distance_To_Vertical_Wall;
		//if (i_Counter == 160) cout << " V" << endl;
	}
}

int main()
{
	//////////////////////////////////////////////////////////////////////////DEFINING VARIAVLES//////////////////////////////////////////////////////////////////////////
	Vector2f f_Player(96, 70); /*Player coordinates*/
	
	Vector2f f_Horizontal_Collision_Point(0.0f, 0.0f); /*Coordinates of the point where first collision with horizontal grid happened*/
	Vector2f f_Vertical_Collision_Point(0.0f, 0.0f); /*Coordinates of the point where first collision with vertical grid happened*/
	
	Vector2f f_Horizontal_Delta(0.0f, 0.0f); /*Value of shift by X and Y axises where next collision with horizontal grid happened*/
	Vector2f f_Vertical_Delta(0.0f, 0.0f); /*Value of shift by X and Y axises where next collision with vertical grid happened*/

	vector <float> Wall_Height; /*Array (vector) of all wall heights that we need to draw*/
	Wall_Height.reserve(f_Width); /*Reserving size of vector for every pixel on screen*/

	vector <float> Ray_Length; /*I use vector of ray lengthes only to calculate color intensity*/
	Ray_Length.reserve(f_Width); /*Actually it better to be done depending on wall height*/

	float f_Main_Angle = 2.3, f_Current_Angle = f_Main_Angle - 30; /*Main Angle - where player looks, Current Angle - where current ray has been casted*/

	RenderWindow window(VideoMode(f_Width, f_Height), "Ray_Casting 2.0"/*, Style::Fullscreen*/); /*Making SFML window*/
	window.setMouseCursorVisible(false);

	//////////////////////////////////////////////////////////////////////////////MAIN CYCLE//////////////////////////////////////////////////////////////////////////////
	while (window.isOpen())
    {
    	Wall_Height.clear();

    	if (f_Main_Angle < 0) f_Main_Angle = 360 + f_Main_Angle;
        else if (f_Main_Angle > 360) f_Main_Angle = -360 + f_Main_Angle;

    	f_Current_Angle = f_Main_Angle + f_FOV / 2;

    	for (int i = 0; i < f_Width; i++)
    	{
    		Find_Distance_To_Wall(f_Player, f_Horizontal_Collision_Point, f_Vertical_Collision_Point, f_Horizontal_Delta, f_Vertical_Delta, f_Current_Angle, f_Main_Angle, i, Wall_Height, Ray_Length);
    		f_Current_Angle -= f_Angle_Shift;
    		if (f_Current_Angle < 0) f_Current_Angle = 360 + f_Current_Angle;
        	else if (f_Current_Angle > 360) f_Current_Angle = -360 + f_Current_Angle;
    	}

    	Event event;
    	while (window.pollEvent(event))
    	{
			if (event.type == Event::Closed) window.close();
		}
		
		///////////////////////////////////////////////////////////////////////////////MOVE///////////////////////////////////////////////////////////////////////////////
		float f_Velocity_X = cos(f_Main_Angle * M_PI / 180);
		float f_Velocity_X_Horizontal = cos((f_Main_Angle + 90) * M_PI / 180);
    	float f_Velocity_Y = -sin(f_Main_Angle * M_PI / 180);
    	float f_Velocity_Y_Horizontal = -sin((f_Main_Angle + 90) * M_PI / 180);

	    if (Keyboard::isKeyPressed(Keyboard::Left)) f_Main_Angle += 8 * f_Angle_Shift; 
	    if (Keyboard::isKeyPressed(Keyboard::Right)) f_Main_Angle -= 8 * f_Angle_Shift;
	    if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)) { f_Player.x += f_Velocity_X * f_Speed; f_Player.y += f_Velocity_Y * f_Speed; }
	    if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S)) { f_Player.x -= f_Velocity_X * f_Speed; f_Player.y -= f_Velocity_Y * f_Speed; }
	    if (Keyboard::isKeyPressed(Keyboard::A)) { f_Player.x += f_Velocity_X_Horizontal * f_Speed; f_Player.y += f_Velocity_Y_Horizontal * f_Speed; }
	    if (Keyboard::isKeyPressed(Keyboard::D)) { f_Player.x -= f_Velocity_X_Horizontal * f_Speed; f_Player.y -= f_Velocity_Y_Horizontal * f_Speed; }

		///////////////////////////////////////////////////////////////////////////////DRAW///////////////////////////////////////////////////////////////////////////////
		window.clear();

		for (int i = 0; i < f_Width; i++)
		{
			int i_Wall_Color = 255 - Ray_Length[i] / (int)(i_Map_Size * i_Block_Size / 255);
	        if (i_Wall_Color < 0) i_Wall_Color = 0;
	        if (i_Wall_Color > 255) i_Wall_Color = 255;

	        //if (i == f_Width / 2) i_Wall_Color = 0;

			Vertex Wall_Slice[] = 
			{
				Vertex(Vector2f(i + 1, f_Height / 2 - Wall_Height[i] / 2), Color(i_Wall_Color, i_Wall_Color, i_Wall_Color)),
	        	Vertex(Vector2f(i + 1, f_Height / 2 + Wall_Height[i] / 2), Color(i_Wall_Color, i_Wall_Color, i_Wall_Color))
			};

			window.draw(Wall_Slice, 2, Lines);
		}

		window.display();
	}

	return 0;
}