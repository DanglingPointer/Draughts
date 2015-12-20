#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DRAUGHTSLIB_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DRAUGHTSLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DRAUGHTSLIB_EXPORTS
# define DRAUGHTSLIB_API __declspec(dllexport)
#else
# define DRAUGHTSLIB_API __declspec(dllimport)
#endif

#ifndef DRAUGHTS_TYPES
#define DRAUGHTS_TYPES

typedef unsigned char byte;
typedef char sbyte;
typedef enum _Piece : byte
{
    Empty = 0x0,
    White = 0x1,
    Black = 0x2,
    King = 0x4
} Piece;
typedef enum _Direction : byte
{
    None = 0x0,
    RightUp = 0x1,
    LeftUp = 0x2,
    RightDown = 0x4,
    LeftDown = 0x8
} Direction;

#endif

#ifdef __cplusplus
extern "C"
{
#endif

    DRAUGHTSLIB_API void CreateGameplay(int depth);

    DRAUGHTSLIB_API void SetPlayerColro(BOOL white);
    
    DRAUGHTSLIB_API Piece GetPieceAt(byte row, byte col);
    
    DRAUGHTSLIB_API BOOL AITurn();
    
    DRAUGHTSLIB_API BOOL PlayerTurn(int row, int col, Direction dirn);
    
    DRAUGHTSLIB_API void Dispose();

    DRAUGHTSLIB_API void Print();

#ifdef __cplusplus
}
#endif