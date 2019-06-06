/*
 * message_parser.h
 *
 *  Created on: 23 mar 2019
 *      Author: rafal
 */

#ifndef AVR_MESSAGE_PARSER_MESSAGE_PARSER_H_
#define AVR_MESSAGE_PARSER_MESSAGE_PARSER_H_

#include <stdint.h>
#include <stdlib.h>
#include "../atm_cbuffer/cbuffer.h"

#define HEAD_START_MARK	'>'
#define HEAD_END_MARK	'<'

struct Header{
	char 		head_start;	//'>'
	uint16_t	id;
	uint32_t 	msg_len;
	uint16_t	crc;
	char		head_end;	//'<'
};

namespace crc_result{
	enum result{
		ack,
		nack,
		dtx,
	};
}

namespace msg_id{
	enum id{
		txt_command,
		write_at,
		test,
		dtx = 0xfffe,
		crc_failed = 0xffff,
	};
}

class Message{
private:
	bool				get_header();
	//int32_t 			msg_start_mark;
	bool				check_crc();
public:
	//crc_result::result	crc;
	uint8_t 			raw_header[sizeof(Header)];
	Header&				header;
	CircBuffer&			cbuffer;
	CircBuffer 			peek;
	static 	uint8_t 	header_size;
	void				disp_header();
	bool 				ready;
	uint16_t			get_msg();
	Message(CircBuffer& buffer);
	operator bool(){
		return ready;
	}
};



#endif /* AVR_MESSAGE_PARSER_MESSAGE_PARSER_H_ */
