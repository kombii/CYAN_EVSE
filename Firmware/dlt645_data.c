#include "dlt645_private.h"
#include <string.h>
#include <math.h>

//字节位置枚举类型
typedef enum
{
    BYTE_RESET = 0,   //重置
    BYTE_LOW_ADDRESS, //低位
    BYTE_HIGH_ADDRESS //高位
} byte_part;

/**
 * Name:    _crc
 * Brief:   crc和校验
 * Input:
 *  @msg:   校验数据包
 *  @len:   数据包长度
 * Output:  校验值
 */
int _crc(uint8_t *msg, int len)
{
    uint8_t crc = 0;
    while (len--)
    {
        crc += *msg++;
    }
    return crc;
}

/**
 * Name:    dlt645_common_check
 * Brief:   645协议接收数据公共部分校验
 * Input:
 *  @msg:   校验数据包
 *  @len:   数据包长度
 *  @addr:  从站地址
 * Output:  校验成功：0，校验失败：-1
 */
int dlt645_common_check(uint8_t *msg, int len, uint8_t *addr)
{
	uint8_t crc;
    //数据包长度校验
    if (len < 7)
    {
        return -1;
    }
    //数据帧标志校验
    if (msg[0] != DL645_START_CODE ||
        msg[DL645_ADDR_LEN + 1] != DL645_START_CODE ||
        msg[len - 1] != DL645_STOP_CODE)
    {
		#ifdef DLT645_DEBUG
        DLT645_LOG("check code error!\n");
		#endif
        return -1;
    }
    //CRC校验
    crc = _crc(msg, len - 2);
    if (crc != msg[len - 2])
    {
		#ifdef DLT645_DEBUG
        DLT645_LOG("check crc error!\n");
		#endif
        return -1;
    }
    //控制码主从校验
    if ((msg[DL645_CONTROL_POS] & C_TD_MASK) == (C_TD_MASTER << C_TD_POS))
    {		
		#ifdef DLT645_DEBUG
        DLT645_LOG("check control direction error!\n");
		#endif
        return -1;
    }
    //控制码应答校验
    if ((msg[DL645_CONTROL_POS] & C_ACK_MASK) == (C_ACK_ERR << C_ACK_POS))
    {
		
		#ifdef DLT645_DEBUG
        DLT645_LOG("check ACK error!\n");
		#endif
       
        return msg[len - 3];
    }
    //从站地址校验
    if (memcmp(msg + 1, addr, 6) != 0)
    {
        return -1;
    }

    return 0;
}

/**
 * Name:    data_package_translate_to_int
 * Brief:   将接收到的dlt645数据包中的数据转化为整数
 * Input:
 *  @read_data: 数据首地址
 *  @len:       数据长度
 * Output:  转化后的整数
 */
int data_package_translate_to_int(uint8_t *read_data, uint16_t len)
{
    //权值
    uint8_t number_weight = 0;
    //当前数组下标索引
    uint8_t current_index = 0;
    //当前解析字节位
    uint8_t current_byte_part = BYTE_RESET;
    //当前整数值
    int i_value = 0;

    while (len--)
    {
        current_byte_part = BYTE_LOW_ADDRESS;
        do
        {
            switch (current_byte_part)
            {
            //当前处理字节低位
            case BYTE_LOW_ADDRESS:
                i_value += ((read_data[current_index] - 0x33) & 0x0f) * pow(10, number_weight);
                number_weight++;
                current_byte_part = BYTE_HIGH_ADDRESS;
                break;
            //当前处理字节高位
            case BYTE_HIGH_ADDRESS:
                i_value += ((read_data[current_index] - 0x33) >> 4) * pow(10, number_weight);
                number_weight++;
                current_byte_part = BYTE_RESET;
                break;
            }
        } while (current_byte_part != BYTE_RESET);
        current_index++;
    }
    return i_value;
}

/**
 * Name:    dlt645_data_parse_by_format_to_float
 * Brief:   根据数据格式将645协议读取的数据转换为真实数据并存储
 *          ！真实数据为浮点数据，需要注意的是无论读取数据长度是多少，存储数据长度都应是4字节
 * Input:
 *  @read_data:     645协议读取的数据
 *  @read_len:      读取数据的长度
 *  @data_format:   转换的数据格式，如 XX.XX,XX.XXX
 * Output:  转换成功返回0，失败返回-1
 */
int dlt645_data_parse_by_format_to_float(uint8_t *read_data, uint16_t read_len, const char *data_format, uint8_t *store_address)
{
	int i;
	float fval;
    //权值
    int num_weight = 0;
    int ival = data_package_translate_to_int(read_data, read_len);

    for (i = 0; i < strlen(data_format); i++)
    {
        if (*(data_format + i) == '.')
        {
            num_weight = strlen(data_format) - i - 1;
            if (num_weight < 0)
            {
                return -1;
            }
            break;
        }
    }
    fval = ival / pow(10, num_weight);
    memcpy(store_address, &fval, 4);
    return 0;
}
