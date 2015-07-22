#include<iostream>
#include"draughts.h"
#define FSIZE 8
using namespace Draughts;

int main()
{
	Field<FSIZE> mf;
	WhitePiece* wp = new WhitePiece;
	BlackPiece* bp = new BlackPiece;
	mf('b', 4) = bp;
	mf('b', 2) = wp;

	std::cout << mf;

	RightMove<FSIZE> rm(mf);
	LeftMove<FSIZE> lm(mf);
	Eat<FSIZE> eat(mf);

	wp->Accept(&rm);
	std::cout << mf;

	bp->Accept(&eat);
	std::cout << mf;

	bp->Accept(&lm);
	std::cout << mf;

	system("pause");
	return 0;
}