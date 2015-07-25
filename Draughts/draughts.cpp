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

	RightMove<FSIZE> rm(f);
	LeftMove<FSIZE> lm(f);
	LeftJump<FSIZE> lj(f);
	RightJump<FSIZE> rj(f);
	MoveFinder<FSIZE> mf(f);

	f('b', 4)->Accept(mf);
	std::cout << mf.RMovies().empty() << mf.LMovies().empty() << '\n';

	f('b', 4)->Accept(rm);
	std::cout << f;

	Field<FSIZE> f2(f);
	std::cout << f2;

	system("pause");
	return 0;
}