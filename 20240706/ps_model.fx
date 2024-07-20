//----------------------------------------------------------------------------
//!	@file	ps_model.fx
//!	@brief	MV1モデルピクセルシェーダー
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

//--------------------------------------------------------------
// 定数バッファ
//--------------------------------------------------------------
cbuffer CameraInfo : register(b10)
{
    matrix mat_view_; //!< ビュー行列
    matrix mat_proj_; //!< 投影行列
    float3 eye_position_; //!< カメラの位置
};

TextureCube iblDiffuseTexture : register(t14); //
TextureCube iblSpecularTexture : register(t15); //

SamplerState iblDiffuseSampler : register(s14); //sampler use s
SamplerState iblSpecularSampler : register(s15); //sampler use s

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT_MODEL input)
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

	
    float roughness = 1.0; // ラフさ            (つるつる) 0.0 ～ 1.0 (ざらざら)
    float metalness = 0.0; // 金属度合い metallic (非金属) 0.0 ～ 1.0 (金属)
	
	
	
	
	//----------------------------------------------------------
	// 拡散反射光 Diffuse
	//----------------------------------------------------------
    float3 L = normalize(float3(1, 1, -1)); // 光源の方向 L

	// Lambertモデル
	//    diffuse = max( dot(N, L), 0 )
	//
	// saturate = 数値を0.0～1.0の間に収める。 clamp(x, 0, 1) と同義
	// GPUは saturate と abs は 0サイクル コストなしで実行できる
	
	// 光量を 1/π 倍することでエネルギー保存則を満たす計算になる
    float PI = 3.141592;
    float Kd = 1.0 / PI;
    float diffuse = saturate(dot(N, L)) * Kd;
	
	//----------------------------------------------------------
	// 鏡面反射光 Specular
	//----------------------------------------------------------
    float3 H = normalize(L + V);
	
    float NdotH = saturate(dot(N, H));
	
	
#if 0	
	// Blinn-Phongモデル
	//   specular = pow( saturate( dot(N, H) ), specularPower )
	//   specularPowerは数値を上げると鋭いシルエットになる。
    float	specularPower = 250;
    float	Ks = (specularPower + 9.0) / (PI * 9.0); // 分子と分母を交換して逆数にしている
    float	specular = pow(saturate(dot(N, H)), specularPower) * Ks;
#else
	// Trowbridge-Reitz (GGX)モデル	
    float alpha = roughness * roughness; // α = ラフネスの2乗
    float numerator = alpha * alpha; // 分子
    float denominator = NdotH * NdotH * (alpha * alpha - 1.0) + 1.0; // 分母
    denominator *= denominator * PI;
	
    float specular = numerator / denominator;
#endif
	
	// フレネル反射
	// Schlick の近似式
    float f0 = 0.04; // 4%
    float fresnel = f0 + (1.0 - f0) * pow(1.0 - dot(N, V), 5.0);
	
    float3 color;
    color = (diffuse < 0.1) ? float3(0.7, 0.7, 0.7) : float3(1, 1, 1);
    color *= textureColor.rgb;
    color += specular * fresnel;

	//    color = V;

	
    float3 iblDiffuse = iblDiffuseTexture.Sample(iblDiffuseSampler,N).rgb * (1.0/PI); //use rgb instead of rgba //can be used without pi, will result to ibnSpecular
    //color = iblSpecularTexture.Sample(iblSpecularSampler, N).rgb;  //METAL
	
	//反射ベクトルRを計算する
	//R = reflect(入射ベクトル、法線ベクトル)
    float3 R = reflect(-V, N); //-V is a vector from camera to the object, but reversed
    float3 specularColor = float3(0.04, 0.04, 0.04);
    float3 environmentBRDF = pow(1.0 - max(roughness, dot(N, V)), 3.0) + specularColor;   

    float mip = 8.0 * roughness;
    float3 iblSpecular = iblSpecularTexture.SampleLevel(iblSpecularSampler, R, mip).rgb; //GOLD but incorrect //The third argument is the smoothness, bigger smoother
    iblSpecular *= environmentBRDF;
    
    textureColor.rgb = pow(textureColor.rgb, 2.2); //ADDITION fog look
    
    color = iblDiffuse * textureColor.rgb + iblSpecular;
    
    color = color / (color + 1);
    color.rgb = pow(color.rgb, 1.0 /2.2);
    
	 
    if ((int(input.position_.y) & 7) >> 2 == 0)
    {
//         color *= 0.5;
    }
	
    output.color0_ = float4(color, 1);

	// 出力パラメータを返す
    return output;
}
