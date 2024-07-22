#include "Main.h"
#include "Game.h"
#include "Random.h"
#include "Hit.h"

#define LINE_MAX	5

const char* sample_text[LINE_MAX] = {
	"���b�Z�[�W�̕`�扉�o",
	"�X�y�[�X�L�[�ň�莞�ԗh�炷",
	"�I�����o�Ńo�[�̕\��",
	"�ړ��ɍ��킹�ē����x��ς���",
	"�E������I���s�𓯂����W�ɂ�����",
};

int font_handle;

//	���F��������̍��W
Float2 pos1;
//	���F�̕�����̍��W
Float2 pos2;
//	�ΐF�̕�����̍��W
Float2 pos3;
//	���F�̕�����̍��W
Float2 pos4;

int ox;
int oy;
//	�I�����Ă���s�̔ԍ�
int base_y;
int select_num;
int shake_frame = 0;
int text_w[LINE_MAX];
int bar_w[LINE_MAX];
//---------------------------------------------------------------------------------
//	����������
//---------------------------------------------------------------------------------
void GameInit()
{
	//	�t�H���g�f�[�^�̓ǂݍ���
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
//	�X�V����
//---------------------------------------------------------------------------------
void GameUpdate()
{
	//	�X�y�[�X�L�[���������物�F��������̗h��J�n
	if (PushHitKey(KEY_INPUT_SPACE))
	{

		shake_frame = 60;

	}

	shake_frame--;
	shake_frame = max(0, shake_frame);

	//	�㉺�L�[�őI��ԍ��̕ύX
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

	//	�ΐF�̕��������ʏ�Ɉړ�������
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
//	�`�揈��
//---------------------------------------------------------------------------------
void GameRender()
{
	//	���F��������̕`��
	for (int i = 0; i < LINE_MAX; i++) {

		float x = pos1.x + ox;
		float y = pos1.y + 20 * i + oy;

		DrawStringFToHandle(x, y, sample_text[i], GetColor(255, 255, 0), font_handle);
	}

	//	���F�̕�����̕`��
	for (int i = 0; i < LINE_MAX; i++) {

		float x = pos2.x;
		float y = pos2.y + 20 * i;

		DrawFillBox(x, y, x + bar_w[i], y + 20, GetColor(0, 0, 128));


		DrawStringFToHandle(x, y, sample_text[i], GetColor(0, 255, 255), font_handle);
	}


	//	�ΐF�̕�����̕`��
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

	//	���F�i�I�𕶎���j�̕�����̕`��
	for (int i = 0; i < LINE_MAX; i++) {

		//float x = pos4.x;
		float x = 750.0f - text_w[i];
		float y = pos4.y + 20 * i - base_y;

		int color = GetColor(160, 160, 160);
		if (i == select_num) 
		{
			//	�I��ԍ��̍s�̕����񂾂��F��ύX���Ă��܂�
			color = GetColor(255, 255, 255);
		}

		DrawStringFToHandle(x, y, sample_text[i], color, font_handle);
	}

	//	���F�̕�������E�����ɂ�����I���s�̂x���W�����߂�ۂ̖ڈ��ƂȂ�u<<�v�̕`��
	DrawStringFToHandle(750, 300, "<<", GetColor(255, 255, 255), font_handle);

	DrawString(16, 16, "���F�F�X�y�[�X�L�[�łP�b�Ԃ����h�炷", GetColor(255, 255, 255));
	DrawString(16, 32, "���F�F�㉺�L�[�őI�𕶎����؂�ւ��i���̕�����̏���������悤�ȃo�[�̕\���j", GetColor(255, 255, 255));
	DrawString(16, 48, "�ΐF�F���X�ɓ����x��ύX���Ȃ����ʏ�Ɉړ�", GetColor(255, 255, 255));
	DrawString(16, 64, "���F�F�E�����ɂ��āA�㉺�L�[�őI�𕶎����؂�ւ��i���̕����񂪁u<<�v�̏��ɗ���悤�Ɂj", GetColor(255, 255, 255));
}
//---------------------------------------------------------------------------------
//	�I������
//---------------------------------------------------------------------------------
void GameExit()
{
	DeleteFontToHandle(font_handle);
}
