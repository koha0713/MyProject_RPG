//=========================
// Main.cpp
//=========================
#include "Game.h"

//=========================
// エントリーポイント
//=========================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
	//=================
	// ゲームクラス生成
	//=================
	Game game;

	//=================
	// ゲーム実行
	//=================
	return game.Run();

}