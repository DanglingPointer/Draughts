// DraughtsLib.cpp : Defines the exported functions for the DLL application.
//
#include"DraughtsEngine.h"
#include "DraughtsLib.h"

// DLL entry point
//
BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

DRAUGHTSLIB_API Gameplay *CreateGameplay(bool player_is_white, int depth)
{
    return new Gameplay(player_is_white, depth);
}

DRAUGHTSLIB_API Piece GetPieceAt(Gameplay *gp, byte row, byte col)
{
    return gp->GetPieceAt(row, col);
}

DRAUGHTSLIB_API bool AITurn(Gameplay *gp)
{
    return gp->AITurn();
}

DRAUGHTSLIB_API bool PlayerTurn(Gameplay *gp, int row, int col, Direction dirn)
{
    return gp->PlayerTurn(row, col, dirn);
}

DRAUGHTSLIB_API void Dispose(Gameplay *gp)
{
    if (gp)
        delete gp;
}