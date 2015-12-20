#include<iostream>
#include<string>
#include"DraughtsEngine.h"
int main()
{
    Gameplay gp(9);
    std::cout << "Color: ";
    std::string color;
    std::cin >> color;
    if (color == "white")
    {
        gp.SetColor(true);
    }
    else
    {
        gp.SetColor(false);
        ToStream(std::cout, gp);
        std::cout << "\nAI thinking..................\n";
        gp.AITurn();
    }
    do
    {
        ToStream(std::cout, gp);
        std::cout << "Enter [row col direction] ";
        int row, col;
        std::string dir;
        std::cin >> row >> col >> dir;
        Direction dirn;
        if (dir == "ru")
            dirn = Direction::RightUp;
        else if (dir == "rd")
            dirn = Direction::RightDown;
        else if (dir == "lu")
            dirn = Direction::LeftUp;
        else if (dir == "ld")
            dirn = Direction::LeftDown;
        else
            dirn = Direction::None;
        gp.PlayerTurn(row, col, dirn);
        ToStream(std::cout, gp);
        std::cout << "\nAI thinking..................\n";
        gp.AITurn();
    } while (true);



    system("pause");
	return 0;
}