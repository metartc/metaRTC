//
// Copyright (c) 2019-2022 yanggaofeng
//

#include <yangavutil/video/YangSpspps.h>
#include <yangutil/yangtype.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>


typedef unsigned long DWORD;

uint32_t yang_sps_Ue(uint8_t *pBuff, uint32_t nLen, uint32_t *nStartBit)
{

    uint32_t nZeroNum = 0;
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



    DWORD dwRet = 0;
    for (uint32_t i=0; i<nZeroNum; i++)
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


int yang_sps_Se(uint8_t *pBuff, uint32_t nLen, uint32_t *nStartBit)
{
    int UeVal=yang_sps_Ue(pBuff,nLen,nStartBit);
    double k=UeVal;
    int nValue=ceil(k/2);
    if (UeVal % 2==0)
        nValue=-nValue;
    return nValue;
}


DWORD yang_sps_u(uint32_t BitCount,uint8_t * buf,uint32_t *nStartBit)
{
    DWORD dwRet = 0;
    for (uint32_t i=0; i<BitCount; i++)
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
    int val=0;

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


int yang_h264_decode_spspps(uint8_t * buf,uint32_t nLen,int32_t *p_width,int32_t *p_height,int32_t *p_fps)
{

    uint32_t StartBit=0;
    uint32_t *startBitp=&StartBit;
    *p_fps=0;
    yang_de_emulation_prevention(buf,&nLen);
    YangSps tsps;
    memset(&tsps,0,sizeof(YangSps));
    YangSps* sps=&tsps;
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

            sps->seq_parameter_set_id=yang_sps_Ue(buf,nLen,startBitp);

            if( sps->profile_idc == 100 || sps->profile_idc == 110 ||
            		sps->profile_idc == 122 || sps->profile_idc == 144 )
            {
                sps->chroma_format_idc=yang_sps_Ue(buf,nLen,startBitp);

                if( sps->chroma_format_idc == 3 )	sps->residual_colour_transform_flag=yang_sps_u(1,buf,startBitp);
                sps->bit_depth_luma_minus8=yang_sps_Ue(buf,nLen,startBitp);
                sps->bit_depth_chroma_minus8=yang_sps_Ue(buf,nLen,startBitp);
                sps->qpprime_y_zero_transform_bypass_flag=yang_sps_u(1,buf,startBitp);
                sps->seq_scaling_matrix_present_flag=yang_sps_u(1,buf,startBitp);
                if( sps->seq_scaling_matrix_present_flag )
                {
                    for( int i = 0; i < 8; i++ ) {
                    	sps->seq_scaling_list_present_flag[i]=yang_sps_u(1,buf,startBitp);
                    }
                }
            }
            sps->log2_max_frame_num_minus4=yang_sps_Ue(buf,nLen,startBitp);
            sps->pic_order_cnt_type=yang_sps_Ue(buf,nLen,startBitp);

            if( sps->pic_order_cnt_type == 0 )
            	sps->log2_max_pic_order_cnt_lsb_minus4=yang_sps_Ue(buf,nLen,startBitp);
            else if( sps->pic_order_cnt_type == 1 )
            {
                sps->delta_pic_order_always_zero_flag=yang_sps_u(1,buf,startBitp);
                sps->offset_for_non_ref_pic=yang_sps_Se(buf,nLen,startBitp);
                sps->offset_for_top_to_bottom_field=yang_sps_Se(buf,nLen,startBitp);
                sps->num_ref_frames_in_pic_order_cnt_cycle=yang_sps_Ue(buf,nLen,startBitp);

                int* offset_for_ref_frame=(int*)malloc(sps->num_ref_frames_in_pic_order_cnt_cycle*sizeof(int));
                for( int i = 0; i < sps->num_ref_frames_in_pic_order_cnt_cycle; i++ )
                    offset_for_ref_frame[i]=yang_sps_Se(buf,nLen,startBitp);
                free(offset_for_ref_frame);
            }
            sps->num_ref_frames=yang_sps_Ue(buf,nLen,startBitp);
            sps->gaps_in_frame_num_value_allowed_flag=yang_sps_u(1,buf,startBitp);
            sps->pic_width_in_mbs_minus1=yang_sps_Ue(buf,nLen,startBitp);
            sps->pic_height_in_map_units_minus1=yang_sps_Ue(buf,nLen,startBitp);

            *p_width=(sps->pic_width_in_mbs_minus1+1)*16;
            *p_height=(sps->pic_height_in_map_units_minus1+1)*16;

            sps->frame_mbs_only_flag=yang_sps_u(1,buf,startBitp);

            if(!sps->frame_mbs_only_flag)
            	sps->mb_adaptive_frame_field_flag=yang_sps_u(1,buf,startBitp);

            sps->direct_8x8_inference_flag=yang_sps_u(1,buf,startBitp);
            sps->frame_cropping_flag=yang_sps_u(1,buf,startBitp);
            if(sps->frame_cropping_flag)
            {
                sps->frame_crop_left_offset=yang_sps_Ue(buf,nLen,startBitp);
                sps->frame_crop_right_offset=yang_sps_Ue(buf,nLen,startBitp);
                sps->frame_crop_top_offset=yang_sps_Ue(buf,nLen,startBitp);
                sps->frame_crop_bottom_offset=yang_sps_Ue(buf,nLen,startBitp);
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
                    sps->chroma_sample_loc_type_top_field=yang_sps_Ue(buf,nLen,startBitp);
                    sps->chroma_sample_loc_type_bottom_field=yang_sps_Ue(buf,nLen,startBitp);
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
