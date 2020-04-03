/*
 * message_parser.h
 *
 *  Created on: 23 mar 2019
 *      Author: rafal
 */

#ifndef AVR_MESSAGE_PARSER_RXMESSAGE_PARSER_H_
#define AVR_MESSAGE_PARSER_RXMESSAGE_PARSER_H_

#include <stdint.h>
#include <stdlib.h>
#include "../atm_cbuffer/cbuffer.h"

#define HEAD_START_MARK	'>'
#define HEAD_END_MARK	'<'


struct Header{
	char 		head_start;	//'>'
	uint16_t	id;
	uint16_t	context;
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

namespace rx_id{
	enum id{
		txt_command,
		write_at,
		rxflush,
		setbankname,		//to be removed, defined twice, use it for different purposes ?
		get_sram_packet,
		get_bank_packet,
		enable_sram,
		update_sram,
		update_sram_bytes,
		handshake,
		get_write_stats,
		bootloader_safe,
		disable_bootloader,
		dummy,
		reset_emu,
		bank1_set,
		bank2_set,
		bank3_set,
		get_bank_in_use,
		set_bank_name,
		digifant_code_check,
		set_pin,
		apply_digidiag,
		disable_digidiag,
		wipe_banks,
		bootloader_old,
		get_banks_info,
		reset_banks_info,
		update_bank_data,
		freemem,
		//--special commands--
			test,
			nack = 0xfffd,
			dtx = 0xfffe,
			crc_failed = 0xffff,

	};
}

class RxMessage{
private:
	bool				get_header();
	bool				check_crc();
public:
	uint8_t 			raw_header[sizeof(Header)];
	Header&				header;
	CircBuffer&			cbuffer;
	CircBuffer 			peek;
	static 	uint8_t 	header_size;
	void				disp_header();
	bool 				ready;
	uint16_t			get_msg();
	RxMessage(CircBuffer& buffer);
	operator bool(){
		return ready;
	}
	operator uint16_t(){
		return header.context;
	}
};

bool check_crc(RxMessage& message);

#endif /* AVR_MESSAGE_PARSER_RXMESSAGE_PARSER_H_ */
