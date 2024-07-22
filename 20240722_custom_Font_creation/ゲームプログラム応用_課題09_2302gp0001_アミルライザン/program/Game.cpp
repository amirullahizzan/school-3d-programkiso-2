#include "Main.h"
#include "Game.h"
#include "Random.h"
#include "Hit.h"

#define LINE_MAX	5

const char* sample_text[LINE_MAX] = {
	"メッセージの描画演出",
	"スペースキーで一定時間揺らす",
	"選択演出でバーの表示",
	"移動に合わせて透明度を変える",
	"右揃えや選択行を同じ座標にしたり",
};

int font_handle;

//	黄色い文字列の座標
Float2 pos1;
//	水色の文字列の座標
Float2 pos2;
//	緑色の文字列の座標
Float2 pos3;
//	白色の文字列の座標
Float2 pos4;

int ox;
int oy;
//	選択している行の番号
int base_y;
int select_num;
int shake_frame = 0;
int text_w[LINE_MAX];
int bar_w[LINE_MAX];
//---------------------------------------------------------------------------------
//	初期化処理
//---------------------------------------------------------------------------------
void GameInit()
{
	//	フォントデータの読み込み
	font_handle = LoadFontDataToHandle("data/sample.dft");

	pos1.set(50.0f, 100.0f);
	pos2.set(50.0f, 300.0f);
	pos3.set(450.0f, 200.0f);
	pos4.set(450.0f, 300.0f);

	for (int i = 0; i < LINE_MAX; i++)
	{
		text_w[i] = GetDrawStringWidthToHandle(sample_text[i], -1, font_handle);
	}
}
//---------------------------------------------------------------------------------
//	更新処理
//---------------------------------------------------------------------------------
void GameUpdate()
{
	//	スペースキーを押したら黄色い文字列の揺れ開始
	if (PushHitKey(KEY_INPUT_SPACE))
	{

		shake_frame = 60;

	}

	shake_frame--;
	shake_frame = max(0, shake_frame);

	//	上下キーで選択番号の変更
	if (PushHitKey(KEY_INPUT_DOWN))
	{
		select_num++;
	}
	if (PushHitKey(KEY_INPUT_UP))
	{
		select_num--;
	}

	select_num = max(0, min(select_num, LINE_MAX - 1));

	for (int i = 0; i < LINE_MAX; i++)
	{
		if (i == select_num)
		{
			bar_w[i] += 20;
		}
		else
		{
			bar_w[i] -= 20;
		}
		bar_w[i] = max(0, min(bar_w[i], text_w[i]));
	}

	//	緑色の文字列を画面上に移動させる
	pos3.y -= 1;
	if (pos3.y < 0.0f)
	{
		pos3.y = 200.0f;
	}

	if (base_y < select_num * 20 )
	{
		base_y += 2;
	}
	if (base_y > select_num * 20)
	{
		base_y -= 2;
	}
}
//---------------------------------------------------------------------------------
//	描画処理
//---------------------------------------------------------------------------------
void GameRender()
{
	//	黄色い文字列の描画
	for (int i = 0; i < LINE_MAX; i++) {

		float x = pos1.x + ox;
		float y = pos1.y + 20 * i + oy;

		DrawStringFToHandle(x, y, sample_text[i], GetColor(255, 255, 0), font_handle);
	}

	//	水色の文字列の描画
	for (int i = 0; i < LINE_MAX; i++) {

		float x = pos2.x;
		float y = pos2.y + 20 * i;

		DrawFillBox(x, y, x + bar_w[i], y + 20, GetColor(0, 0, 128));


		DrawStringFToHandle(x, y, sample_text[i], GetColor(0, 255, 255), font_handle);
	}


	//	緑色の文字列の描画
	for (int i = 0; i < LINE_MAX; i++)
	{
		float x = pos3.x;
		float y = pos3.y + 20 * i;

		int alpha = 255;
		if (y < 100)
		{
			alpha = 255 - (100 - y) * 10;
		}
		else if(y < 200)
		{
			alpha = (200 - y) * 10;

		}
		else
		{
			alpha = 0;
		}
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			DrawStringFToHandle(x, y, sample_text[i], GetColor(0, 255, 0), font_handle);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 1);
	}

	//	白色（選択文字列）の文字列の描画
	for (int i = 0; i < LINE_MAX; i++) {

		//float x = pos4.x;
		float x = 750.0f - text_w[i];
		float y = pos4.y + 20 * i - base_y;

		int color = GetColor(160, 160, 160);
		if (i == select_num) 
		{
			//	選択番号の行の文字列だけ色を変更しています
			color = GetColor(255, 255, 255);
		}

		DrawStringFToHandle(x, y, sample_text[i], color, font_handle);
	}

	//	白色の文字列を右揃えにしたり選択行のＹ座標を決める際の目安となる「<<」の描画
	DrawStringFToHandle(750, 300, "<<", GetColor(255, 255, 255), font_handle);

	DrawString(16, 16, "黄色：スペースキーで１秒間だけ揺らす", GetColor(255, 255, 255));
	DrawString(16, 32, "水色：上下キーで選択文字列を切り替え（その文字列の所が分かるようなバーの表示）", GetColor(255, 255, 255));
	DrawString(16, 48, "緑色：徐々に透明度を変更しながら画面上に移動", GetColor(255, 255, 255));
	DrawString(16, 64, "白色：右揃えにして、上下キーで選択文字列を切り替え（その文字列が「<<」の所に来るように）", GetColor(255, 255, 255));
}
//---------------------------------------------------------------------------------
//	終了処理
//---------------------------------------------------------------------------------
void GameExit()
{
	DeleteFontToHandle(font_handle);
}
