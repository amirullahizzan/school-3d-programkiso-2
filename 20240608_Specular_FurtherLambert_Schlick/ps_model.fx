//----------------------------------------------------------------------------
//!	@file	ps_model.fx
//!	@brief	MV1モデルピクセルシェーダー
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

//--------------------------------------------------------------
// 定数バッファ
//--------------------------------------------------------------
cbuffer CameraInfo : register(b10) //Assign to gpu register
{
    matrix mat_view_; //!< ビュー行列
    matrix mat_proj_; //!< 投影行列
    float3 eye_position_; //!< カメラの位置
};

//GPU changes of this file will reflect immediately

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT_3D input)
{
    PS_OUTPUT output;

    float2 uv = input.uv0_;
    float3 N = normalize(input.normal_); // 法線

    float3 V = normalize(eye_position_ - input.worldPosition_);
	
	//------------------------------------------------------------
	// 法線マップ
	//------------------------------------------------------------
    N = Normalmap(N, input.worldPosition_, uv);

	//------------------------------------------------------------
	// テクスチャカラーを読み込み
	//------------------------------------------------------------
    float4 textureColor = DiffuseTexture.Sample(DiffuseSampler, uv);

    // アルファテスト
    if (textureColor.a < 0.5)
        discard;

    output.color0_ = textureColor * input.diffuse_;

	//----------------------------------------------------------
	// 拡散反射光 Diffuse
	//----------------------------------------------------------
    float3 L = normalize(float3(1, 1, -1)); // 光源の方向 L

	// Lambertモデル
	//    diffuse = max( dot(N, L), 0 )
	//
	// saturate = 数値を0.0～1.0の間に収める。 clamp(x, 0, 1) と同義
	// GPUは saturate と abs は 0サイクル コストなしで実行できる
	
	///////////////////////////////////
	//Addition ++
    float PI = 3.14592;
    float Kd = 1.0f / PI; //Reflectance Constant Diffuse
	///////////////////////////////////
	
    float diffuse = saturate(dot(N, L)) * Kd; //Kd added
	
	
	//----------------------------------------------------------
	// 鏡面反射光 Specular
	//----------------------------------------------------------
    float3 H = normalize(L + V);
	
	// Blinn-Phongモデル
	//   specular = pow( saturate( dot(N, H) ), specularPower )
	//   specularPowerは数値を上げると鋭いシルエットになる。
    float specularPower = 50.0;
	/////////////////////
	//Addition ++
    float Ks = (specularPower + 9.0) / (PI / 9.0);
	/////////////////////
    float specular = pow(saturate(dot(N, H)), specularPower) * Ks; //Ks added
	
	//Fresnel Reflection
	//Schlick 式
    float f0 = 0.04; //4%
	//Rinkaku light / Rim light
    float fresnel = (1.0f - f0) * pow(1.0 - dot(N, V), 5.0);
	
    //specular *= 1000; //SILKY SMOOTH
    specular *= 200; //Brighter
    //float fresnel = (1.0f - f0) * pow(dot(N, V), 5.0); Steel like
	
	//RENDER
	
    float3 color;
    color = (diffuse < 0.5) ? float3(0.7, 0.7, 0.7) : float3(1, 1, 1);
    color *= textureColor.rgb;
     //color += specular;
    color += specular * fresnel;
	 
    if ((int(input.position_.y) & 7) >> 2 == 0)
    {
//         color *= 0.5;
    }
	
    output.color0_ = float4(color, 1);

	// 出力パラメータを返す
    return output;
}
