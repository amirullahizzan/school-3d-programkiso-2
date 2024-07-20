//---------------------------------------------------------------------------
//! @file   SceneShader.cpp
//! @brief  シェーダーサンプルシーン
//---------------------------------------------------------------------------
#include "SceneShader.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentFilterFade.h>

namespace {

int SmokeHandle = -1;

struct Particle {
    float3 position_;    //!< 位置座標
    float3 velocity_;    //!< 速度
    float  angle_;
    float  angularVelocity_;

    float life_;    //!< 寿命
};

//! 0.0f～1.0fの乱数値を取得
float frand()
{
    return static_cast<float>(rand()) * (1.0f / static_cast<float>(RAND_MAX));
}

//! -1.0f～+1.0fの乱数値を取得
float sfrand()
{
    return frand() * 2.0f - 1.0f;
}

struct ParticleSystem : noncopyable {
public:
    ParticleSystem() = default;

    virtual ~ParticleSystem() = default;

    void Emit()
    {
        float3 pos      = float3(0, 0, 0);
        float3 velocity = float3(sfrand(), 1.0f, sfrand()) * 0.05f;
        float  life     = 10.0f;

        Particle p{};
        p.position_        = pos;
        p.velocity_        = velocity;
        p.life_            = life;
        p.angle_           = frand() * 2.0f * 3.141592f;
        p.angularVelocity_ = sfrand() * 0.1f;

        particles_.emplace_back(std::move(p));
    }

    void update()
    {
        float deltaTime = GetDeltaTime();

        for(u32 i = 0; i < particles_.size(); i++) {
            auto& p      = particles_[i];
            p.position_ += p.velocity_;
            p.life_     -= deltaTime;
            p.angle_    += p.angularVelocity_;

            if(p.life_ <= 0.0f) {
                particles_[i] = particles_.back();    //move it in the back,

                particles_.pop_back();    //erase the back vector

                i -= 1;
            }
        }

        //slow because it has to shift all vectors
        //  for (auto it = particles_.begin(); it != particles_.end();)
        //  {
        //
        //      if (it->life_ <= 0.0f)
        //      {
        //          it = particles_.erase(it);
        //      }
        //      else
        //      {
        //          it++;
        //      }
        //  }
    }

    void render()
    {
        DxLib::SetWriteZBufferFlag(false);    //affect next lines

        matrix mat_view         = cast(DxLib::GetCameraViewMatrix());    // CAMERA VIEW matrix
        matrix mat_camera_world = inverse(mat_view);                     //reverse matrix. CAMERA VIEW MATRIX reversed = CAMERA WORLD matrix

        float3 camera_position = mat_camera_world.translate();
        float3 front           = mat_camera_world.axisZ();

        auto depthSortPred = [&](const Particle& a, const Particle& b)

        {
            float3 dir_a = camera_position - a.position_;
            float3 dir_b = camera_position - b.position_;

            float depth_a = dot(dir_a, dir_a);    // 長さの２乗　＝　aまでの距離の２乗
            float depth_b = dot(dir_b, dir_b);    // 長さの２乗　＝　bまでの距離の２乗

            //float depth_a = dot(a.position_, front);
            //float depth_b = dot(a.position_, front);

            return depth_a > depth_b;
            //return a.position_.z > b.position_.z; };
        };

        std::sort(particles_.begin(), particles_.end(), depthSortPred);

        for(auto& p : particles_) {
            float angle = p.angle_;

            float centerU = 0.5f;
            float centerV = 0.5f;
            float size    = 3.0f + (5.0f - p.life_);

            //angle += 0.1f;

            DrawBillboard3D(cast(p.position_), centerU, centerV, size, angle, SmokeHandle, true);
        }
        DxLib::SetWriteZBufferFlag(true);    //affect next lines
    }

private:
    std::vector<Particle> particles_;
};
ParticleSystem particleSystem;


std::shared_ptr<Texture> iblDiffuseTexture = nullptr;
std::shared_ptr<Texture> iblSpecularTexture = nullptr;

}    // namespace

Texture* getIblDiffuseTexture()
{
    return iblDiffuseTexture.get();
}

Texture* getIblSpecularTexture()
{
    return iblSpecularTexture.get();
}

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! 初期化
//---------------------------------------------------------------------------
bool SceneShader::Init()
{
    auto obj = Scene::CreateObjectPtr<Object>();

    // モデルコンポーネント
    model_ = obj->AddComponent<ComponentModel>("data/Sample/Player/model.mv1");

    iblDiffuseTexture = std::make_shared<Texture>("data/ibl/iblDiffuseHDR.dds");
    iblSpecularTexture = std::make_shared<Texture>("data/ibl/iblSpecularHDR.dds");

    

    {
        // for(int i = 0; i < 2; i++) {
        //     auto obj = Scene::CreateObjectPtr<Object>();
        //
        //     // モデルコンポーネント
        //     model_ = obj->AddComponent<ComponentModel>("data/Sample/Player/model.mv1");
        // }
    }

    // カメラコンポーネント
    auto camera = obj->AddComponent<ComponentCamera>();
    camera->SetPerspective(60.0f);
    camera->SetPositionAndTarget({0.f, 6.f, -15.f}, {0.f, 5.f, 0.f});
    // camera->SetCurrentCamera(); //< こちらは1つめのカメラの場合は必要ありません

    // フェードコンポーネント
    filter_fade_ = obj->AddComponent<ComponentFilterFade>();

    // 画像の読み込み
    texture_ = std::make_shared<Texture>("data/Shader/seafloor.dds");

    // 頂点シェーダー
    shader_vs_ = std::make_shared<ShaderVs>("data/Shader/vs_3d");

    // ピクセルシェーダー
    shader_ps_    = std::make_shared<ShaderPs>("data/Shader/ps_texture");
    shader_ps_3d_ = std::make_shared<ShaderPs>("data/Shader/ps_3d_texture");

    SmokeHandle = DxLib::LoadGraph("data/smoke.png");

    return true;
}

//---------------------------------------------------------------------------
//! 更新
//! @param  [in]    delta   経過時間
//---------------------------------------------------------------------------
void SceneShader::Update()
{
    f32 delta = GetDeltaTime();

    // Y軸中心の回転
    static f32 ry  = 0.0f;
    ry            += 0.5f * delta;

    model_->Matrix() = mul(matrix::scale(0.1f), matrix::rotateY(ry));

    particleSystem.Emit();

    //Update particles
    particleSystem.update();
}

//---------------------------------------------------------------------------
//! 描画
//---------------------------------------------------------------------------
void SceneShader::Draw()
{
    DrawFormatString(100, 50, GetColor(255, 255, 255), "ShaderDemo");

    //using slot 0~15 
    //DxLib::SetUseTextureToShader(14, *iblDiffuseTexture.get());
    //DxLib::SetUseTextureToShader(15, *iblSpecularTexture.get());

    //==========================================================
    // プリミティブの描画
    //==========================================================

    SetDrawMode(DX_DRAWMODE_BILINEAR);            // テクスチャをバイリニア補間
    SetTextureAddressMode(DX_TEXADDRESS_WRAP);    // テクスチャを繰り返し

    //----------------------------------------------------------
    // 床の描画
    //----------------------------------------------------------
    {
        VERTEX3DSHADER v[4]{};

        constexpr f32 SIZE = 5.0f;

        // 頂点データの準備
        v[0].pos = {-SIZE, 0.0f, +SIZE};
        v[1].pos = {+SIZE, 0.0f, +SIZE};
        v[2].pos = {-SIZE, 0.0f, -SIZE};
        v[3].pos = {+SIZE, 0.0f, -SIZE};
        v[0].dif = GetColorU8(255, 255, 255, 255);
        v[1].dif = GetColorU8(255, 255, 255, 255);
        v[2].dif = GetColorU8(255, 255, 255, 255);
        v[3].dif = GetColorU8(255, 255, 255, 255);
        v[0].u   = 0.0f;
        v[0].v   = 0.0f;
        v[1].u   = 2.0f;
        v[1].v   = 0.0f;
        v[2].u   = 0.0f;
        v[2].v   = 2.0f;
        v[3].u   = 2.0f;
        v[3].v   = 2.0f;

        // 使用するテクスチャを設定 (slot=0)
        SetUseTextureToShader(0, *texture_);

        // 使用する頂点シェーダーを設定
        SetUseVertexShader(*shader_vs_);

        // 使用するピクセルシェーダーを設定
        SetUsePixelShader(*shader_ps_3d_);

        // 描画
        DrawPrimitive3DToShader(v, 4, DX_PRIMTYPE_TRIANGLESTRIP);
    }
    // 使い終わったらテクスチャ設定を解除
    // 解除しておかないとモデル描画に影響あり。
    SetUseTextureToShader(0, -1);

    //----------------------------------------------------------
    // 2Dの描画
    //----------------------------------------------------------
    {
        VERTEX2DSHADER v[4]{};

        constexpr u32 SIZE = 256;

        // 頂点データの準備
        v[0].pos = {0.0f, 0.0f, 0.0f};
        v[1].pos = {SIZE, 0.0f, 0.0f};
        v[2].pos = {0.0f, SIZE, 0.0f};
        v[3].pos = {SIZE, SIZE, 0.0f};
        v[0].rhw = 1.0f;    // rhw = 1.0f 初期化は2D描画に必須
        v[1].rhw = 1.0f;
        v[2].rhw = 1.0f;
        v[3].rhw = 1.0f;
        v[0].dif = GetColorU8(255, 255, 255, 255);
        v[1].dif = GetColorU8(255, 255, 255, 255);
        v[2].dif = GetColorU8(255, 255, 255, 255);
        v[3].dif = GetColorU8(255, 255, 255, 255);
        v[0].u   = 0.0f;
        v[0].v   = 0.0f;
        v[1].u   = 1.0f;
        v[1].v   = 0.0f;
        v[2].u   = 0.0f;
        v[2].v   = 1.0f;
        v[3].u   = 1.0f;
        v[3].v   = 1.0f;

        // 使用するテクスチャを設定 (slot=0)
        SetUseTextureToShader(0, *texture_);

        // 使用するピクセルシェーダーを設定 (2Dの場合は頂点シェーダー不要)
        SetUsePixelShader(*shader_ps_);

        // 描画
        DrawPrimitive2DToShader(v, 4, DX_PRIMTYPE_TRIANGLESTRIP);
    }
    // 使い終わったらテクスチャ設定を解除
    // 解除しておかないとモデル描画に影響あり。
    SetUseTextureToShader(0, -1);

    //----------------------------------------------------------
    // ビルボードを描画
    //----------------------------------------------------------

    for(int i = 0; i < 2; i++) {
        // モデルコンポーネント
        //    DrawSphere3D(VECTOR{0, 0, 0}, 4.0f, 64, RED, YELLOW, 1);
    }

    particleSystem.render();
}

//---------------------------------------------------------------------------
//! 終了
//---------------------------------------------------------------------------
void SceneShader::Exit()
{
}

//---------------------------------------------------------------------------
//! GUI表示
//---------------------------------------------------------------------------
void SceneShader::GUI()
{
}
