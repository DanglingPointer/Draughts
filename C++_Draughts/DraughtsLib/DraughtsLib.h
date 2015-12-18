// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DRAUGHTSLIB_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DRAUGHTSLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DRAUGHTSLIB_EXPORTS
#define DRAUGHTSLIB_API __declspec(dllexport)
#else
#define DRAUGHTSLIB_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    DRAUGHTSLIB_API Gameplay *CreateGameplay(bool player_is_white, int depth);
    
    DRAUGHTSLIB_API Piece GetPieceAt(Gameplay *gp, byte row, byte col);
    
    DRAUGHTSLIB_API bool AITurn(Gameplay *gp);
    
    DRAUGHTSLIB_API bool PlayerTurn(Gameplay *gp, int row, int col, Direction dirn);
    
    DRAUGHTSLIB_API void Dispose(Gameplay *gp);

#ifdef __cplusplus
}
#endif