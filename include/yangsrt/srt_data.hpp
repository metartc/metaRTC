
#ifndef SRT_DATA_H
#define SRT_DATA_H
#include <string>
#include <memory>

#define SRT_MSG_DATA_TYPE  0x01
#define SRT_MSG_CLOSE_TYPE 0x02

class SRT_DATA_MSG {
public:
	SRT_DATA_MSG();
    SRT_DATA_MSG(uint32_t  len);
	SRT_DATA_MSG(uint8_t* data_p, uint32_t  len);
    ~SRT_DATA_MSG();

    uint32_t  msg_type();
    uint32_t  data_len();
    uint8_t* get_data();
   // std::string get_path();

private:
    uint32_t    _msg_type;
    uint32_t    _len;
    uint8_t* _data_p;
   // std::string _key_path;
};

typedef std::shared_ptr<SRT_DATA_MSG> SRT_DATA_MSG_PTR;

#endif
