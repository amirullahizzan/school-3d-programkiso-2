//---------------------------------------------------------------------------
//! @file   Shadowmap.cpp
//! @brief  シャドウマップ管理クラス
//---------------------------------------------------------------------------
#include "Shadowmap.h"

//---------------------------------------------------------------------------
//! 初期化
//---------------------------------------------------------------------------
bool Shadowmap::Init()
{
    //----------------------------------------------------------
    // シャドウマップテクスチャを作成
    //----------------------------------------------------------
    u32 resolution   = shadow_resolution_;
    shadowmap_depth_ = std::make_shared<Texture>(resolution, resolution, DXGI_FORMAT_D32_FLOAT);
    shadowmap_color_ = std::make_shared<Texture>(resolution, resolution, DXGI_FORMAT_R32_FLOAT);    //Red

    //----------------------------------------------------------
    // シャドウマップのバッファーの描画初期化を登録
    //----------------------------------------------------------

    auto beginFunc = [&]() { begin(); };
    auto endFunc   = [&]() { end(); };

    //次回、setting up in ComponentModel 描画
    SetProc("Shadowmap::start", beginFunc, ProcTiming::Shadow, ProcPriority::HIGHEST);
    SetProc("Shadowmap::end", endFunc, ProcTiming::Shadow, ProcPriority::LOWEST);

    return Super::Init();
}

//---------------------------------------------------------------------------
//! 更新
//---------------------------------------------------------------------------
void Shadowmap::Update()
{

}

//---------------------------------------------------------------------------
//! 終了
//---------------------------------------------------------------------------
void Shadowmap::Exit()
{
}

//---------------------------------------------------------------------------
//! GUI表示
//---------------------------------------------------------------------------
void Shadowmap::GUI()
{
    Super::GUI();
}

//---------------------------------------------------------------------------
//! シャドウ描画開始
//---------------------------------------------------------------------------
void Shadowmap::begin()
{
    //------------------------------------------------
    //シャドウ用おでオプスバッファーを設定
    //------------------------------------------------
    //※DxLibの仕様でカラーバッファを両方指定しておかないといけない
    //カラーバッファを設定しておく
    SetRenderTarget(
        shadowmap_color_.get(),
        shadowmap_depth_
            .get());    //this sets the render target to not be on the Scene, but instead on Shadow Renderers (2048 x 2048 instead of usual Window screen)

    //デップスバッファをクリア
    ClearDepth(1.0f);    // 1.0f = 無限遠
    //Clearing above furthest point
    //We want the futhest point behind the shadow (Depth buffer)

    //------------------------------------------------
    //カメラを光源位置から光源方向に向いて設定
    //------------------------------------------------
    float3 center_position = float3(0, 0, 0);    //カメラのLookAt
    float  range           = 10.0f;              //影を撮影範囲(±10.0m)
    float  height          = 100.0f;             //カメラの高度(±100.0m) Distance from Look at -> Camera pos

    //ビュー行列 （方向と位置）
    float3 look_at  = center_position;
    float3 position = look_at - light_dir_ * height;    //高さぶん、　上に移動した場所 (逆方向かける高さ）

    matrix mat_view = matrix::lookAtLH(position, look_at);

    //投影行列　(影の範囲)
    //平行投影 (Used for measuring camera) / Parallel Projection

    //Compressing into the SS
    float  height_margin = 100.0f;                                                                         //Increasing further distance
    float  scale_x       = 1.0f / range;
    float  scale_y       = 1.0f / range;
    float  scale_z       = height + height_margin; //As long as it hits a ground it doesnt matter, the further doesnt matter

    matrix mat_proj      = matrix::scale(float3(scale_x, scale_y, scale_z));    //1.0f is used to inverse
    //if only using height in Z here, will not enough because it wont reach renders behind Look At

    //設定
    DxLib::SetCameraViewMatrix(mat_view);
    DxLib::SetupCamera_ProjectionMatrix(mat_proj);
}

//---------------------------------------------------------------------------
//! シャドウ描画終了
//---------------------------------------------------------------------------
void Shadowmap::end()
{
    //デプスバッファをカラーバッファーにコピーする
    //-------------------------------------------------
    //DxLibの仕様でデップスバッファをテクスチャーとして利用できない仕様がある
    // ただし、DirectX11のAPIでデップスバッファか  →  カラーバッファコピーはできる
    // コピー先のからーバッファはDxLibでできる
    //-------------------------------------------------

    //-------------------------------------------------
    //RenderTargetを元に戻す
    //-------------------------------------------------
    //Resetting into the original buffer
    SetRenderTarget(GetHdrBuffer(),GetDepthStencil());
}
