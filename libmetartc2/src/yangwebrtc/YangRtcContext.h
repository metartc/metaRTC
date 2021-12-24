#ifndef SRC_YANGWEBRTC_YANGRTCCONTEXT_H_
#define SRC_YANGWEBRTC_YANGRTCCONTEXT_H_
#include <yangstream/YangStreamType.h>
#include <yangutil/yangavinfotype.h>

#include <map>
#include <vector>
class YangSdp;
class YangRtcSourceDescription;

class YangRtcContext{
public:
	YangRtcContext();
	~YangRtcContext();
	std::string codec_;

	YangAudioParam* remote_audio;
	YangVideoParam* remote_video;
	YangSdp* local_sdp;
	YangSdp* remote_sdp;

	uint32_t audioSsrc;
	uint32_t videoSsrc;
	int32_t state;

	YangRtcSourceDescription* source;
	YangStreamConfig* streamConf;
	YangContext* context;
};
class YangRtcTrack{
public:
	YangRtcTrack();
	~YangRtcTrack();
	std::string type_;
	    // track_id
	std::string id_;
	uint32_t ssrc_;
	uint32_t rtx_ssrc_;
	uint32_t fec_ssrc_;
    std::string mid_;
    // msid_: track stream id
    std::string msid_;
    std::map<int, std::string> extmaps_;
public:
    YangRtcTrack* copy();
    void add_rtp_extension_desc(int32_t id, std::string uri);
     void del_rtp_extension_desc(std::string uri);
    int32_t get_rtp_extension_id(std::string uri);
    bool has_ssrc(uint32_t ssrc);
    void set_rtx_ssrc(uint32_t ssrc);
    void set_fec_ssrc(uint32_t ssrc);
    void set_mid(std::string mid);
};
class YangRtcSourceDescription
{
public:
    // the id for this stream;
    std::string id_;

    YangRtcTrack* audio_track_desc_;
    std::vector<YangRtcTrack*> video_track_descs_;
public:
    YangRtcSourceDescription();
    virtual ~YangRtcSourceDescription();

public:
    YangRtcSourceDescription* copy();
    YangRtcTrack* find_track_description_by_ssrc(uint32_t ssrc);
};

/**
class YangRtcTrackDescription
{
public:
    // type: audio, video
    std::string type_;
    // track_id
    std::string id_;
    // ssrc is the primary ssrc for this track,
    // if sdp has ssrc-group, it is the first ssrc of the ssrc-group
    uint32_t ssrc_;
    // rtx ssrc is the second ssrc of "FEC" src-group,
    // if no rtx ssrc, rtx_ssrc_ = 0.
    uint32_t fec_ssrc_;
    // rtx ssrc is the second ssrc of "FID" src-group,
    // if no rtx ssrc, rtx_ssrc_ = 0.
    uint32_t rtx_ssrc_;
    // key: rtp header extension id, value: rtp header extension uri.
    std::map<int, std::string> extmaps_;
    // Whether this track active. default: active.
    bool is_active_;
    // direction
    std::string direction_;
    // mid is used in BOUNDLE
    std::string mid_;
    // msid_: track stream id
    std::string msid_;

    // meida payload, such as opus, h264.
    YangCodecPayload* media_;
    YangCodecPayload* red_;
    YangCodecPayload* rtx_;
    YangCodecPayload* ulpfec_;
public:
    YangRtcTrackDescription();
    virtual ~YangRtcTrackDescription();
public:
    // whether or not the track has ssrc.
    // for example:
    //    we need check track has the ssrc in the ssrc_group, then add ssrc_group to the track,
    bool has_ssrc(uint32_t ssrc);
public:
    void add_rtp_extension_desc(int32_t id, std::string uri);
    void del_rtp_extension_desc(std::string uri);
    void set_direction(std::string direction);
    void set_codec_payload(YangCodecPayload* payload);
    // auxiliary paylod include red, rtx, ulpfec.
    void create_auxiliary_payload(const std::vector<YangMediaPayloadType> payload_types);
    void set_rtx_ssrc(uint32_t ssrc);
    void set_fec_ssrc(uint32_t ssrc);
    void set_mid(std::string mid);
    int32_t get_rtp_extension_id(std::string uri);
public:
    YangRtcTrackDescription* copy();
};


**/
#endif /* SRC_YANGWEBRTC_YANGRTCCONTEXT_H_ */
