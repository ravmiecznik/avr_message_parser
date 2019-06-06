/*
 * message_parser.cpp
 *
 *  Created on: 23 mar 2019
 *      Author: rafal
 */

#include "message_parser.h"
#include <stdio.h>
#include <avr/delay.h>
#include <ctype.h>
#include <avr/crc16.h>
#include "../avr_ports/avr_ports.h"

extern AvrPin led_red, led_blue;

//Message

Message::Message(CircBuffer& buffer): cbuffer(buffer), header((Header&)*raw_header), peek(cbuffer.peek()){
	header.msg_len = 0;
	ready = false;
	int16_t timeout_ms = 100;
	uint8_t timeout_step = 5;
	if(get_header()){
		for(int32_t i=0; i<header.msg_len; i++)	//it should take about 36us to receive one char
			_delay_us(35);						//can't use formula in _delay_us()
		while(cbuffer.available < header.msg_len and (timeout_ms-=timeout_step) > 0){
			_delay_ms(timeout_step);
		}
		if(timeout_ms > 0){
			check_crc();
		}
		else{
			//printf("timeout\n");
			led_red = hi;
			cbuffer.flush();
			header.crc = msg_id::dtx;
			led_red = lo;
			//crc = crc_result::dtx;
		}
		ready = true;
	}
}

uint16_t Message::get_msg(){
	return 0;
}

bool Message::check_crc(){
	/*
	 * Msg should be at relational pos 0 (not absolute) in cbuffer
	 */
	uint32_t cnt = header.msg_len;
	uint16_t calc_crc = 0;
	char c;

	cbuffer.peek_sync(&peek);
	while(cnt--){
		c = peek.get();
		//print_buff_char(c);
		calc_crc = _crc_xmodem_update(calc_crc, c);
	}
	//printf("\n");
//	if(calc_crc == header.crc){
//		crc = crc_result::ack;
//	}
//	else{
//		crc = crc_result::nack;
//		cbuffer.flush();
//	}
	if(calc_crc != header.crc){
		header.id = msg_id::crc_failed;
	}
	return calc_crc == header.crc;
}

void Message::disp_header(){
	//Header h = (Header&)*raw_header;
	printf("%c\n", 				header.head_start);
	printf("id           %d\n", header.id);
	printf("len         %lu\n", header.msg_len);
	printf("crc(hex)  %X\n", 	header.crc);
	printf("%c\n", 				header.head_end);
}


bool Message::get_header(){
	/*
	 * Looks for header in cbuffer
	 * Fills raw_hader in search process
	 * returns True/Flase if valid header found
	 */


	//return false;
	bool found=false;
	char c;
	if(cbuffer.available >= header_size){
		while(cbuffer.available){
			cbuffer.peek_sync(&peek);
			c = peek.get();
			if(c == HEAD_START_MARK){
				raw_header[0] = c;
				for(uint8_t i=0; i<header_size-1; i++){
					//go to 10th char from current one and assign to raw_header
					c = peek.get();
					raw_header[i+1] = c;
				}
				if(c == HEAD_END_MARK){
					//success
					found = true;
					cbuffer.flush(header_size);
					break;
				}
			}
			//move to next mark point
			cbuffer.get();
		}
	}
	return found;
}
uint8_t Message::header_size = sizeof(Header);
