//---------------------------------------------------------------------------
//! @file   Shadowmap.h
//! @brief  シャドウマップ管理クラス
//---------------------------------------------------------------------------
#pragma once

//===========================================================================
//! シャドウマップ管理クラス
//===========================================================================
class Shadowmap : public Object
{
public:
    BP_OBJECT_DECL(Shadowmap, u8"シャドウマップ管理")

    //-------------------------------------------------
    //! @name Object継承クラス
    //-------------------------------------------------
    //!@{

    virtual bool Init() override;      //!< 初期化
    virtual void Update() override;    //!< 更新
    virtual void Exit() override;      //!< 終了
    virtual void GUI() override;       //!< GUI表示

    //!@}

    //!シャドウ描画開始
    static void begin();
    //!シャドウ終了
    static void end();

private:
    static inline u32    shadow_resolution_ = 2048;    //!< シャドウ解像度
    static inline float3 light_dir_         = normalize(float3(1.0f, 1.0f, 1.0f)); //光源の方向

    static inline std::shared_ptr<Texture> shadowmap_depth_;    //!< シャドウマップデプステクスチャー
    static inline std::shared_ptr<Texture> shadowmap_color_;    //!< シャドウマップカラーテクスチャー
};
