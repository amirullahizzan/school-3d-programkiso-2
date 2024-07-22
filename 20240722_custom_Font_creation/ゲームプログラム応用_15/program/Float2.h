#pragma once

//---------------------------------------------------------------------------------
//	Float2 クラス
//---------------------------------------------------------------------------------
class Float2
{
public:
	float x;
	float y;

	Float2();									//	コンストラクタ
	Float2( float x, float y );

	void clear();								//	ゼロ初期化
	void set( float x, float y );				//	値のセット
	void set( Float2 &v );

	float GetLength();							//	長さ取得
	void normalize();							//	正規化
	void SetLength( float length );				//	長さ設定

	Float2& operator = ( const Float2 &v );		//	= 演算子のオーバーロード

	Float2& operator += ( const Float2 &v );	//	+= 演算子のオーバーロード
	Float2& operator -= ( const Float2 &v );	//	-= 演算子のオーバーロード
	Float2& operator *= ( const float f );		//	*= 演算子のオーバーロード
	Float2& operator /= ( const float f );		//	/= 演算子のオーバーロード
};

//	+ 演算子のオーバーロード
Float2 operator + ( const Float2 &v1, const Float2 &v2 );
//	- 演算子のオーバーロード
Float2 operator - ( const Float2 &v1, const Float2 &v2 );
//	* 演算子のオーバーロード
Float2 operator * ( const Float2 &v, const float f );
//	/ 演算子のオーバーロード
Float2 operator / ( const Float2 &v, const float f );

//	２つの Float2 の距離を求める
float GetFloat2Distance( Float2 &pos1, Float2 &pos2 );
//	２つの Float2 の内積を求める
float GetFloat2Dot( Float2 &v1, Float2 &v2 );
//	２つの Float2 の外積を求める
float GetFloat2Cross( Float2 &v1, Float2 &v2 );
