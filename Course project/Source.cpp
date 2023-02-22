#include<iostream>;
#include<Windows.h>;
#include<conio.h>;
#include<time.h>;
#include<string.h>;
//#include <stdlib.h>;
#include<io.h>
using namespace std;
enum Color {
	Black, Blue, Green, Cyan, Red,
	Magenta, Brown, LightGray, DarkGray, LightBlue, LightGreen, LightCyan,
	LightRed, LightMagenta, Yellow, White
};
enum Direction { Up = 72, Left = 75, Right = 77, Down = 80, Enter = 13, esc = 27, Space = 32, Num1 = 49, Num2 = 50, Num3 = 51, Num4 = 52 };
HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);//получаем дискриптор активного окна
void SetCursor(int x, int y)
{
	COORD myCoords = { x,y };
	SetConsoleCursorPosition(hStdOut, myCoords);
}
void SetColor(Color text, Color background)
{
	SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}
//------------------координаты сообщений в консоли-------------------
int PlayerCountMessageX = 17, PlayerCountMessageY = 3, EnemyCountMessageX = 73, EnemyCountMessageY = 3;
int PlayerShotX = 45, PlayerShotY = 6, EnemyShotX = 45, EnemyShotY = 5;
int ConsolMessageX = 25, ConsolMessageY = 5;
int menuX = 30, menuY = 7;
//-------------------------------------------------------------------
const int AllCount = 10;
const int fieldH = 12, fieldW = 24; // размеры поля
const int strLength = 20, countPunctsMenu = 3, strPass = 15; //
int fieldX = 20, fieldY = 10, startX = 22, startY = 11, fieldEnemyX = 76, startEnemyX = 78; // координаты начала отрисовки полей и начала курсора
int size = 1, sizeCol = 2; // размер корабля
int copyX, copyY, firstHitX, firstHitY; // переменные для сохранения координат при сложном режиме игры.
char** ship = nullptr; // массив для отрисовки корабля при ручной расстановке
char field[fieldH][fieldW]; // массив поля игрока
char fieldEnemy[fieldH][fieldW]; // массив поля противника
char direction;//направление выстрела
struct User
{
	char login[strLength];
	char pass[strPass];
	int points = 0;
	int countGames = 0;
	int countVictory = 0;
};
struct Ship
{
	int xCoord, yCoord, size, health;
	bool gorizontal;
};
void showMenu(char mas[][20], int row, int punct) // функция отрисовки активного пункта
{
	for (size_t i = 0; i < row; i++)
	{
		SetCursor(menuX, menuY + i);
		if (i == punct)
		{
			SetColor(Magenta, White);
		}
		else SetColor(Black, White);
		cout << mas[i];
	}
	SetColor(Black, White);
}
int Menu(char mas[][20], int row) // функция меню
{
	int key, number = 0;
	do
	{
		showMenu(mas, row, number);
		key = _getch();
		switch (key)
		{
		case Up:
			if (number >= 1)
				number--;
			break;
		case Down:
			if (number < row - 1)
				number++;
			break;
		case Enter:
			return number;
			break;
		}
	} while (key != esc);
	return -1;
}
bool ChekPasword(User userName, User temp, FILE*& f)
{
	errno_t err = fopen_s(&f, "users.txt", "r");
	bool key = false;
	while (!feof(f))
	{
		fread(&temp, sizeof(User), 1, f);
		if (!_stricmp(userName.pass, temp.pass))
		{
			key = true;
		}
		if (key == true)
			return true;
	}
	return false;
}
bool Registration(User userName, User temp, FILE*& f)
{
	bool key = false;
	errno_t err = fopen_s(&f, "users.txt", "r");
	if (err == 0)
	{
		while (!feof(f))
		{
			fread(&temp, sizeof(User), 1, f);
			if (!_stricmp(userName.login, temp.login))
			{
				system("cls");
				key = true;
				SetCursor(ConsolMessageX, ConsolMessageY);
				cout << "Такой пользователь уже существует, введите другое имя.";
				Sleep(1000);
				fclose(f);
				return false;
			}
		}
		if (key == false)
		{
			fclose(f);
			fopen_s(&f, "users.txt", "a");
			if (f != nullptr)
			{
				SetCursor(menuX, menuY + 1);
				cout << "Пароль: ";
				gets_s(userName.pass, strPass);
				fwrite(&userName, sizeof(User), 1, f);
				SetCursor(ConsolMessageX, ConsolMessageY);
				cout << "Вы успешно зарегистрированы";
				Sleep(1000);
				system("cls");
				fclose(f);
				return true;
			}
		}
	}
	else
	{
		fopen_s(&f, "users.txt", "a");
		if (f != nullptr)
		{
			SetCursor(menuX, menuY + 1);
			cout << "Пароль: ";
			gets_s(userName.pass, strPass);
			fwrite(&userName, sizeof(User), 1, f);
			SetCursor(ConsolMessageX, ConsolMessageY);
			cout << "Вы успешно зарегистрировны";
			Sleep(1000);
			system("cls");
			fclose(f);
			return true;
		}
	}
}

int AutorizationLogin(User userName, User temp, FILE*& f)
{
	bool key = false;
	errno_t err = fopen_s(&f, "users.txt", "r");
	if (err == 0)
	{
		while (!feof(f))
		{
			fread(&temp, sizeof(User), 1, f);
			if (!_stricmp(userName.login, temp.login))
			{
				key = true;
			}
		}
		if (key == true)
			return true;
		else
			return false;
	}
	else
	{
		SetCursor(ConsolMessageX, ConsolMessageY);
		cout << "Зарегистрированных пользователей не найдено. Зарегистрируйтесь!";
		Sleep(1000);
		return -1;
	}
}
int memory(FILE* f, User*& mas)
{
	int length = 0;
	errno_t err = fopen_s(&f, "users.txt", "r");
	if (f != nullptr)
	{
		//чтобы работала эти функции файл должен быть открыт на чтение
		length = _filelength(_fileno(f)) / sizeof(User);
		if (length > 0)
		{
			mas = new User[length];

			for (size_t i = 0; i < length; i++)
			{
				fread(&mas[i], sizeof(User), 1, f);
			}
		}
		fclose(f);
	}
	return length;
}
template<typename Type>
void cleanMemory(Type**& mas, int row)
{
	if (mas != nullptr)
	{
		for (size_t i = 0; i < row; i++)
		{
			delete[]mas[i];
		}
		delete[]mas;
		mas = nullptr;
	}
}

template<typename Type>
void reMemory(Type**& mas, int row, int col)
{
	cleanMemory(mas, row);
	mas = new Type * [row];
	for (size_t i = 0; i < row; i++)
	{
		mas[i] = new Type[col];
	}
}
template<typename myType>
void SetShip(myType** ship, int sizeRow, int sizeCol) // заполнение динамического массива корабля
{
	for (size_t i = 0; i < sizeRow; i++)
	{
		for (size_t j = 0; j < sizeCol; j++)
		{
			ship[i][j] = '*';
		}
	}
}
void SetShip(bool gorizontal, int size, int x, int y, char field[][fieldW], int fieldX) // установка корабля на поле и отрисовка зоны вокруг корабля
{
	if (gorizontal)
	{
		for (int i = x - fieldX; i < x - fieldX + size * 2; i++)
		{
			field[y - fieldY][i] = '*';
		}
		for (size_t i = y - fieldY - 1; i < y - fieldY + 2; i++)
		{
			for (size_t j = x - fieldX - 2; j < x - fieldX + size * 2 + 2; j += 2)
			{
				if (field[i][j] == '-' || field[i][j] == '|' || field[i][j] == '*')
					continue;
				else
					field[i][j] = 'x';
			}
		}
	}
	else
	{
		for (size_t i = y - fieldY; i < y - fieldY + size; i++)
		{
			for (size_t j = x - fieldX; j < x - fieldX + 2; j++)
			{
				field[i][j] = '*';
			}
		}
		for (size_t i = y - fieldY - 1; i < y - fieldY + size + 1; i++)
		{
			for (size_t j = x - fieldX - 2; j < x - fieldX + 4; j += 2)
			{
				if (field[i][j] == '-' || field[i][j] == '|' || field[i][j] == '*')
					continue;
				else
					field[i][j] = 'x';
			}
		}
	}
}
void SetShip(Ship mas[AllCount], bool gorizontal, int size, int x, int y, int index) //заполнение структуры корабля
{
	mas[index].gorizontal = gorizontal;
	mas[index].size = size;
	mas[index].health = size;
	mas[index].xCoord = x;
	mas[index].yCoord = y;
}
template<typename Type>
void ShowBattleShips(int x, int y, Type** ship, int size, int sizeCol, bool gorizontal, Color color1) // отрисовка корабля
{
	SetCursor(x, y);
	SetColor(color1, color1);
	if (gorizontal)
	{
		for (size_t i = 0, s = 0; i < size; i++, s++)
		{
			if (i == 0)
			{
				SetCursor(x + i, y);
				for (size_t j = 0; j < sizeCol; j++)
				{
					cout << ship[i][j];
				}
			}
			else if (i > 0)
			{
				SetCursor(x + i + s, y);
				for (size_t j = 0; j < sizeCol; j++)
				{
					cout << ship[i][j];
				}
			}

		}
	}
	else
		for (size_t i = 0; i < size; i++)
		{
			SetCursor(x, y + i);

			for (size_t j = 0; j < sizeCol; j++)
			{
				cout << ship[i][j];
			}
		}
	SetColor(Black, White);
}
template<typename Type>
void ShowBattleShips(int x, int y, Type** ship, int size, int sizeCol, bool gorizontal, Color color1, Color color2, char field[][fieldW]) // функция закрашивания
{
	SetCursor(x, y);
	SetColor(color1, color1);
	if (gorizontal)
	{
		for (size_t i = 0, s = 0; i < size; i++, s++)
		{
			if (i == 0)
			{
				SetCursor(x + i, y);
				if (field[y - fieldY][x + i - fieldX] == '*')
				{
					SetColor(color1, color1);
				}
				else
					SetColor(color2, color2);
				for (size_t j = 0; j < sizeCol; j++)
				{
					cout << ship[i][j];
				}
			}
			else if (i > 0)
			{
				SetCursor(x + i + s, y);
				if (field[y - fieldY][x + i + s - fieldX] == '*')
				{
					SetColor(color1, color1);
				}
				else SetColor(color2, color2);
				for (size_t j = 0; j < sizeCol; j++)
				{
					cout << ship[i][j];
				}
			}

		}
	}
	else
		for (size_t i = 0; i < size; i++)
		{
			SetCursor(x, y + i);
			if (field[y + i - fieldY][x - fieldX] == '*')
			{
				SetColor(color1, color1);
			}
			else SetColor(color2, color2);

			for (size_t j = 0; j < sizeCol; j++)
			{
				cout << ship[i][j];
			}
		}
	SetColor(Black, White);
}
void ShiftShip(bool gorizontal, int& x, int& y, int size) //сдвиг корабля если пытаемся развернуть рядом с краем
{
	if (gorizontal)
	{
		if (x <= fieldW + fieldX && x >= fieldW + fieldX - size * 2)
		{
			x -= size * 2 - 2;
		}
	}
	else
	{
		if (y <= fieldH + fieldY && y >= fieldH + fieldY - size)
		{
			y -= size - 1;
		}
	}
}
void SetBattleField(char field[][fieldW])
{
	for (size_t i = 0; i < fieldH; i++)
	{
		for (size_t j = 0; j < fieldW; j++)
		{
			if (i == 0 || i == fieldH - 1)
			{
				field[i][j] = '-';

			}
			else if (j == 0 || j == fieldW - 1 || j == 1 || j == fieldW - 2)
			{
				field[i][j] = '|';
			}
			else
			{
				field[i][j] = 'w';
			}
		}
	}
}
void ShowBattleField(char field[][fieldW], Color colorShip, Color colorArea, int x, int y) // отрисовка поля
{
	for (size_t i = 0; i < fieldH; i++)
	{
		SetCursor(x, y + i);
		for (size_t j = 0; j < fieldW; j++)
		{
			if (i == 0 || i == fieldH - 1)
			{
				SetColor(Black, Black);
				cout << field[i][j];
			}
			else if (j == 0 || j == fieldW - 1 || j == 1 || j == fieldW - 2)
			{
				SetColor(Black, Black);
				cout << field[i][j];
			}
			else if (field[i][j] == 'x') // зона вокруг корабля при ручной установке
			{
				SetColor(colorArea, Blue);
				cout << field[i][j];
			}
			else if (field[i][j] == 'o') // промах
			{
				SetColor(Red, Blue);
				cout << field[i][j];
			}
			else if (field[i][j] == '#') // ранение
			{
				SetColor(Red, Red);
				cout << field[i][j];
			}
			else if (field[i][j] == '*') // живой корабль
			{
				SetColor(colorShip, colorShip);
				cout << field[i][j];
			}
			else
			{
				SetColor(Blue, Blue);
				cout << field[i][j];
			}
		}
	}
	SetColor(Black, White);
}
void clearStr(int x, int y) // стирание строки
{
	const int length = 200;
	int mas[length];

	for (size_t i = 0; i < length; i++)
	{
		mas[i] = 1;
	}
	SetCursor(x, y);
	SetColor(White, White);
	for (size_t i = 0; i < length; i++)
	{
		cout << mas[i];
	}
	SetColor(Black, White);
}
bool chekCrossing(char field[][fieldW], bool gorizontal, int x, int y, int size, int fieldX) // проверка на пересечение с другими кораблями
{
	if (gorizontal)
	{
		for (size_t i = 0; i < size * 2; i++)
		{
			if (field[y - fieldY][x - fieldX + i] == '*' || field[y - fieldY][x - fieldX + i] == 'x')
			{
				return true;
			}
		}
	}
	else
	{
		for (size_t i = 0; i < size; i++)
		{
			if (field[y - fieldY + i][x - fieldX] == '*' || field[y - fieldY + i][x - fieldX] == 'x')
			{
				return true;
			}
		}
	}
	return false;
}
void ChekPlacement(char field[][fieldW], int x, int y, int size, bool gorizontal, int& count, int& AllCount, int fieldX) // установка корабля и проверка
{
	if (count == 0)
	{
		SetCursor(0, 6);
		cout << "Все корабли этого типа расставлены, выберите другой.";
	}
	else
	{
		if (chekCrossing(field, gorizontal, x, y, size, fieldX) == true)
		{
			SetCursor(0, 6);
			cout << "Здесь нельзя разместить корабль.";
		}
		else
		{
			SetShip(gorizontal, size, x, y, field, fieldX);
			count--;
			AllCount--;
		}
	}
}
void SettingsBattleShips(Ship ShipPlayer[AllCount]) // ручная расстановка корабля
{
	int key, x = startX, y = startY;
	int AllCount = 10, countFlagman = 1, count3 = 2, count2 = 3, count1 = 4;
	int ShipNumber = 0;
	bool gorizontal = true;
	int size = 1, sizeCol = 2;
	reMemory(ship, size, sizeCol);
	do
	{	
		SetCursor(0, 0);
		cout << "Нажмите цифру, чтобы выбрать корабль: \n";
		cout << "1 - *\t\t x "<< count1<<"\n";
		cout << "2 - **\t\t x "<< count2<<"\n";
		cout << "3 - ***\t\t x "<<count3<<"\n";
		cout << "4 - ****\t x "<<countFlagman<<"\n";
		cout << "Клавиша 'Пробел', чтобы повернуть корабль\n";		
		ShowBattleField(field, Green, Red, fieldX, fieldY);
		ShowBattleShips(x, y, ship, size, sizeCol, gorizontal, Green);
		key = _getch();
		clearStr(0, 6);
		ShowBattleShips(x, y, ship, size, sizeCol, gorizontal, Green, Blue, field);
		switch (key)
		{
		case Up:
			if (y > fieldY + 1)
				y--;
			break;
		case Left:
			if (x > fieldX + 2)
				x -= 2;
			break;
		case Right:
			if (gorizontal)
			{
				if (ship == 0)
				{
					if (x < fieldW + fieldX - 3)
					{
						x += 2;
					}
				}
				else if (x < fieldW + fieldX - size * 2 - 2)
					x += 2;
			}
			else
			{
				if (x <= fieldW + fieldX - 6)
				{
					x += 2;
				}
			}
			break;
		case Down:
			if (gorizontal)
			{
				if (y < fieldH + fieldY - 2)
					y++;
			}
			else
			{
				if (y <= fieldH + fieldY - size - 2)
					y++;
			}
			break;
		case Enter:
			if (size == 1)
			{
				ChekPlacement(field, x, y, size, gorizontal, count1, AllCount, fieldX);
				SetShip(ShipPlayer, gorizontal, size, x, y, ShipNumber);				
				ShipNumber++;
			}
			else if (size == 2)
			{
				ChekPlacement(field, x, y, size, gorizontal, count2, AllCount, fieldX);
				SetShip(ShipPlayer, gorizontal, size, x, y, ShipNumber);				
				ShipNumber++;
			}
			else if (size == 3)
			{
				ChekPlacement(field, x, y, size, gorizontal, count3, AllCount, fieldX);
				SetShip(ShipPlayer, gorizontal, size, x, y, ShipNumber);				
				ShipNumber++;
			}
			else if (size == 4)
			{
				ChekPlacement(field, x, y, size, gorizontal, countFlagman, AllCount, fieldX);
				SetShip(ShipPlayer, gorizontal, size, x, y, ShipNumber);				
				ShipNumber++;
			}
			break;
		case Num1:
			cleanMemory(ship, size);
			size = 1;
			reMemory(ship, size, sizeCol);
			SetShip(ship, size, sizeCol);
			break;
		case Num2:
			cleanMemory(ship, size);
			size = 2;
			reMemory(ship, size, sizeCol);
			SetShip(ship, size, sizeCol);
			ShiftShip(gorizontal, x, y, size);
			break;
		case Num3:
			cleanMemory(ship, size);
			size = 3;
			reMemory(ship, size, sizeCol);
			SetShip(ship, size, sizeCol);
			ShiftShip(gorizontal, x, y, size);
			break;
		case Num4:
			cleanMemory(ship, size);
			size = 4;
			reMemory(ship, size, sizeCol);
			SetShip(ship, size, sizeCol);
			ShiftShip(gorizontal, x, y, size);
			break;
		case Space:
			if (gorizontal)
			{
				gorizontal = false;
				if (y <= fieldH + fieldY && y >= fieldH + fieldY - size)
				{
					y -= size - 1;
				}
			}
			else
			{
				gorizontal = true;
				if (x <= fieldW + fieldX && x >= fieldW + fieldX - size * 2)
				{
					x -= size * 2 - 2;
				}
			}
			break;
		}
		ShowBattleField(field, Green, Blue, fieldX, fieldY);
	} while (AllCount != 0);
}
void RandomCoord(int& x, int& y, int size, bool gorizontal, int StartCoordX) // рандомные координаты для авто установки кораблей
{
	if (gorizontal)
	{
		do
		{
			if (size == 1)
				x = StartCoordX + rand() % 19;
			else
				x = StartCoordX + rand() % (19 - size * 2);
		} while (x % 2 != 0);
		y = startY + rand() % 10;
	}
	else
	{
		do
		{
			x = StartCoordX + rand() % 19;
		} while (x % 2 != 0);
		if (size == 1)
			y = startY + rand() % 10;
		else
			y = startY + rand() % (10 - size);
	}
}
void AutoPlacement(char field[][fieldW], int fieldX, int fieldY, int StartCoordX, Color colorShips, Color Area, Ship Ship[AllCount]) // автоматическая расстановка кораблей
{
	int AllCount = 10, countFlagman = 1, count3 = 2, count2 = 3, count1 = 4;
	int x = 0, y = 0, size = 4;
	int ShipNumber = 0;
	bool gorizontal;
	do
	{
		gorizontal = rand() % 2;
		RandomCoord(x, y, size, gorizontal, StartCoordX);
		if (size == 4)
		{
			if (!chekCrossing(field, gorizontal, x, y, size, fieldX))
			{
				SetShip(gorizontal, size, x, y, field, fieldX); // установка кораблся
				countFlagman--;
				AllCount--;
				SetShip(Ship, gorizontal, size, x, y, ShipNumber);	// заполнение структуры	 		
				ShipNumber++;
				if (countFlagman == 0)
				{
					size--;
				}
			}
		}
		else if (size == 3)
		{
			if (!chekCrossing(field, gorizontal, x, y, size, fieldX))
			{
				SetShip(gorizontal, size, x, y, field, fieldX);
				count3--;
				AllCount--;
				SetShip(Ship, gorizontal, size, x, y, ShipNumber);				
				ShipNumber++;
				if (count3 == 0)
				{
					size--;
				}
			}
		}
		else if (size == 2)
		{
			if (!chekCrossing(field, gorizontal, x, y, size, fieldX))
			{
				SetShip(gorizontal, size, x, y, field, fieldX);
				count2--;
				AllCount--;
				SetShip(Ship, gorizontal, size, x, y, ShipNumber);				
				ShipNumber++;
				if (count2 == 0)
				{
					size--;
				}
			}
		}
		else if (size == 1)
		{
			if (!chekCrossing(field, gorizontal, x, y, size, fieldX))
			{
				SetShip(gorizontal, size, x, y, field, fieldX);
				SetShip(Ship, gorizontal, size, x, y, ShipNumber);				
				ShipNumber++;
				count1--;
				AllCount--;
			}
		}
		ShowBattleField(field, colorShips, Area, fieldX, fieldY);
	} while (AllCount != 0);
}
int Identification(int x, int y, char field[][fieldW], Ship Ships[AllCount])
{
	for (size_t i = 0; i < AllCount; i++)
	{
		if (Ships[i].gorizontal)
		{
			for (size_t j = 0; j < Ships[i].size * 2; j += 2)
			{
				if (Ships[i].xCoord + j == x && Ships[i].yCoord == y)
				{
					return i;
				}
			}
		}
		else
		{
			for (size_t j = 0; j < Ships[i].size; j++)
			{
				if (Ships[i].yCoord + j == y && Ships[i].xCoord == x)
				{
					return i;
				}
			}
		}
	}
}
void ShowArea(int x, int y, char field[][fieldW], Ship Ships[AllCount], int fieldX)
{
	int ShipNumber = Identification(x, y, field, Ships);
	if (Ships[ShipNumber].gorizontal)
	{
		for (size_t i = Ships[ShipNumber].yCoord - fieldY - 1; i < Ships[ShipNumber].yCoord - fieldY + 2; i++)
		{
			for (size_t j = Ships[ShipNumber].xCoord - fieldX - 2; j < Ships[ShipNumber].xCoord - fieldX + Ships[ShipNumber].size * 2 + 2; j += 2)
			{
				if (field[i][j] == 'x')
				{
					field[i][j] = 'o';
				}
			}
		}
	}
	else
	{
		for (size_t i = Ships[ShipNumber].yCoord - fieldY - 1; i < Ships[ShipNumber].yCoord - fieldY + Ships[ShipNumber].size + 1; i++)
		{
			for (size_t j = Ships[ShipNumber].xCoord - fieldX - 2; j < Ships[ShipNumber].xCoord - fieldX + 4; j += 2)
			{
				if (field[i][j] == 'x')
				{
					field[i][j] = 'o';
				}
			}
		}
	}
}
void AimingShotEnemy(int copyX, int copyY, int& x, int& y, int hit, Ship ShipsPlayer[AllCount], char& direction, bool miss) 
{
	bool gorizontal;
	if (hit == 1)
	{
		gorizontal = rand() % 2;
		if (gorizontal)
		{
			y = copyY;
			do
			{
				x = (copyX - 2) + rand() % 5;
			} while (x % 2 != 0);
			if (x > copyX)
				direction = 'r';
			if (x < copyX)
				direction = 'l';
		}
		else
		{
			x = copyX;
			y = (copyY - 1) + rand() % 3;
			if (y > copyY)
				direction = 'd';
			if (y < copyY)
				direction = 'u';

		}
	}
	else if (hit > 1)
	{
		y = copyY;
		x = copyX;
		if (direction == 'r')
		{
			if (miss || field[y - fieldY][x - fieldX + 2] == 'o' || field[y - fieldY][x - fieldX + 2] == '|')
			{
				x = firstHitX - 2;
				direction = 'l';
			}
			else
				x += 2;
		}
		else if (direction == 'l')
		{
			if (miss || field[y - fieldY][x - fieldX - 2] == 'o' || field[y - fieldY][x - fieldX - 2] == '|')
			{
				x = firstHitX + 2;
				direction = 'r';
			}
			else
				x -= 2;
		}
		else if (direction == 'd')
		{
			if (miss || field[y - fieldY + 1][x - fieldX] == 'o' || field[y - fieldY + 1][x - fieldX] == '-')
			{
				y = firstHitY - 1;
				direction = 'u';
			}
			else
				y++;
		}
		else if (direction == 'u')
		{
			if (miss || field[y - fieldY - 1][x - fieldX] == 'o' || field[y - fieldY - 1][x - fieldX] == '-')
			{
				y = firstHitY + 1;
				direction = 'd';
			}
			else
				y--;
		}
	}
}
void ShotEnemy(Ship ShipsPlayer[AllCount], bool hardMode, int& hit, bool& miss, int& count) // выстрел противника
{
	int ShipNumber = 0;
	int y, x;
	bool key = false;
	do
	{
		if (!hardMode)
		{
			y = startY + rand() % 10;
			do
			{
				x = startX + rand() % 19;
			} while (x % 2 != 0);
		}
		else
			if (hit > 0)
				AimingShotEnemy(copyX, copyY, x, y, hit, ShipsPlayer, direction, miss);
			else
			{
				y = startY + rand() % 10;
				do
				{
					x = startX + rand() % 19;
				} while (x % 2 != 0);
			}
		SetCursor(x, y);
		if (field[y - fieldY][x - fieldX] == 'o' || field[y - fieldY][x - fieldX] == '#' || field[y - fieldY][x - fieldX] == '|' || field[y - fieldY][x - fieldX] == '-')
			continue;
		else
		{
			if (field[y - fieldY][x - fieldX] == '*')
			{
				field[y - fieldY][x - fieldX] = '#';
				if (field[y - fieldY][x - fieldX + 1] == '*')
					field[y - fieldY][x - fieldX + 1] = '#';

				ShipNumber = Identification(x, y, field, ShipsPlayer);
				ShipsPlayer[ShipNumber].health--;
				if (ShipsPlayer[ShipNumber].health > 0)
				{
					SetCursor(EnemyShotX, EnemyShotY);
					SetColor(Red, White);
					cout << "Ваш корабль подбит.";
					miss = false;
					copyX = x;
					copyY = y;
					if (hit == 0)
					{
						firstHitX = x;
						firstHitY = y;
					}
					hit++;
				}
				else
				{
					SetCursor(EnemyShotX, EnemyShotY);
					SetColor(Red, White);
					cout << "Ваш корабль уничтожен.";
					ShowArea(x, y, field, ShipsPlayer, fieldX);
					firstHitX = 0;
					firstHitY = 0;
					hit = 0;
					count--;
					clearStr(PlayerCountMessageX, PlayerCountMessageY);
				}
				key = true;
			}
			else if (field[y - fieldY][x - fieldX] == 'x')
			{
				field[y - fieldY][x - fieldX] = 'o';
				{
					key = true;
					miss = true;
				}

			}
			else if (field[y - fieldY][x - fieldX] == 'w')
			{
				field[y - fieldY][x - fieldX] = 'o';
				{
					key = true;
					miss = true;
				}
			}
		}
	} while (key == false);
}

int Battle(int startX, Ship ShipsPlayer[AllCount], Ship ShipsEnemy[AllCount], bool hardMode, int& PlayerCount, int& EnemyCount) // выстрел игрока
{
	int x = startX, y = startY, key, hit = 0;
	int points = 0;
	int ShipNumber = 0;
	bool miss = false;
	do
	{		
		SetCursor(x, y);
		key = _getch();
		clearStr(EnemyShotX, EnemyShotY);
		clearStr(PlayerShotX, PlayerShotY);
		SetCursor(x, y);
		switch (key)
		{
		case Up:
			if (y > fieldY + 1)
				y--;
			break;
		case Left:
			if (x > startEnemyX)
				x -= 2;
			break;
		case Right:
			if (x < fieldEnemyX + fieldW - 4)
			{
				x += 2;
			}
			break;
		case Down:
			if (y < fieldH + fieldY - 2)
				y++;
			break;
		case Enter:
			if (fieldEnemy[y - fieldY][x - fieldEnemyX] == 'o' || fieldEnemy[y - fieldY][x - fieldEnemyX] == '#')
			{
				SetCursor(PlayerShotX, PlayerShotY);
				SetColor(Black, White);
				cout << "Вы уже стреляли по этим координатам.";
			}
			else
			{
				if (fieldEnemy[y - fieldY][x - fieldEnemyX] == 'w')
					fieldEnemy[y - fieldY][x - fieldEnemyX] = 'o';
				else if (fieldEnemy[y - fieldY][x - fieldEnemyX] == 'x')
					fieldEnemy[y - fieldY][x - fieldEnemyX] = 'o';
				else if (fieldEnemy[y - fieldY][x - fieldEnemyX] == '*')
				{
					fieldEnemy[y - fieldY][x - fieldEnemyX] = '#';
					if (fieldEnemy[y - fieldY][x - fieldEnemyX + 1] == '*')
						fieldEnemy[y - fieldY][x - fieldEnemyX + 1] = '#';

					ShipNumber = Identification(x, y, field, ShipsEnemy);
					ShipsEnemy[ShipNumber].health--;
					if (ShipsEnemy[ShipNumber].health > 0)
					{
						SetCursor(45, 6);
						SetColor(Green, White);
						cout << "Есть попадание.";
					}
					else
					{
						SetCursor(45, 6);
						SetColor(Green, White);
						cout << "Корабль противника уничтожен.";
						ShowArea(x, y, fieldEnemy, ShipsEnemy, fieldEnemyX);
						EnemyCount--;
						points += 100;
						clearStr(PlayerCountMessageX, PlayerCountMessageY);
					}
				}
				ShotEnemy(ShipsPlayer, hardMode, hit, miss, PlayerCount);
			}
			break;
		}
		ShowBattleField(field, Green, Blue, fieldX, fieldY);
		ShowBattleField(fieldEnemy, Blue, Blue, fieldEnemyX, fieldY);
		SetCursor(PlayerCountMessageX, PlayerCountMessageY);
		cout << "Осталось ваших кораблей: " << PlayerCount;
		SetCursor(EnemyCountMessageX, EnemyCountMessageY);
		cout << "Осталось кораблей противника: " << EnemyCount;
	} while (EnemyCount > 0 && PlayerCount > 0);
	return points;
}
void save(int list, User*& listUsers, User Player, FILE* f)
{
	for (size_t i = 0; i < list; i++)
	{
		if (!_stricmp(Player.login, listUsers[i].login))
		{
			listUsers[i].countGames += Player.countGames;
			listUsers[i].countVictory += Player.countVictory;
			listUsers[i].points += Player.points;
		}
	}
	if (f != nullptr) fclose(f);
	errno_t err = fopen_s(&f, "users.txt", "w");
	if (f != nullptr)
	{
		for (size_t i = 0; i < list; i++)
		{
			fwrite(&listUsers[i], sizeof(User), 1, f);
		}
		fclose(f);
	}
}
void sorting(User* list, int length)
{
	bool key;
	int kol = 0;
	do
	{
		key = false;
		for (size_t i = 0; i < length - kol - 1; i++)
		{
			if (list[i].points < list[i+1].points)
			{
				swap(list[i], list[i + 1]);
				key = true;
			}
		}
		kol++;
	} while (key == true);
}
void PlayerLose(User &Player)
{
	clearStr(PlayerShotX, PlayerShotY);
	clearStr(EnemyShotX, EnemyShotY);
	SetCursor(ConsolMessageX, ConsolMessageY);
	cout << "К сожалению вы проиграли, повезёт в другой раз!";
	Player.countGames++;
	SetCursor(ConsolMessageX, ConsolMessageY + 1);
	cout << "Вы заработали: " << Player.points << " очков.";
	cout << "Нажмите Enter, чтобы продолжить.";	
}
void PlayerWin(User &Player)
{
	clearStr(PlayerShotX, PlayerShotY);
	clearStr(EnemyShotX, EnemyShotY);
	SetCursor(ConsolMessageX, ConsolMessageY);
	cout << "Поздравляем вас с победой!\n";
	Player.points += 1000;
	Player.countGames++;
	Player.countVictory++;
	SetCursor(ConsolMessageX, ConsolMessageY + 1);
	cout << "Вам получаете 2000 очков.";
	cout << "Нажмите Enter, чтобы продолжить.";	
}
int main()
{
	SetConsoleCP(1251);//ввод кириллицы
	SetConsoleOutputCP(1251);//вывод кириллицы
	srand(time(NULL));
	system("cls");
	char RegistrationMenu[countPunctsMenu][strLength] = { "Регистрация", "Авторизация", "Выход" };
	char MainMenu[countPunctsMenu][strLength] = { "Начать игру", "Статистика", "Назад" };
	char DifficultLevel_Menu[countPunctsMenu][strLength] = { "Лёгкий", "Сложный", "Назад" };
	char PlacementShips[countPunctsMenu][strLength] = { "Автоматически", "Вручную", "Назад" };
	char EndMenu[countPunctsMenu][strLength] = { "Да", "Нет" };
	Ship ShipsPlayer[AllCount];
	Ship ShipsEnemy[AllCount];
	User Player;
	User temp;
	User* listUsers = nullptr;
	FILE* f{};	
	int punct = 0, button, list;
	int PlayerShipsCount, EnemyShipsCount;
	bool hardMode = false;
	bool key = false, exit = false, prev = false, battle = false, chek = false;
	do
	{
		do
		{
			do
			{
				key = false;
				system("cls");
				punct = Menu(RegistrationMenu, countPunctsMenu);
				if (punct == 0)
				{
					do
					{
						do
						{
							chek = false;
							system("cls");
							SetCursor(menuX, menuY);
							cout << "Логин: ";
							gets_s(Player.login, strLength);
							if (strlen(Player.login) < 1)
							{
								SetCursor(ConsolMessageX, ConsolMessageY);
								cout << "Нельзя ввести меньше одного символа";
								Sleep(1000);
								clearStr(ConsolMessageX, ConsolMessageY);
								chek = true;
							}
						} while (chek == true);

						if (Registration(Player, temp, f))
							key = true;
						else
							key = false;
					} while (key == false);
				}
				else if (punct == 1)
				{
					do
					{
						key = false;
						clearStr(ConsolMessageX, ConsolMessageY);
						system("cls");
						SetCursor(menuX, menuY);
						cout << "Логин: ";
						gets_s(Player.login, strLength);
						SetCursor(10, 6);
						if (AutorizationLogin(Player, temp, f))
						{
							fclose(f);
							SetCursor(menuX, menuY + 1);
							cout << "Пароль: ";
							gets_s(Player.pass, strPass);
							if (ChekPasword(Player, temp, f))
							{
								fclose(f);
								SetCursor(ConsolMessageX, ConsolMessageY);
								cout << "Вы успешно авторизовались.";
								Sleep(1000);
								key = true;
							}
							else
							{
								fclose(f);
								SetCursor(ConsolMessageX, ConsolMessageY);
								cout << "Неверный пароль.";
								Sleep(1000);
							}
						}
						else if (!AutorizationLogin(Player, temp, f))
						{
							fclose(f);
							SetCursor(ConsolMessageX, ConsolMessageY);
							cout << "Не верный логин, или такого пользователя не существует, зарегистрируйтесь";
							Sleep(1000);
							break;
						}
						else
						{
							fclose(f);
							break;
						}
					} while (key == false);
				}
				else
				{
					punct = -1;
					exit = true;
				}
			} while (punct != -1 && key != true);
			list = memory(f, listUsers);
			if (key == true)
			{
				do
				{
					system("cls");
					do
					{
						battle = false;
						prev = false;
						punct = Menu(MainMenu, countPunctsMenu);
						if (punct == 0) // Начать игру
						{
							do
							{
								hardMode = false;
								prev = false;
								system("cls");
								SetCursor(ConsolMessageX, ConsolMessageY);
								cout << "Выберите уровень сложности.";
								punct = Menu(DifficultLevel_Menu, 3);
								if (punct == 0) // Лёгкий
								{
									system("cls");
									SetCursor(ConsolMessageX, ConsolMessageY);
									cout << "Расставить корабли: \n";
									punct = Menu(PlacementShips, 3);
									if (punct == 0 || punct == 1)
									{
										system("cls");
										battle = true;
										break;
									}
									else if (punct == 2)
									{
										punct = 0;
										system("cls");
									}
								}
								else if (punct == 1) // сложный
								{
									system("cls");
									SetCursor(ConsolMessageX, ConsolMessageY);
									cout << "Расставить корабли: \n";
									hardMode = true;
									punct = Menu(PlacementShips, 3);
									if (punct == 0 || punct == 1)
									{
										system("cls");
										battle = true;
										break;
									}
									else if (punct == 2)
									{
										punct = 0;
										system("cls");
									}
								}
								else if (punct == 2) // назад
								{
									prev = true;
									system("cls");
								}
							} while (!prev);
							prev = false;
						}
						else if (punct == 1) // статистика
						{
							sorting(listUsers, list);
							system("cls");
							SetCursor(ConsolMessageX, ConsolMessageY);
							cout << "Статистика:\n";
							SetCursor(ConsolMessageX, ConsolMessageY + 1);
							cout << "\tИгрок\tСыграно игр\tПобеды\tОчки";
							for (size_t i = 0; i < list; i++)
							{
								SetCursor(ConsolMessageX, ConsolMessageY + i + 2);
								cout << i + 1 << ": \t" << listUsers[i].login << "\t\t" << listUsers[i].countGames << "\t" << listUsers[i].countVictory << "\t" << listUsers[i].points << "\n";
							}
							SetCursor(ConsolMessageX, ConsolMessageY + list + 2);
							cout << "Нажмите Esc, чтобы выйти в меню.";
							button = _getch();
							if (button == esc)
							{
								system("cls");
							}
						}
						else if (punct == 2)
							prev = true;
					} while (!battle && !prev);
					if (punct == 1)
					{
						SetBattleField(field);
						SetBattleField(fieldEnemy);
						PlayerShipsCount = 10;
						EnemyShipsCount = 10;
						SettingsBattleShips(ShipsPlayer);
						system("cls");
						ShowBattleField(field, Green, Blue, fieldX, fieldY);
						AutoPlacement(fieldEnemy, fieldEnemyX, fieldY, startEnemyX, Blue, Blue, ShipsEnemy);
						Player.points += Battle(startEnemyX, ShipsPlayer, ShipsEnemy, hardMode, PlayerShipsCount, EnemyShipsCount);
						if (PlayerShipsCount == 0)
						{
							PlayerLose(Player);							
							do
							{
								button = _getch();
							} while (button != Enter);
							system("cls");
						}
						else
						{
							PlayerWin(Player);							
							do
							{
								button = _getch();
							} while (button != Enter);
							system("cls");
						}
					}
					else if (punct == 0)
					{
						SetBattleField(field);
						SetBattleField(fieldEnemy);
						PlayerShipsCount = 10;
						EnemyShipsCount = 10;
						AutoPlacement(field, fieldX, fieldY, startX, Green, Blue, ShipsPlayer);
						AutoPlacement(fieldEnemy, fieldEnemyX, fieldY, startEnemyX, Blue, Blue, ShipsEnemy);
						Player.points += Battle(startEnemyX, ShipsPlayer, ShipsEnemy, hardMode, PlayerShipsCount, EnemyShipsCount);
						if (PlayerShipsCount == 0)
						{
							PlayerLose(Player);							
							do
							{
								button = _getch();
							} while (button != Enter);
							system("cls");
						}
						else
						{
							PlayerWin(Player);							
							do
							{
								button = _getch();
							} while (button != Enter);
							system("cls");
						}
					}
					if (!prev)
					{
						Sleep(1000);
						system("cls");
						SetCursor(ConsolMessageX, ConsolMessageY);
						cout << "Хотите сыграть ещё?";
						punct = Menu(EndMenu, countPunctsMenu);
						if (punct == 0)
						{
							save(list, listUsers, Player, f);
							Player.points = 0;
							Player.countGames = 0;
							Player.countVictory = 0;
							exit = false;
						}
						else
						{
							system("cls");
							SetCursor(ConsolMessageX, ConsolMessageY);
							cout << "Надеемся вы вернётесь!";
							Sleep(1000);
							exit = true;
						}
					}
				} while (!prev && !exit);
			}
			else
			{
				system("cls");
				SetCursor(ConsolMessageX, ConsolMessageY);
				cout << "Надеемся вы вернётесь!";
				Sleep(1000);
				exit = true;
			}

		} while (!exit);
		save(list, listUsers, Player, f);
	} while (!exit);
	system("pause>>NULL");
}