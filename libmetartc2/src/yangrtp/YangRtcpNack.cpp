#include <yangrtp/YangRtcpNack.h>
#include <yangutil/sys/YangLog.h>

YangRtcpNack::YangRtcpNack(uint32_t sender_ssrc)
{
    m_header.padding = 0;
    m_header.type = YangRtcpType_rtpfb;
    m_header.rc = 1;
    m_header.version = kRtcpVersion;
    m_ssrc = sender_ssrc;
    m_media_ssrc_ = 0;
}

YangRtcpNack::~YangRtcpNack()
{
}

uint32_t YangRtcpNack::get_media_ssrc() const
{
    return m_media_ssrc_;
}

vector<uint16_t> YangRtcpNack::get_lost_sns() const
{
    vector<uint16_t> sn;
    for(set<uint16_t, YangSeqCompareLess>::iterator it = m_lost_sns.begin(); it != m_lost_sns.end(); ++it) {
        sn.push_back(*it);
    }
    return sn;
}

bool YangRtcpNack::empty()
{
    return m_lost_sns.empty();
}

void YangRtcpNack::set_media_ssrc(uint32_t ssrc)
{
    m_media_ssrc_ = ssrc;
}

void YangRtcpNack::add_lost_sn(uint16_t sn)
{
    m_lost_sns.insert(sn);
}

int32_t YangRtcpNack::decode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/rfc4585#section-6.1
        0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|   FMT   |       PT      |          length               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of packet sender                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of media source                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   :            Feedback Control Information (FCI)                 :
   :                                                               :

    Generic NACK
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |            PID                |             BLP               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    int32_t err = Yang_Ok;
    m_data = buffer->head();
    m_nb_data = buffer->left();

    if(Yang_Ok != (err = decode_header(buffer))) {
        return yang_error_wrap(err, "decode header");
    }

    m_media_ssrc_ = buffer->read_4bytes();
    char bitmask[20];
    for(int32_t i = 0; i < (m_header.length - 2); i++) {
        uint16_t pid = buffer->read_2bytes();
        uint16_t blp = buffer->read_2bytes();
        m_lost_sns.insert(pid);
        memset(bitmask, 0, 20);
        for(int32_t j=0; j<16; j++) {
            bitmask[j] = (blp & ( 1 << j )) >> j ? '1' : '0';
            if((blp & ( 1 << j )) >> j)
                m_lost_sns.insert(pid+j+1);
        }
        bitmask[16] = '\n';
        //srs_info("[%d] %d / %s", i, pid, bitmask);
    }

    return err;
}
uint64_t YangRtcpNack::nb_bytes()
{
    return kRtcpPacketSize;
}

int32_t YangRtcpNack::encode(YangBuffer *buffer)
{
    /*
    @doc: https://tools.ietf.org/html/rfc4585#section-6.1
        0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |V=2|P|   FMT   |       PT      |          length               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of packet sender                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  SSRC of media source                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   :            Feedback Control Information (FCI)                 :
   :                                                               :

    Generic NACK
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |            PID                |             BLP               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    int32_t err = Yang_Ok;
    if(!buffer->require(nb_bytes())) {
        return yang_error_wrap(ERROR_RTC_RTCP, "requires %d bytes", nb_bytes());
    }

    vector<YangPidBlp> chunks;
    do {
        YangPidBlp chunk;
        chunk.in_use = false;
        uint16_t pid = 0;
        for(set<uint16_t, YangSeqCompareLess>::iterator it = m_lost_sns.begin(); it != m_lost_sns.end(); ++it) {
            uint16_t sn = *it;
            if(!chunk.in_use) {
                chunk.pid = sn;
                chunk.blp = 0;
                chunk.in_use = true;
                pid = sn;
                continue;
            }
            if((sn - pid) < 1) {
                yang_info("skip seq %d", sn);
            } else if( (sn - pid) > 16) {
                // add new chunk
                chunks.push_back(chunk);
                chunk.in_use = false;
            } else {
                chunk.blp |= 1 << (sn-pid-1);
            }
        }
        if(chunk.in_use) {
            chunks.push_back(chunk);
        }

        m_header.length = 2 + chunks.size();
        if(Yang_Ok != (err = encode_header(buffer))) {
            err = yang_error_wrap(err, "encode header");
            break;
        }

        buffer->write_4bytes(m_media_ssrc_);
        for(vector<YangPidBlp>::iterator it_chunk = chunks.begin(); it_chunk != chunks.end(); it_chunk++) {
            buffer->write_2bytes(it_chunk->pid);
           // yang_trace("\nsend seq====%hu",it_chunk->pid);
            buffer->write_2bytes(it_chunk->blp);
        }
    } while(0);

    return err;
}
