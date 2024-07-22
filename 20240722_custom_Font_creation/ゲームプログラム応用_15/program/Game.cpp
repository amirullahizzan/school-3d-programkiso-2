#include "Main.h"
#include "Game.h"
#include "Random.h"
#include "Hit.h"

//const char *sample_text = "色々なフォントで文字列の描画";
const char *sample_text = "Windowsでコンピューターの世界";

int font_handle1;
int font_handle2;
int size_x;
int size_y;


//---------------------------------------------------------------------------------
//	初期化処理
//---------------------------------------------------------------------------------
void GameInit()
{
	SetFontSize(50);
	font_handle1 = CreateFontToHandle( "メイリオ", 50, -1 );
	//font_handle2 = LoadFontDataToHandle("data/sample.dft");
	font_handle2 = LoadFontDataToHandle("data/sample2.dft");

	GetDrawStringSizeToHandle(&size_x, &size_y,NULL,sample_text,-1,font_handle2);
}
//---------------------------------------------------------------------------------
//	更新処理
//---------------------------------------------------------------------------------
void GameUpdate()
{
}
//---------------------------------------------------------------------------------
//	描画処理
//---------------------------------------------------------------------------------
void GameRender()
{
	//DrawString( 50, 50, sample_text, GetColor( 255, 255, 255 ) );

	//DrawStringToHandle( 50, 150, sample_text, GetColor( 255, 255, 0 ), font_handle1 );
	DrawStringToHandle( 50, 250, sample_text, GetColor( 0, 255, 255), font_handle2 );

	DrawLine(50,250+size_y,50+size_x,250+size_y,GetColor(0,255,255), 3);
}
//---------------------------------------------------------------------------------
//	終了処理
//---------------------------------------------------------------------------------
void GameExit()
{
	DeleteFontToHandle( font_handle1 );
	DeleteFontToHandle( font_handle2 );
}
