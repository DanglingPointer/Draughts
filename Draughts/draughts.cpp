#include<iostream>
#include"draughts.h"
#define FSIZE 10
using namespace Draughts;

int main()
{
	Field<FSIZE> f;
	std::cout << f;


	//WhitePiece* wp = new WhitePiece;
	//BlackPiece* bp = new BlackPiece;
	//f('b', 4) = bp;
	//f('b', 2) = wp;

	//std::cout << f;

	RightMove<FSIZE> rm(f);
	LeftMove<FSIZE> lm(f);
	LeftEat<FSIZE> leat(f);
	RightEat<FSIZE> reat(f);
	MoveFinder<FSIZE> det(f);

	//wp->Accept(rm);
	//std::cout << f;

	//bp->Accept(eat);
	//std::cout << f;

	//bp->Accept(lm);
	//std::cout << f;

	system("pause");
	return 0;
}