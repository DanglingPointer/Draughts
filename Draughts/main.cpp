#include"basic_classes.h"
#include"engine.h"
//#define FSIZE 10

using namespace Draughts;

int main()
{
	Field<FSIZE> f;
	std::cout << f;


	WhiteKing* wp = new WhiteKing;
	BlackKing* bp = new BlackKing;
	wp->dirn = UP;
	bp->dirn = DOWN;
	f('g', 4) = wp;
	f('g', 5) = bp;
	std::cout << f;

	RightMove<FSIZE> rm(f);
	LeftMove<FSIZE> lm(f);
	LeftJump<FSIZE> lj(f);
	RightJump<FSIZE> rj(f);
	MoveFinder<FSIZE> mf(f);

	f('g', 4)->Accept(rm);
	f('g', 5)->Accept(lm);
	std::cout << f;
	f.Reset();
	std::cout << f;
	//---


	//f('b', 4)->Accept(mf);
	//std::cout << mf.RMovies().empty() << mf.LMovies().empty() << '\n';

	//f('b', 4)->Accept(rm);
	//std::cout << f;

	//Field<FSIZE> f2(f);
	//std::cout << f2;

	//CmdPlay play;
	//play.Run();

	system("pause");
	return 0;
}