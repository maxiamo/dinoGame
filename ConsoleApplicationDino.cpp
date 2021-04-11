#include <Windows.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>
#include <unordered_map>
#include <fstream>
#include <string>

using namespace std;
int score = 100;
int penalty = 0;
class Point {
	int _x, _y;

public:
	Point() {
		_x = 0;
		_y = 0;
	}

	Point(int x, int y) {
		_x = x;
		_y = y;
	}

	int getX() { return _x; }
	int getY() { return _y; }

};

// Отсюда вы можете брать цвета и здесь же создавать свои
class ConsoleColor {
public:
	static COLORREF red() { return RGB(255, 0, 0); }
	static COLORREF blue() { return RGB(0, 0, 255); }
	static COLORREF gray() { return RGB(200, 200, 200); }
	static COLORREF white() { return RGB(255, 255, 255); }
	static COLORREF green() { return RGB(0, 255, 0); }
	static COLORREF black() { return RGB(0, 0, 0); }
	static COLORREF brown() { return RGB(80, 20, 25); }
	static COLORREF yellow() { return RGB(255, 255, 0); }
};

// Это класс, который нужен для рисования. В нем все готово. Ничего менять не нужно. Нужно только пользоваться
class ConsoleDrawer {
	HANDLE _conHandle;
	HWND _hwnd;
	HDC _hdc;
	PAINTSTRUCT _ps;
	unordered_map<COLORREF, HGDIOBJ> _bm;
	unordered_map<COLORREF, HGDIOBJ> _pm;
	HGDIOBJ _curentBrush;
	HGDIOBJ _curentPen;

	int _width;
	int _height;
	bool _isWork = true;

	void selectBrush(COLORREF color, bool filled = false) {
		if (!filled) {
			SelectObject(_hdc, CreateSolidBrush(NULL_BRUSH));
			return;
		}
		if (_bm.find(color) == _bm.end())
			_bm.insert({ color, CreateSolidBrush(color) });

		if (_curentBrush != _bm[color]) {
			_curentBrush = _bm[color];
			SelectObject(_hdc, _curentBrush);
		}
	}

	void selectPen(COLORREF color) {
		if (_pm.find(color) == _pm.end())
			_pm.insert({ color, CreatePen(PS_SOLID, 1, color) });
		if (_curentPen != _pm[color]) {
			_curentPen = _pm[color];
			SelectObject(_hdc, _curentPen);
		}
	}

public:
	ConsoleDrawer() {
		setlocale(LC_ALL, "");
		_conHandle = GetStdHandle(STD_INPUT_HANDLE);
		int t = sizeof(HGDIOBJ);
		_hwnd = GetConsoleWindow();
		RECT rc;
		GetClientRect(_hwnd, &rc);
		_width = rc.right;
		_height = rc.bottom;
		_hdc = GetDC(_hwnd);
	}

	void clearScreen() {
		system("cls");
	}

	int getWidth() { return _width; }
	int getHeight() { return _height; }

	HANDLE get_conHandle() {
		return _conHandle;
	}

	bool isWork() {

		return _isWork;
	}

	void stop() {
		_isWork = false;
	}

	void drawBackground(COLORREF color = ConsoleColor::gray()) {
		RECT rc;
		GetClientRect(_hwnd, &rc);
		drawRect(Point(), Point(_width, _height), color, true);
	}

	void drawPoint(Point p, COLORREF color) {
		SetPixel(_hdc, p.getX(), p.getY(), color);
	}

	void drawLine(Point p1, Point p2, COLORREF color) {
		selectPen(color);
		//SelectObject(_hdc, CreatePen(PS_SOLID, 1, color));
		//SetDCPenColor(_hdc, color);
		MoveToEx(_hdc, p1.getX(), p1.getY(), NULL);
		LineTo(_hdc, p2.getX(), p2.getY());
	}

	// Рисует круг или окружность по центру и радиусу
	// filled - нужно ли закрашивать внутренность
	void drawCircle(Point c, int radius, COLORREF color, bool filled = false) {
		selectPen(color);
		selectBrush(color, filled);
		//SetDCBrushColor(_hdc, color);
		Ellipse(_hdc, c.getX() - radius, c.getY() - radius, c.getX() + radius, c.getY() + radius);

	}

	// Рисует прямоугольник по двум точкам
	// p1 - нижний левый угол
	// p2 - верхний правый угол
	void drawRect(Point p1, Point p2, COLORREF color, bool filled = false) {
		selectPen(color);
		selectBrush(color, filled);
		Rectangle(_hdc, p1.getX(), p1.getY(), p2.getX(), p2.getY());
	}

	// Рисует треугольник по трем точкам
	void drawTriangle(Point p1, Point p2, Point p3, COLORREF color, bool filled = false) {
		selectPen(color);
		selectBrush(color, filled);
		POINT apt[3];
		apt[0].x = p1.getX(); apt[0].y = p1.getY();
		apt[1].x = p2.getX(); apt[1].y = p2.getY();
		apt[2].x = p3.getX(); apt[2].y = p3.getY();
		Polygon(_hdc, apt, 3);
	}
};

// Это самый базовый класс, от которого наследуются все объекты, которые появляются на поле
class ParkObject {
protected:
	int _cellX, _cellY, _cellSize;

public:
	ParkObject(int cellX, int cellY, int cellSize) {
		_cellX = cellX;
		_cellY = cellY;
		_cellSize = cellSize;
	}

	bool isInside(int cellX, int cellY) {
		return (_cellX == cellX) && (_cellY == cellY);
	}

	int getCellX() {
		return _cellX;
	}

	int getCellY() {
		return _cellY;
	}

	int getX() {
		return _cellX * _cellSize;
	}

	int getY() {
		return _cellY * _cellSize;
	}

};

class SnowMan : public ParkObject {

public:
	SnowMan(int cellX, int cellY, int cellSize) : ParkObject(cellX, cellY, cellSize) { }

	void draw(ConsoleDrawer& cd) {
		//Переходим от ячеек к координатам пикселей
		int x = getX();
		int y = getY();

		double d = _cellSize / 100.0;
		cd.drawCircle(Point(x + _cellSize / 2, y + _cellSize - 20 * d), 20 * d, ConsoleColor::white(), true);
		cd.drawCircle(Point(x + _cellSize / 2, y + _cellSize - 55 * d), 15 * d, ConsoleColor::white(), true);
		cd.drawCircle(Point(x + _cellSize / 2, y + _cellSize - 80 * d), 10 * d, ConsoleColor::white(), true);
		cd.drawRect(Point(x + _cellSize / 2 - 5 * d, y + 10 * d), Point(x + _cellSize / 2 + 5 * d, y + d), ConsoleColor::brown(), true);
	}
};
int gg = 0;
int color;
class Tree : public ParkObject {

public:
	Tree(int cellX, int cellY, int cellSize) : ParkObject(cellX, cellY, cellSize) {}
	COLORREF colortree = ConsoleColor::green();
	void draw(ConsoleDrawer& cd) {
		//Переходим от ячеек к координатам пикселей
		int x = getX();
		int y = getY();
		//	--/---------------------------------------------------------------------------------:::::::::::::::::::::::::::::::DDDDDDDDDDDDDDDDDDD

		if (color >= 0 && color <= 6)
		{
			gg = 1;
			//colortree = ConsoleColor::green();

		}
		else if (color > 6 && color <= 8)
		{
			gg = 2;
			//colortree = ConsoleColor::blue();
		}
		else if (color > 8 && color <= 10)// colortree = ConsoleColor::yellow();
		{
			//colortree = ConsoleColor::yellow();
			gg = 3;
		}

		int d = _cellSize / 10;
		cd.drawRect(Point(x + 4 * d, y + _cellSize), Point(x + 6 * d, y + _cellSize - 2 * d), ConsoleColor::brown(), true);
		//	cd.drawTriangle(Point(x + 2 * d, y + _cellSize - 2 * d), Point(x + _cellSize / 2, y + d), Point(x + _cellSize - 2 * d, y + _cellSize - 2 * d), ConsoleColor::green(), true);
		cd.drawTriangle(Point(x + 2 * d, y + _cellSize - 2 * d), Point(x + _cellSize / 2, y + d), Point(x + _cellSize - 2 * d, y + _cellSize - 2 * d), colortree, true);
	}
};
class Present : public ParkObject {

public:
	Present(int cellX, int cellY, int cellSize) : ParkObject(cellX, cellY, cellSize) {
	}

	void draw(ConsoleDrawer& cd) {
		//Переходим от ячеек к координатам пикселей
		int x = getX();
		int y = getY();

		double d = _cellSize / 100.0;
		cd.drawRect(Point(x + 20 * d, y + _cellSize), Point(x + 80 * d, y + 40 * d), ConsoleColor::red(), true);
		cd.drawRect(Point(x + 40 * d, y + _cellSize), Point(x + 60 * d, y + 40 * d), ConsoleColor::yellow(), true);
		cd.drawCircle(Point(x + 35 * d, y + 20 * d), 10 * d, ConsoleColor::yellow(), true);
		cd.drawCircle(Point(x + 65 * d, y + 20 * d), 10 * d, ConsoleColor::yellow(), true);
		cd.drawTriangle(Point(x + 50 * d, y + 40 * d), Point(x + 35 * d, y + 30 * d), Point(x + 45 * d, y + 20 * d), ConsoleColor::yellow(), true);
		cd.drawTriangle(Point(x + 50 * d, y + 40 * d), Point(x + 65 * d, y + 30 * d), Point(x + 55 * d, y + 20 * d), ConsoleColor::yellow(), true);
	}
};
class HappyDino : public ParkObject {

public:
	HappyDino(int cellX, int cellY, int cellSize) : ParkObject(cellX, cellY, cellSize) {}

	void draw(ConsoleDrawer& cd) {

		//Переходим от ячеек к координатам пикселей
		int x = getX();
		int y = getY();
		COLORREF color = ConsoleColor::green();

		double d = _cellSize / 125.0;
		cd.drawRect(Point(x + d, y + 63 * d), Point(x + 24 * d, y + 53 * d), color, true);
		cd.drawRect(Point(x + 75 * d, y + 63 * d), Point(x + 98 * d, y + 53 * d), color, true);
		cd.drawRect(Point(x + 27 * d, y + 93 * d), Point(x + 72 * d, y + 35 * d), color, true);//body
		cd.drawRect(Point(x + 27 * d, y + 124 * d), Point(x + 42 * d, y + 94 * d), color, true);
		cd.drawRect(Point(x + 57 * d, y + 124 * d), Point(x + 72 * d, y + 94 * d), color, true);
		cd.drawRect(Point(x + 27 * d, y + 32 * d), Point(x + 88 * d, y + 2 * d), color, true);//head
		cd.drawCircle(Point(x + 42 * d, y + 12 * d), 8 * d, ConsoleColor::gray(), true);
		cd.drawTriangle(Point(x + 27 * d, y), Point(x + 49 * d, y - 45 * d), Point(x + 72 * d, y), ConsoleColor::red(), true);
		cd.drawCircle(Point(x + 49 * d, y - 50 * d), 10 * d, ConsoleColor::white(), true);

	}




	void step(int direction) {
		if (direction == 0)
			_cellX -= 1;
		if (direction == 1)
			_cellY -= 1;
		if (direction == 2)
			_cellX += 1;
		if (direction == 3)
			_cellY += 1;
	}
};
class EvilDino : public ParkObject {

public:
	EvilDino(int cellX, int cellY, int cellSize) : ParkObject(cellX, cellY, cellSize) {}

	void draw(ConsoleDrawer& cd) {

		//Переходим от ячеек к координатам пикселей
		int x = getX();
		int y = getY();
		COLORREF colorEvil = ConsoleColor::red();

		double d = _cellSize / 125.0;
		cd.drawRect(Point(x + d, y + 63 * d), Point(x + 24 * d, y + 53 * d), color, true);
		cd.drawRect(Point(x + 75 * d, y + 63 * d), Point(x + 98 * d, y + 53 * d), color, true);
		cd.drawRect(Point(x + 27 * d, y + 93 * d), Point(x + 72 * d, y + 35 * d), color, true);//body
		cd.drawRect(Point(x + 27 * d, y + 124 * d), Point(x + 42 * d, y + 94 * d), color, true);
		cd.drawRect(Point(x + 57 * d, y + 124 * d), Point(x + 72 * d, y + 94 * d), color, true);
		cd.drawRect(Point(x + 27 * d, y + 32 * d), Point(x + 88 * d, y + 2 * d), color, true);//head
		cd.drawCircle(Point(x + 42 * d, y + 12 * d), 8 * d, ConsoleColor::gray(), true);
		cd.drawTriangle(Point(x + 27 * d, y), Point(x + 49 * d, y - 45 * d), Point(x + 72 * d, y), ConsoleColor::red(), true);
		cd.drawCircle(Point(x + 49 * d, y - 50 * d), 10 * d, ConsoleColor::white(), true);

	}




	void stepEvil(int direction) {
		/*int dir = (rand() % 5);
		if (dir == 0)
			_cellX -= 1;
		if (dir == 1)
			_cellY -= 1;
		if (dir == 2)
			_cellX += 1;
		if (dir == 3)
			_cellY += 1;
			*/

		if (direction == 0)
			_cellX -= 1;
		if (direction == 1)
			_cellY -= 1;
		if (direction == 2)
			_cellX += 1;
		if (direction == 3)
			_cellY += 1;
	}


};

class DinoPark {
	int _cellsXCount;
	int _cellsYCount;
	int _cellSize;
	vector<SnowMan> _snowmen;
	vector<Tree> _trees;
	vector<HappyDino> _hDino;
	vector<EvilDino> _EvilDino;
	vector<Present> _present;
public:
	DinoPark(int width, int height, int cellSize) {
		_cellsXCount = width;
		_cellsYCount = height;
		_cellSize = cellSize;
	}

	// Этот метод проверяет, что находится в клетке:
	// -1 - снеговик
	//  0 - пусто
	//  1 - дерево
	//  2 - динозавр
	int find(int x, int y) {
		for (int i = 0; i < _snowmen.size(); i++) {
			if (_snowmen[i].isInside(x, y))
				return -1;
		}
		for (int i = 0; i < _trees.size(); i++) {
			if (_trees[i].isInside(x, y))
				return 1;
		}
		for (int i = 0; i < _hDino.size(); i++) {
			if (_hDino[i].isInside(x, y))
				return 2;

		}
		for (int i = 0; i < _EvilDino.size(); i++) {
			if (_EvilDino[i].isInside(x, y))
				return 4;
		}
		for (int i = 0; i < _present.size(); i++) {
			if (_present[i].isInside(x, y))
				return 3;
		}
		return 0;
	}

	// Метод для отрисовки всего поля
	void draw(ConsoleDrawer& cd) {

		// Рисуем сетку
		for (int i = 0; i <= _cellsXCount; i++) {
			int x = i * _cellSize;
			int y = _cellsYCount * _cellSize;
			cd.drawLine(Point(x, 0), Point(x, y), ConsoleColor::black());
		}
		for (int i = 0; i <= _cellsYCount; i++) {
			int x = _cellsXCount * _cellSize;
			int y = i * _cellSize;
			cd.drawLine(Point(0, y), Point(x, y), ConsoleColor::black());
		}

		// Рисуем снеговиков
		for (int i = 0; i < _snowmen.size(); i++) {
			_snowmen[i].draw(cd);
		}

		// Рисуем деревья
		for (int i = 0; i < _trees.size(); i++) {
			_trees[i].draw(cd);

		}

		// Рисуем динозавра
		for (int i = 0; i < _hDino.size(); i++) {
			_hDino[i].draw(cd);
		}
		for (int i = 0; i < _EvilDino.size(); i++) {
			_EvilDino[i].draw(cd);
		}
		for (int i = 0; i < _present.size(); i++) {
			_present[i].draw(cd);
		}
	}

	// Метод обработки вашего хода
	void step(ConsoleDrawer& cd) {

		// Пока ходит только счастливый динозавр

		// Ловим нажатие на клавиатуру
		KEY_EVENT_RECORD key;
		INPUT_RECORD irec[100];
		DWORD cc;

		ReadConsoleInput(cd.get_conHandle(), irec, 100, &cc);
		for (DWORD j = 0; j < cc; ++j) {
			if (irec[j].EventType == KEY_EVENT && irec[j].Event.KeyEvent.bKeyDown) {

				for (int i = 0; i < _hDino.size(); i++) {
					// Смотрим по сторонам
					// -1 - снеговик
					//  0 - пусто
					//  1 - дерево
					//  2 - динозавр
					vector<int> res = {
						lookLeft(_hDino[i].getCellX(), _hDino[i].getCellY()),
						lookUp(_hDino[i].getCellX(), _hDino[i].getCellY()),
						lookRight(_hDino[i].getCellX(), _hDino[i].getCellY()),
						lookDown(_hDino[i].getCellX(), _hDino[i].getCellY())
					};
					vector<int> resEv = {
						lookLeft(_EvilDino[i].getCellX(), _EvilDino[i].getCellY()),
						lookUp(_EvilDino[i].getCellX(), _EvilDino[i].getCellY()),
						lookRight(_EvilDino[i].getCellX(), _EvilDino[i].getCellY()),
						lookDown(_EvilDino[i].getCellX(), _EvilDino[i].getCellY())
					};
					// Проверяем, какая именно кнопка была нажата
					switch (irec[j].Event.KeyEvent.wVirtualKeyCode) {

					case VK_LEFT:

						// если путь свободен, идем
						if (res[0] != -1) {

							_hDino[i].step(0);
							// если мы на что-то наступили (пока что только дерево), обновляем картину
							if (res[0] != 0) {
								refresh(cd);

								refreshPresent(cd);

								penalty = 0;
								color = (rand() % 10);
								ColorChange();
								if (gg == 1) {
									score += 10;
								}
								else if (gg == 2) {
									score += 20;
								}
								else if (gg == 3) {
									score += 50;
								}
								//int color = (rand() % 10);
								//if (color >= 0 && color <= 6) _trees[i].colortree = ConsoleColor::green();
							//	else if (color > 6 && color <= 8) _trees[i].colortree = ConsoleColor::blue();
							//	else if (color > 8 && color <= 10) _trees[i].colortree = ConsoleColor::yellow();
								//-------------------------------------------------------------------------GVGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
							}
							else if (res[0] == 0) {
								score -= 2;
								penalty++;

							}
							Penka();
							if (resEv[0] != -1) {
								_EvilDino[i].stepEvil(0);
								if (resEv[0] != 0) {
									refreshEvil(cd);
								}
							}
						}
						if (score < 0) {
							cd.stop();
							cd.clearScreen();

							cout << "Score = " << score << endl;

						}
						break;
					case VK_UP:

						if (res[1] != -1) {
							_hDino[i].step(1);

							if (res[1] != 0) {
								refresh(cd);
								refreshPresent(cd);
								penalty = 0;
								color = (rand() % 10);
								ColorChange();
								if (gg == 1) {
									score += 10;

								}
								else if (gg == 2) {
									score += 20;

								}
								else if (gg == 3) {
									score += 50;
								}
								//	int color = (rand() % 10);
									//if (color >= 0 && color <= 6) _trees[i].colortree = ConsoleColor::green();
									//else if (color > 6 && color <= 8) _trees[i].colortree = ConsoleColor::blue();
								//	else if (color > 8 && color <= 10) _trees[i].colortree = ConsoleColor::yellow();
									//_trees[i].colortree = ConsoleColor::blue();
							}
							else if (res[1] == 0) {
								score -= 2;
								penalty++;
							}
							Penka();
							if (resEv[1] != -1) {
								_EvilDino[i].stepEvil(1);
								if (resEv[1] != 0) {
									refreshEvil(cd);
								}
							}
						}
						if (score < 0) {
							cd.stop();
							cd.clearScreen();

							cout << "Score = " << score << endl;

						}
						break;
					case VK_RIGHT:

						if (res[2] != -1) {
							_hDino[i].step(2);
							if (res[2] != 0) {
								refresh(cd);
								penalty = 0;
								refreshPresent(cd);
								color = (rand() % 10);
								ColorChange();
								if (gg == 1) {
									score += 10;
								}
								else if (gg == 2) {
									score += 20;
									//	_trees[1].colortree = ConsoleColor::blue();
								}
								else if (gg == 3) {
									score += 50;
								}
								//int color = (rand() % 10);
								//if (color >= 0 && color <= 6) _trees[i].colortree = ConsoleColor::green();
							//	else if (color > 6 && color <= 8) _trees[i].colortree = ConsoleColor::blue();
							//	else if (color > 8 && color <= 10) _trees[i].colortree = ConsoleColor::yellow();
							}
							else if (res[2] == 0) {
								score -= 2;
								penalty++;

							}
							Penka();
							if (resEv[2] != -1) {
								_EvilDino[i].stepEvil(2);
								if (resEv[2] != 0) {
									refreshEvil(cd);
								}
							}
						}
						if (score < 0) {
							cd.stop();
							cd.clearScreen();

							cout << "Score = " << score << endl;

						}
						break;
					case VK_DOWN:

						if (res[3] != -1) {
							_hDino[i].step(3);
							if (res[3] != 0) {
								refresh(cd);
								refreshPresent(cd);
								penalty = 0;
								color = (rand() % 10);
								ColorChange();
								if (gg == 1) {
									score += 10;
								}
								else if (gg == 2) {
									score += 20;
								}
								else if (gg == 3) {
									score += 50;
								}
								//int color = (rand() % 10);
							//	if (color >= 0 && color <= 6) _trees[i].colortree = ConsoleColor::green();
							//	else if (color > 6 && color <= 8) _trees[i].colortree = ConsoleColor::blue();
							//	else if (color > 8 && color <= 10) _trees[i].colortree = ConsoleColor::yellow();
							}
							else if (res[3] == 0) {
								score -= 2;
								penalty++;


							}





							if (resEv[3] != -1) {
								_EvilDino[i].stepEvil(3);
								if (resEv[3] != 0) {
									refreshEvil(cd);
								}
							}


							Penka();
						}
						if (score < 0) {
							cd.stop();
							cd.clearScreen();

							cout << "Score = " << score << endl;

						}
						break;

					case VK_ESCAPE:
						cd.stop();
						cd.clearScreen();
						//-------------------------------------------------------------------------------
						cout << "Score = " << score << endl;
						//cout << penalty;

						break;
					case VK_SHIFT:
						score = 100;
						penalty = 0;
						cd.clearScreen();

						srand(time(0));
						const int CellSize = 50;
						int width = cd.getWidth() / CellSize;
						int height = cd.getHeight() / CellSize;
						DinoPark dinoPark(width, height, CellSize);
						int snowcount = (rand()) % 18;
						int snowcount2 = (rand()) % 18;
						int snowcount3 = (rand()) % 18;
						dinoPark.addSnowMan(5, 2);
						dinoPark.addSnowMan(7, 4);
						dinoPark.addSnowMan(5, 6);
						dinoPark.addSnowMan(3, 4);
						dinoPark.addSnowMan(snowcount, 8);
						dinoPark.addSnowMan(snowcount2, 6);
						dinoPark.addSnowMan(snowcount3, 3);
						dinoPark.addSnowMan(snowcount - 1, 2);

						dinoPark.addTree(3, 2);
						dinoPark.addTree(7, 2);
						dinoPark.addTree(7, 6);
						dinoPark.addTree(3, 6);
						dinoPark.addEvilDino(9, 5);
						//dinoPark.addPresent(5,5);
						dinoPark.addHappyDino(5, 4);

						dinoPark.addPresent(5, 5);


						cd.clearScreen();
						color = (rand() % 10);
						while (cd.isWork()) {
							cd.drawBackground();
							dinoPark.draw(cd);
							dinoPark.step(cd);

						}


						break;
					}

				}

			}
		}
	}

	void addSnowMan(int x, int y) {
		_snowmen.push_back(SnowMan(x, y, _cellSize));
	}

	void addTree(int x, int y) {
		_trees.push_back(Tree(x, y, _cellSize));
	}


	void addHappyDino(int x, int y) {
		_hDino.push_back(HappyDino(x, y, _cellSize));
	}
	void addEvilDino(int x, int y) {
		_EvilDino.push_back(EvilDino(x, y, _cellSize));
	}

	void addPresent(int x, int y) {
		_present.push_back(Present(x, y, _cellSize));
	}

	// Взгляд на клетку вверх
	int lookUp(int cellX, int cellY) {
		if (cellY == 0)
			return -1;
		return find(cellX, cellY - 1);
	}

	// Взгляд на клетку вниз
	int lookDown(int cellX, int cellY) {
		if (cellY == _cellsYCount - 1)
			return -1;
		return find(cellX, cellY + 1);
	}

	// Взгляд на клетку вправо
	int lookRight(int cellX, int cellY) {
		if (cellX == _cellsXCount - 1)
			return -1;
		return find(cellX + 1, cellY);
	}

	// Взгляд на клетку влево
	int lookLeft(int cellX, int cellY) {
		if (cellX == 0)
			return -1;
		return find(cellX - 1, cellY);
	}

	// Обновляем картину
	void refresh(ConsoleDrawer& cd) {
		for (int i = 0; i < _hDino.size(); i++) {
			// Смотрим, где стоит динозавр
			int x = _hDino[i].getCellX();
			int y = _hDino[i].getCellY();
			int Mx = _EvilDino[i].getCellX();
			int Ny = _EvilDino[i].getCellY();
			// Находим то дерево, на котором он стоит
			for (int j = 0; j < _trees.size(); j++) {
				if (_trees[j].isInside(x, y)) {
					// Переносим дерево в другое место
					// Цикл нужен, чтобы не попасть в место, которое уже занято
					for (int k = 0; k < 100; k++) {
						int _x = rand() % _cellsXCount;
						int _y = rand() % _cellsYCount;
						if (find(_x, _y) == 0) {
							_trees[j] = Tree(_x, _y, _cellSize);
							break;
						}
					}
				}
			}

		}
	}
	void refreshEvil(ConsoleDrawer& cd) {
		score -= 200;
		for (int i = 0; i < _EvilDino.size(); i++) {
			// Смотрим, где стоит динозавр

			int Mx = _EvilDino[i].getCellX();
			int Ny = _EvilDino[i].getCellY();
			// Находим то дерево, на котором он стоит
			for (int j = 0; j < _trees.size(); j++) {
				if (_trees[j].isInside(Mx, Ny)) {
					// Переносим дерево в другое место
					// Цикл нужен, чтобы не попасть в место, которое уже занято
					for (int k = 0; k < 100; k++) {
						int _x = rand() % _cellsXCount;
						int _y = rand() % _cellsYCount;
						if (find(_x, _y) == 0) {
							_trees[j] = Tree(_x, _y, _cellSize);
							break;
						}
					}
				}
			}

		}
		for (int i = 0; i < _hDino.size(); i++) {
			int Mx = _EvilDino[i].getCellX();
			int Ny = _EvilDino[i].getCellY();
			if (_hDino[i].isInside(Mx, Ny)) {
				score -= 200;
			}
		}
	}

	void refreshPresent(ConsoleDrawer& cd) {
		for (int i = 0; i < _hDino.size(); i++) {
			// Смотрим, где стоит динозавр Eng - Dino position
			int x = _hDino[i].getCellX();
			int y = _hDino[i].getCellY();
			// Находим то дерево, на котором он стоит Eng - Tree position, that Dino is standing on

			for (int j = 0; j < _present.size(); j++) {
				if (_present[j].isInside(x, y)) {
					int MysteryGift = (rand() % 55);
					if (MysteryGift == 1) addEvilDino(2, 1);
					if (MysteryGift >= 2 && MysteryGift <= 20) score += 100;
					else if (MysteryGift >= 21 && MysteryGift <= 30) score -= 99;
					else if (MysteryGift >= 31 && MysteryGift <= 39) score += 1;
					else if (MysteryGift >= 40 && MysteryGift <= 42) score += 1000;
					else if (MysteryGift >= 43 && MysteryGift <= 47) score -= 150;
					else if (MysteryGift >= 48 && MysteryGift <= 49) addTree(3, 7);
					else if (MysteryGift == 50) score += 10000;
					// Переносим дерево в другое место  Eng - Tree place changing
					// Цикл нужен, чтобы не попасть в место, которое уже занято Eng - cycle is for checking if place is free
					for (int k = 0; k < 100; k++) {
						int _x = rand() % _cellsXCount;
						int _y = rand() % _cellsYCount;
						if (find(_x, _y) == 0) {
							_present[j] = Present(_x, _y, _cellSize);
							break;
						}
					}
				}
			}
		}
	}
	void Penka() {
		if (penalty >= 20) {
			penalty = 0;
			score -= 100;

		}
	}

	void ColorChange() {
		if (gg == 1) {
			_trees[1].colortree = ConsoleColor::green();
		}
		if (gg == 2) {
			_trees[1].colortree = ConsoleColor::blue();
		}
		if (gg == 3) {
			_trees[1].colortree = ConsoleColor::yellow();
		}
	}

};



int main() {
	ConsoleDrawer cd;
	srand(time(0));
	const int CellSize = 50;
	int width = cd.getWidth() / CellSize;
	int height = cd.getHeight() / CellSize;
	DinoPark dinoPark(width, height, CellSize);
	int snowcount = (rand()) % 18;
	int snowcount2 = (rand()) % 18;
	int snowcount3 = (rand()) % 18;
	dinoPark.addSnowMan(5, 2);
	dinoPark.addSnowMan(7, 4);
	dinoPark.addSnowMan(5, 6);
	dinoPark.addSnowMan(3, 4);
	dinoPark.addSnowMan(snowcount, 8);
	dinoPark.addSnowMan(snowcount2, 6);
	dinoPark.addSnowMan(snowcount3, 3);
	dinoPark.addSnowMan(snowcount - 1, 2);

	dinoPark.addTree(3, 2);
	dinoPark.addTree(7, 2);
	dinoPark.addTree(7, 6);
	dinoPark.addTree(3, 6);
	dinoPark.addEvilDino(9, 5);
	//dinoPark.addPresent(5,5);
	dinoPark.addHappyDino(5, 4);

	dinoPark.addPresent(5, 5);


	cd.clearScreen();
	color = (rand() % 10);
	while (cd.isWork()) {
		cd.drawBackground();
		dinoPark.draw(cd);
		dinoPark.step(cd);

	}

	return 0;
}