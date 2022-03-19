#version 300 es
precision mediump float;

out vec4 outColor;
in vec2 vTextCoord;
uniform sampler2D yTexture;
uniform sampler2D uTexture;
uniform sampler2D vTexture;

//R = Y + 1.4075 *（V-128）
//G = Y – 0.3455 *（U –128） – 0.7169 *（V –128）
//B = Y + 1.779 *（U – 128）
//vec4 yuvToRGBAColor(sampler2D ySampler, sampler2D uSampler, sampler2D vSampler, vec2 textCoord) {
//    vec3 yuv;
//    vec3 rgb;
//    //分别取yuv各个分量的采样纹理
//    yuv.r = texture(ySampler, textCoord).r;
//    yuv.g = texture(uSampler, textCoord).r - 0.5;
//    yuv.b = texture(vSampler, textCoord).r - 0.5;
//    rgb = mat3(//OpenGLES矩阵列优先存储
//    1.0, 1.0, 1.0,  //第一列
//    0.0, -0.3455, 1.779,  //第二列
//    1.4075, -0.7169, 0.0    //第三列
//    ) * yuv;
//    vec4 nColor = vec4(rgb, 1);
//    return nColor;
//}

//R = 1.164 *(Y - 16) + 1.596 *(Cr - 128)
//G = 1.164 *(Y - 16) - 0.392 *(Cb - 128) - 0.812 *(Cr - 128)
//B = 1.164 *(Y - 16) + 2.016 *(Cb - 128)
vec4 yuvToRGBAColor(sampler2D ySampler, sampler2D uSampler, sampler2D vSampler, vec2 textCoord) {
    vec3 yuv;
    vec3 rgb;
    //分别取yuv各个分量的采样纹理
    yuv.r = texture(ySampler, textCoord).r - 0.0625;
    yuv.g = texture(uSampler, textCoord).r - 0.5;
    yuv.b = texture(vSampler, textCoord).r - 0.5;
    rgb = mat3(//OpenGLES矩阵列优先存储
    1.164, 1.164, 1.164,  //第一列
    0.0, -0.392, 2.016,  //第二列
    1.596, -0.812, 0.0    //第三列
    ) * yuv;
    vec4 nColor = vec4(rgb, 1);
    return nColor;
}

//  R = 1.164 *(Y - 16) + 1.792 *(Cr - 128)
//  G = 1.164 *(Y - 16) - 0.213 *(Cb - 128) - 0.534 *(Cr - 128)
//  B = 1.164 *(Y - 16) + 2.114 *(Cb - 128)
//vec4 yuvToRGBAColor(sampler2D ySampler, sampler2D uSampler, sampler2D vSampler, vec2 textCoord) {
//    vec3 yuv;
//    vec3 rgb;
//    //分别取yuv各个分量的采样纹理
//    yuv.r = texture(ySampler, textCoord).r - 0.0625;
//    yuv.g = texture(uSampler, textCoord).r - 0.5;
//    yuv.b = texture(vSampler, textCoord).r - 0.5;
//    rgb = mat3(//OpenGLES矩阵列优先存储
//    1.164, 1.164, 1.164,  //第一列
//    0.0, -0.213, 2.114,  //第二列
//    1.192, -0.534, 0.0    //第三列
//    ) * yuv;
//    vec4 nColor = vec4(rgb, 1);
//    return nColor;
//}

void main() {
    vec4 nColor = yuvToRGBAColor(yTexture, uTexture, vTexture, vTextCoord);
    outColor = nColor;
}
