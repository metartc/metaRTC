//
// Copyright (c) 2019-2025 yanggaofeng
//

#include <yangavutil/video/YangSpspps.h>

#include <math.h>
#include <yangutil/sys/YangLog.h>
typedef unsigned long yangdword;

typedef struct {
    int32_t forbidden_zero_bit;
    int32_t nal_ref_idc;
    int32_t nal_unit_type;
    int32_t profile_idc;
    int32_t constraint_set0_flag;
    int32_t constraint_set1_flag;
    int32_t constraint_set2_flag;
    int32_t constraint_set3_flag;
    int32_t reserved_zero_4bits;
    int32_t level_idc;
    int32_t seq_parameter_set_id;
    int32_t chroma_format_idc;
    int32_t residual_colour_transform_flag;
    int32_t bit_depth_luma_minus8;
    int32_t bit_depth_chroma_minus8;
    int32_t qpprime_y_zero_transform_bypass_flag;
    int32_t seq_scaling_matrix_present_flag;
    int32_t seq_scaling_list_present_flag[8];
    int32_t log2_max_frame_num_minus4;
    int32_t pic_order_cnt_type;
    int32_t log2_max_pic_order_cnt_lsb_minus4;
    int32_t delta_pic_order_always_zero_flag;
    int32_t offset_for_non_ref_pic;
    int32_t offset_for_top_to_bottom_field;
    int32_t num_ref_frames_in_pic_order_cnt_cycle;
    int32_t num_ref_frames;
    int32_t gaps_in_frame_num_value_allowed_flag;
    int32_t pic_width_in_mbs_minus1;
    int32_t pic_height_in_map_units_minus1;
    int32_t frame_mbs_only_flag;
    int32_t mb_adaptive_frame_field_flag;
    int32_t direct_8x8_inference_flag;
    int32_t frame_cropping_flag;
    int32_t frame_crop_left_offset;
    int32_t frame_crop_right_offset;
    int32_t frame_crop_top_offset;
    int32_t frame_crop_bottom_offset;
    int32_t vui_parameter_present_flag;
    int32_t aspect_ratio_info_present_flag;
    int32_t aspect_ratio_idc;
    int32_t sar_width;
    int32_t sar_height;
    int32_t overscan_info_present_flag;
    int32_t overscan_appropriate_flagu;
    int32_t video_signal_type_present_flag;
    int32_t video_format;
    int32_t video_full_range_flag;
    int32_t colour_description_present_flag;
    int32_t colour_primaries;
    int32_t transfer_characteristics;
    int32_t matrix_coefficients;
    int32_t chroma_loc_info_present_flag;
    int32_t chroma_sample_loc_type_top_field;
    int32_t chroma_sample_loc_type_bottom_field;
    int32_t timing_info_present_flag;
    int32_t num_units_in_tick;
    int32_t time_scale;
}YangSps;

uint32_t yang_sps_ue(uint8_t *pBuff, uint32_t nLen, uint32_t *nStartBit)
{

    uint32_t nZeroNum = 0,i;
    yangdword dwRet = 0;

    while (*nStartBit < nLen * 8)
    {
        if (pBuff[*nStartBit / 8] & (0x80 >> (*nStartBit % 8)))
        {
            break;
        }
        nZeroNum++;
        (*nStartBit)++;
    }

    (*nStartBit) ++;

    for (i=0; i<nZeroNum; i++)
    {
        dwRet <<= 1;
        if (pBuff[*nStartBit / 8] & (0x80 >> (*nStartBit % 8)))
        {
            dwRet += 1;
        }
        (*nStartBit)++;
    }

    return (1 << nZeroNum) - 1 + dwRet;
}


int32_t yang_sps_se(uint8_t *pBuff, uint32_t nLen, uint32_t *nStartBit)
{
	int32_t UeVal=yang_sps_ue(pBuff,nLen,nStartBit);
    double k=UeVal;
    int32_t nValue=ceil(k/2);

    if (UeVal % 2==0)
        nValue=-nValue;

    return nValue;
}


yangdword yang_sps_u(uint32_t BitCount,uint8_t * buf,uint32_t *nStartBit)
{
	uint32_t i;
    yangdword dwRet = 0;

    for (i=0; i<BitCount; i++)
    {
        dwRet <<= 1;
        if (buf[*nStartBit / 8] & (0x80 >> (*nStartBit % 8)))
        {
            dwRet += 1;
        }
        (*nStartBit)++;
    }

    return dwRet;
}


void yang_de_emulation_prevention(uint8_t* buf,unsigned int* buf_size)
{
    uint32_t i=0,j=0;
    uint8_t* tmp_ptr=NULL;
    uint32_t tmp_buf_size=0;
    int32_t val=0;

    tmp_ptr=buf;
    tmp_buf_size=*buf_size;

    for(i=0;i<(tmp_buf_size-2);i++)
    {
        //check for 0x000003
        val=(tmp_ptr[i]^0x00) +(tmp_ptr[i+1]^0x00)+(tmp_ptr[i+2]^0x03);
        if(val==0)
        {
            //kick out 0x03
            for(j=i+2;j<tmp_buf_size-1;j++)
                tmp_ptr[j]=tmp_ptr[j+1];

            //and so we should devrease bufsize
            (*buf_size)--;
        }
    }

    return;
}

void yang_h264_decode_getH264RtmpHeader(uint8_t* ps,uint8_t** sps,int32_t* spsLen){
    *spsLen = *(ps + 12) + 1;
    *sps = ps + 13;
}



int yang_h264_decode_spspps(uint8_t * buf,uint32_t nLen,int32_t *p_width,int32_t *p_height,int32_t *p_fps)
{

    uint32_t StartBit=0;
    uint32_t *startBitp=&StartBit;
    YangSps tsps;
    YangSps* sps=&tsps;

    *p_fps=0;
    yang_de_emulation_prevention(buf,&nLen);

    yang_memset(&tsps,0,sizeof(YangSps));

    sps->forbidden_zero_bit=yang_sps_u(1,buf,startBitp);
    sps->nal_ref_idc=yang_sps_u(2,buf,startBitp);
    sps->nal_unit_type=yang_sps_u(5,buf,startBitp);
    if(sps->nal_unit_type==7)
    {
        sps->profile_idc=yang_sps_u(8,buf,startBitp);
        sps->constraint_set0_flag=yang_sps_u(1,buf,startBitp);//(buf[1] & 0x80)>>7;
        sps->constraint_set1_flag=yang_sps_u(1,buf,startBitp);//(buf[1] & 0x40)>>6;
        sps->constraint_set2_flag=yang_sps_u(1,buf,startBitp);//(buf[1] & 0x20)>>5;
        sps->constraint_set3_flag=yang_sps_u(1,buf,startBitp);//(buf[1] & 0x10)>>4;
        sps->reserved_zero_4bits=yang_sps_u(4,buf,startBitp);
        sps->level_idc=yang_sps_u(8,buf,startBitp);

        sps->seq_parameter_set_id=yang_sps_ue(buf,nLen,startBitp);

        if( sps->profile_idc == 100 || sps->profile_idc == 110 ||
                sps->profile_idc == 122 || sps->profile_idc == 144 )
        {
            sps->chroma_format_idc=yang_sps_ue(buf,nLen,startBitp);

            if( sps->chroma_format_idc == 3 )	sps->residual_colour_transform_flag=yang_sps_u(1,buf,startBitp);
            sps->bit_depth_luma_minus8=yang_sps_ue(buf,nLen,startBitp);
            sps->bit_depth_chroma_minus8=yang_sps_ue(buf,nLen,startBitp);
            sps->qpprime_y_zero_transform_bypass_flag=yang_sps_u(1,buf,startBitp);
            sps->seq_scaling_matrix_present_flag=yang_sps_u(1,buf,startBitp);
            if( sps->seq_scaling_matrix_present_flag )
            {
                for( int i = 0; i < 8; i++ ) {
                    sps->seq_scaling_list_present_flag[i]=yang_sps_u(1,buf,startBitp);
                }
            }
        }
        sps->log2_max_frame_num_minus4=yang_sps_ue(buf,nLen,startBitp);
        sps->pic_order_cnt_type=yang_sps_ue(buf,nLen,startBitp);

        if( sps->pic_order_cnt_type == 0 )
            sps->log2_max_pic_order_cnt_lsb_minus4=yang_sps_ue(buf,nLen,startBitp);
        else if( sps->pic_order_cnt_type == 1 )
        {
            sps->delta_pic_order_always_zero_flag=yang_sps_u(1,buf,startBitp);
            sps->offset_for_non_ref_pic=yang_sps_se(buf,nLen,startBitp);
            sps->offset_for_top_to_bottom_field=yang_sps_se(buf,nLen,startBitp);
            sps->num_ref_frames_in_pic_order_cnt_cycle=yang_sps_ue(buf,nLen,startBitp);

            int* offset_for_ref_frame=(int*)yang_malloc(sps->num_ref_frames_in_pic_order_cnt_cycle*sizeof(int));
            for( int i = 0; i < sps->num_ref_frames_in_pic_order_cnt_cycle; i++ )
                offset_for_ref_frame[i]=yang_sps_se(buf,nLen,startBitp);
            free(offset_for_ref_frame);
        }
        sps->num_ref_frames=yang_sps_ue(buf,nLen,startBitp);
        sps->gaps_in_frame_num_value_allowed_flag=yang_sps_u(1,buf,startBitp);
        sps->pic_width_in_mbs_minus1=yang_sps_ue(buf,nLen,startBitp);
        sps->pic_height_in_map_units_minus1=yang_sps_ue(buf,nLen,startBitp);

        *p_width=(sps->pic_width_in_mbs_minus1+1)*16;
        *p_height=(sps->pic_height_in_map_units_minus1+1)*16;

        sps->frame_mbs_only_flag=yang_sps_u(1,buf,startBitp);

        if(!sps->frame_mbs_only_flag)
            sps->mb_adaptive_frame_field_flag=yang_sps_u(1,buf,startBitp);

        sps->direct_8x8_inference_flag=yang_sps_u(1,buf,startBitp);
        sps->frame_cropping_flag=yang_sps_u(1,buf,startBitp);
        if(sps->frame_cropping_flag)
        {
            sps->frame_crop_left_offset=yang_sps_ue(buf,nLen,startBitp);
            sps->frame_crop_right_offset=yang_sps_ue(buf,nLen,startBitp);
            sps->frame_crop_top_offset=yang_sps_ue(buf,nLen,startBitp);
            sps->frame_crop_bottom_offset=yang_sps_ue(buf,nLen,startBitp);
        }
        sps->vui_parameter_present_flag=yang_sps_u(1,buf,startBitp);
        if(sps->vui_parameter_present_flag)
        {
            sps->aspect_ratio_info_present_flag=yang_sps_u(1,buf,startBitp);
            if(sps->aspect_ratio_info_present_flag)
            {
                sps->aspect_ratio_idc=yang_sps_u(8,buf,startBitp);
                if(sps->aspect_ratio_idc==255)
                {
                    sps->sar_width=yang_sps_u(16,buf,startBitp);
                    sps->sar_height=yang_sps_u(16,buf,startBitp);
                }
            }
            sps->overscan_info_present_flag=yang_sps_u(1,buf,startBitp);

            if(sps->overscan_info_present_flag)
                sps->overscan_appropriate_flagu=yang_sps_u(1,buf,startBitp);
            sps->video_signal_type_present_flag=yang_sps_u(1,buf,startBitp);
            if(sps->video_signal_type_present_flag)
            {
                sps->video_format=yang_sps_u(3,buf,startBitp);
                sps->video_full_range_flag=yang_sps_u(1,buf,startBitp);
                sps->colour_description_present_flag=yang_sps_u(1,buf,startBitp);
                if(sps->colour_description_present_flag)
                {
                    sps->colour_primaries=yang_sps_u(8,buf,startBitp);
                    sps->transfer_characteristics=yang_sps_u(8,buf,startBitp);
                    sps->matrix_coefficients=yang_sps_u(8,buf,startBitp);
                }
            }
            sps->chroma_loc_info_present_flag=yang_sps_u(1,buf,startBitp);
            if(sps->chroma_loc_info_present_flag)
            {
                sps->chroma_sample_loc_type_top_field=yang_sps_ue(buf,nLen,startBitp);
                sps->chroma_sample_loc_type_bottom_field=yang_sps_ue(buf,nLen,startBitp);
            }
            sps->timing_info_present_flag=yang_sps_u(1,buf,startBitp);
            if(sps->timing_info_present_flag)
            {
                sps->num_units_in_tick=yang_sps_u(32,buf,startBitp);
                sps->time_scale=yang_sps_u(32,buf,startBitp);
                *p_fps=sps->time_scale/(2*sps->num_units_in_tick);
            }
        }
        return Yang_Ok;
    }
    else
        return 1;
}

int32_t yang_h265_decode_sps(uint8_t* payload,uint32_t nLen,uint32_t* width,uint32_t* height) {
	int32_t i;
    uint32_t StartBit=0;
    uint32_t *startBitp=&StartBit;
    int32_t  sps_max_sub_layers_minus1 ;
    uint32_t sps_seq_parameter_set_id;
    uint32_t chroma_format_idc;
    uint32_t bit_depth_luma_minus8;
    uint32_t bit_depth_chroma_minus8;
    int32_t val=0;

    uint8_t sub_layer_profile_present_flag[6] = {0};
    uint8_t sub_layer_level_present_flag[6]   = {0};
    uint8_t nalData[256]={0};
    (void)val;

    yang_memcpy(nalData,payload,yang_min(256,nLen));

    yang_de_emulation_prevention(nalData,&nLen);
    val =yang_sps_u(4, nalData,startBitp);// sps_video_parameter_set_id

    sps_max_sub_layers_minus1 = yang_sps_u(3, nalData,startBitp);

    if (sps_max_sub_layers_minus1 > 6)
    {
        yang_error("sps_max_sub_layers_minus1 error(%d)",sps_max_sub_layers_minus1);
        return 1;
    }

    val =yang_sps_u(1, nalData,startBitp);


    val =yang_sps_u(2, nalData,startBitp);
    val =yang_sps_u(1, nalData,startBitp);
    val = yang_sps_u(5, nalData,startBitp);//profile

    val = yang_sps_u(32, nalData,startBitp);//6

    val = yang_sps_u(1, nalData,startBitp);//
    val = yang_sps_u(1, nalData,startBitp);//
    val = yang_sps_u(1, nalData,startBitp);//
    val = yang_sps_u(1, nalData,startBitp);//

    val = yang_sps_u(44, nalData,startBitp);//

    val= yang_sps_u(8, nalData,startBitp);// level_idc


    for (i = 0; i < sps_max_sub_layers_minus1; i++) {
        sub_layer_profile_present_flag[i]= yang_sps_u(1, nalData,startBitp);
        sub_layer_level_present_flag[i]= yang_sps_u(1, nalData,startBitp);
    }

    if (sps_max_sub_layers_minus1 > 0)
    {
        for (i = sps_max_sub_layers_minus1; i < 8; i++) {
            //uint8_t reserved_zero_2bits =
            yang_sps_u(2, nalData,startBitp);

        }
    }

    for (i = 0; i < sps_max_sub_layers_minus1; i++)
    {
        if (sub_layer_profile_present_flag[i]) {
            val =yang_sps_u(2, nalData,startBitp);
            val = yang_sps_u(1, nalData,startBitp);
            val = yang_sps_u(5, nalData,startBitp);
            val =yang_sps_u(32, nalData,startBitp);
            val = yang_sps_u(1, nalData,startBitp);
            val = yang_sps_u(1, nalData,startBitp);
            val = yang_sps_u(1, nalData,startBitp);
            val = yang_sps_u(1, nalData,startBitp);
            val = yang_sps_u(44, nalData,startBitp);
        }

        if (sub_layer_level_present_flag[i]) {
            val = yang_sps_u(8, nalData,startBitp);// sub_layer_level_idc[i]
        }
    }

     sps_seq_parameter_set_id= yang_sps_ue(nalData,nLen,startBitp);

    if (sps_seq_parameter_set_id > 15) {
        yang_error("error sps_seq_parameter_set_id(%u)",sps_seq_parameter_set_id);
        return 1;
    }

    chroma_format_idc = yang_sps_ue(nalData,nLen,startBitp);

    if (sps_seq_parameter_set_id > 3) {
        yang_error("error sps_seq_parameter_set_id(%u)",sps_seq_parameter_set_id);
        return 1;
    }

    if (chroma_format_idc == 3) {
        val =  yang_sps_u(1, nalData,startBitp);//
    }

    *width  = yang_sps_ue(nalData,nLen,startBitp); // pic_width_in_luma_samples
    *height  =yang_sps_ue(nalData,nLen,startBitp); // pic_height_in_luma_samples

    if ( yang_sps_u(1, nalData,startBitp)) {
        yang_sps_ue(nalData,nLen,startBitp);
        yang_sps_ue(nalData,nLen,startBitp);
        yang_sps_ue(nalData,nLen,startBitp);
        yang_sps_ue(nalData,nLen,startBitp);
    }

    bit_depth_luma_minus8= yang_sps_ue(nalData,nLen,startBitp);
    bit_depth_chroma_minus8= yang_sps_ue(nalData,nLen,startBitp);

    if (bit_depth_luma_minus8 != bit_depth_chroma_minus8) {
        return 1;
    }

    return Yang_Ok;
}

