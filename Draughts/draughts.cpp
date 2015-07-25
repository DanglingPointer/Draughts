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
	LeftJump<FSIZE> lj(f);
	RightJump<FSIZE> rj(f);
	MoveFinder<FSIZE> mf(f);

	f('b', 4)->Accept(mf); // error: ambiguous conversions from 'MoveFinder<10>' to 'Operation&'
	std::cout << mf.LMovies().empty();

	//wp->Accept(rm);
	//std::cout << f;

	//bp->Accept(eat);
	//std::cout << f;

	//bp->Accept(lm);
	//std::cout << f;

	system("pause");
	return 0;
}