//
// Copyright (c) 2019-2022 yanggaofeng akanchi
//

#include <stdint.h>
#include <memory>
#include <vector>
#include <string>

class YangTsBuffer;

class MpegTsStream
{
public:
    static const uint8_t AAC = 0x0f;
    static const uint8_t AVC = 0x1b;
};

class TsFrame
{
public:
    TsFrame();
    TsFrame(uint8_t st);
    virtual ~TsFrame(){};

public:
    bool empty();
    void reset();

public:
    std::shared_ptr<YangTsBuffer> _data;
    uint64_t pts;
    uint64_t dts;
    uint64_t pcr;
    uint8_t stream_type;
    uint8_t stream_id;
    uint16_t pid;
    uint16_t expected_pes_packet_length;
    bool completed;
};

class TsHeader
{
public:
    TsHeader();
    virtual ~TsHeader();

public:
    void encode(YangTsBuffer *sb);
    void decode(YangTsBuffer *sb);

public:
    uint8_t sync_byte;                      // 8 bits
    uint8_t transport_error_indicator;      // 1 bit
    uint8_t payload_unit_start_indicator;   // 1 bit
    uint8_t transport_priority;             // 1 bit
    uint16_t pid;                           // 13 bits
    uint8_t transport_scrambling_control;   // 2 bits
    uint8_t adaptation_field_control;       // 2 bits
    uint8_t continuity_counter;             // 4 bits
};

class PATHeader
{
public:
    PATHeader();
    virtual ~PATHeader();

public:
    void encode(YangTsBuffer *sb);
    void decode(YangTsBuffer *sb);
    void print();

public:
    uint8_t table_id;                       // 8 bits
    uint8_t section_syntax_indicator;       // 1 bit
    uint8_t b0;                             // 1 bit
    uint8_t reserved0;                      // 2 bits
    uint16_t section_length;                // 12 bits
    uint16_t transport_stream_id;           // 16 bits
    uint8_t reserved1;                      // 2 bits
    uint8_t version_number;                 // 5 bits
    uint8_t current_next_indicator;         // 1 bit
    uint8_t section_number;                 // 8 bits
    uint8_t last_section_number;            // 8 bits
};

class PMTElementInfo
{
public:
    PMTElementInfo();
    PMTElementInfo(uint8_t st, uint16_t pid);
    virtual ~PMTElementInfo();

public:
    void encode(YangTsBuffer *sb);
    void decode(YangTsBuffer *sb);
    uint16_t size();
    void print();

public:
    uint8_t stream_type;                    // 8 bits
    uint8_t reserved0;                      // 3 bits
    uint16_t elementary_PID;                // 13 bits
    uint8_t reserved1;                      // 4 bits
    uint16_t ES_info_length;                // 12 bits
    std::string ES_info;
};

class PMTHeader
{
public:
    PMTHeader();
    virtual ~PMTHeader();

public:
    void encode(YangTsBuffer *sb);
    void decode(YangTsBuffer *sb);
    uint16_t size();
    void print();

public:
    uint8_t table_id;                       // 8 bits
    uint8_t section_syntax_indicator;       // 1 bit
    uint8_t b0;                             // 1 bit
    uint8_t reserved0;                      // 2 bits
    uint16_t section_length;                // 12 bits
    uint16_t program_number;                // 16 bits
    uint8_t reserved1;                      // 2 bits
    uint8_t version_number;                 // 5 bits
    uint8_t current_next_indicator;         // 1 bit
    uint8_t section_number;                 // 8 bits
    uint8_t last_section_number;            // 8 bits
    uint8_t reserved2;                      // 3 bits
    uint16_t PCR_PID;                       // 13 bits
    uint8_t reserved3;                      // 4 bits
    uint16_t program_info_length;           // 12 bits
    std::vector<std::shared_ptr<PMTElementInfo>> infos;
};

class AdaptationFieldHeader
{
public:
    AdaptationFieldHeader();
    virtual ~AdaptationFieldHeader();

public:
    void encode(YangTsBuffer *sb);
    void decode(YangTsBuffer *sb);

public:
    uint8_t adaptation_field_length;                // 8 bits
    uint8_t adaptation_field_extension_flag;        // 1 bit
    uint8_t transport_private_data_flag;            // 1 bit
    uint8_t splicing_point_flag;                    // 1 bit
    uint8_t opcr_flag;                              // 1 bit
    uint8_t pcr_flag;                               // 1 bit
    uint8_t elementary_stream_priority_indicator;   // 1 bit
    uint8_t random_access_indicator;                // 1 bit
    uint8_t discontinuity_indicator;                // 1 bit
};

class PESHeader
{
public:
    PESHeader();
    virtual ~PESHeader();

public:
    void encode(YangTsBuffer *sb);
    void decode(YangTsBuffer *sb);

public:
    uint32_t packet_start_code;             // 24 bits
    uint8_t stream_id;                      // 8 bits
    uint16_t pes_packet_length;             // 16 bits
    uint8_t original_or_copy;               // 1 bit
    uint8_t copyright;                      // 1 bit
    uint8_t data_alignment_indicator;       // 1 bit
    uint8_t pes_priority;                   // 1 bit
    uint8_t pes_scrambling_control;         // 2 bits
    uint8_t marker_bits;                    // 2 bits
    uint8_t pes_ext_flag;                   // 1 bit
    uint8_t pes_crc_flag;                   // 1 bit
    uint8_t add_copy_info_flag;             // 1 bit
    uint8_t dsm_trick_mode_flag;            // 1 bit
    uint8_t es_rate_flag;                   // 1 bit
    uint8_t escr_flag;                      // 1 bit
    uint8_t pts_dts_flags;                  // 2 bits
    uint8_t header_data_length;             // 8 bits
};
