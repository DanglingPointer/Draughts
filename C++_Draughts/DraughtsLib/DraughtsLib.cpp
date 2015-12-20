// DraughtsLib.cpp : Defines the exported functions for the DLL application.

#include<iostream>
#include"DraughtsEngine.h"
#include "DraughtsLib.h"

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
    //switch (ul_reason_for_call)
    //{
    //case DLL_PROCESS_ATTACH:
    //case DLL_THREAD_ATTACH:
    //case DLL_THREAD_DETACH:
    //case DLL_PROCESS_DETACH:
    //    break;
    //}
    return TRUE;
}

Gameplay *pGame = NULL;

DRAUGHTSLIB_API void CreateGameplay(int depth)
{
    if (pGame)
        delete pGame;
    pGame = new Gameplay(depth);
}

DRAUGHTSLIB_API void SetPlayerColro(BOOL white)
{
    pGame->SetColor(white);
}

DRAUGHTSLIB_API Piece GetPieceAt(byte row, byte col)
{
    return pGame->GetPieceAt(row, col);
}

DRAUGHTSLIB_API BOOL AITurn()
{
    return (int)(pGame->AITurn());
}

DRAUGHTSLIB_API BOOL PlayerTurn(int row, int col, Direction dirn)
{
    return (int)(pGame->PlayerTurn(row, col, dirn));
}

DRAUGHTSLIB_API void Dispose()
{
    if (pGame)
        delete pGame;
}

DRAUGHTSLIB_API void Print()
{
    ToStream(std::cout, *pGame);
}