#include "Main.h"
#include "Game.h"
#include "Hit.h"

//---------------------------------------------------------------------------------
//	円同士の当たり判定（座標に Float2 を渡すバージョン）
//---------------------------------------------------------------------------------
bool CheckCircleHit( Float2 &center1, float radius1, Float2 &center2, float radius2 )
{
	float distance = GetFloat2Distance( center1, center2 );
	float radius = radius1 + radius2;
	if( distance <= radius ){
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------------
//	点と円の当たり判定（座標に Float2 を渡すバージョン）
//---------------------------------------------------------------------------------
bool CheckPointCircleHit( Float2 &point, Float2 &center, float radius )
{
	float distance = GetFloat2Distance( point, center );
	if( distance <= radius ){
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------------
//	四角同士の当たり判定（座標と大きさに Float2 を渡すバージョン）
//---------------------------------------------------------------------------------
bool CheckBoxHit( Float2 &box_pos1, Float2 &box_size1, Float2 &box_pos2, Float2 &box_size2 )
{
	if( box_pos1.x + box_size1.x >= box_pos2.x && box_pos1.x <= box_pos2.x + box_size2.x ){
		if( box_pos1.y + box_size1.y >= box_pos2.y && box_pos1.y <= box_pos2.y + box_size2.y ){
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------------
//	点と四角の当たり判定（座標と大きさに Float2 を渡すバージョン）
//---------------------------------------------------------------------------------
bool CheckPointBoxHit( Float2 &point, Float2 &box_pos, Float2 &box_size )
{
	if( point.x >= box_pos.x && point.x <= box_pos.x + box_size.x ){
		if( point.y >= box_pos.y && point.y <= box_pos.y + box_size.y ){
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------------------
//	点と三角形の当たり判定
//---------------------------------------------------------------------------------
bool CheckPointTriangleHit( Float2 &point, Float2 &triangle_pos1, Float2 &triangle_pos2, Float2 &triangle_pos3 )
{
	Float2 dir1 = triangle_pos1 - point;
	Float2 dir2 = triangle_pos2 - point;
	Float2 dir3 = triangle_pos3 - point;

	float cross1 = GetFloat2Cross( dir1, dir2 );
	float cross2 = GetFloat2Cross( dir2, dir3 );
	float cross3 = GetFloat2Cross( dir3, dir1 );

	if( cross1 < 0.0f && cross2 < 0.0f && cross3 < 0.0f ){
		return true;
	}
	if( cross1 > 0.0f && cross2 > 0.0f && cross3 > 0.0f ){
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------------
//	線分と点の一番近い座標を取得する
//---------------------------------------------------------------------------------
Float2 GetFloat2LinePointNearPosition( Float2 &line_start, Float2 &line_goal, Float2 &point )
{
	Float2 line_dir = line_goal - line_start;
	line_dir.normalize();

	Float2 point_dir = point - line_start;

	float line_dot = GetFloat2Dot( line_dir, point_dir );

	Float2 near_dir = line_dir * line_dot;

	Float2 near_pos = line_start + near_dir;

	Float2 dir1 = line_start - near_pos;

	if( GetFloat2Dot( line_dir, dir1 ) > 0.0f ){
		near_pos = line_start;
	}

	Float2 dir2 = line_goal - near_pos;

	if( GetFloat2Dot( line_dir, dir2 ) < 0.0f ){
		near_pos = line_goal;
	}

	return near_pos;
}
//---------------------------------------------------------------------------------
//	線分と点の一番近い距離を取得する
//---------------------------------------------------------------------------------
float GetFloat2LinePointNearDistance( Float2 &line_start, Float2 &line_goal, Float2 &point )
{
	Float2 near_pos = GetFloat2LinePointNearPosition( line_start, line_goal, point );

	float near_distance = GetFloat2Distance( point, near_pos );

	return near_distance;
}
//---------------------------------------------------------------------------------
//	線分と円の当たり判定
//---------------------------------------------------------------------------------
bool CheckLineCircleHit( Float2 &line_start, Float2 &line_goal, Float2 &circle_pos, float circle_radius )
{
	float near_distance = GetFloat2LinePointNearDistance( line_start, line_goal, circle_pos );

	if( near_distance <= circle_radius ){
		return true;
	}
	return false;
}

//*********************************************************************************
//	今まで使っていた float を渡すバージョン
//*********************************************************************************
//---------------------------------------------------------------------------------
//	２点の長さを取得する関数
//---------------------------------------------------------------------------------
float GetDistance( float x1, float y1, float x2, float y2 )
{
	float x = x1 - x2;
	float y = y1 - y2;
	float distance = sqrtf( x * x + y * y );
	return distance;
}
//---------------------------------------------------------------------------------
//	円同士の当たり判定
//---------------------------------------------------------------------------------
bool CheckCircleHit( float x1, float y1, float r1, float x2, float y2, float r2 )
{
	float distance = GetDistance( x1, y1, x2, y2 );
	float radius = r1 + r2;
	if( distance <= radius ){
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------------
//	点と円の当たり判定
//---------------------------------------------------------------------------------
bool CheckPointCircleHit( float point_x, float point_y, float circle_x, float circle_y, float circle_r )
{
	float distance = GetDistance( point_x, point_y, circle_x, circle_y );
	if( distance <= circle_r ){
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------------
//	四角同士の当たり判定
//---------------------------------------------------------------------------------
bool CheckBoxHit( float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2 )
{
	if( x1 + w1 >= x2 && x1 <= x2 + w2 ){
		if( y1 + h1 >= y2 && y1 <= y2 + h2 ){
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------------
//	点と四角の当たり判定
//---------------------------------------------------------------------------------
bool CheckPointBoxHit( float point_x, float point_y, float box_x, float box_y, float box_w, float box_h )
{
	if( point_x >= box_x && point_x <= box_x + box_w ){
		if( point_y >= box_y && point_y <= box_y + box_h ){
			return true;
		}
	}
	return false;
}
