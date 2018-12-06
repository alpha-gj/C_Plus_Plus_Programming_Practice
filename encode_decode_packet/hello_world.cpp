#include <iostream>
#include<stdlib.h>
#include<stdio.h>
#include <vector>
#include <string.h>
using namespace std;


/* Data type for MCU_PACKET */
#define MCU_DATA_FW_VER				0x01
#define MCU_DATA_CAM_NAME			0x02
#define MCU_DATA_WIFI_STATUS		0x03

/* Payload size for MCU_PACKET */
#define PAYLOAD_SIZE				32

/* MCU_PACKET_SIZE*/
#define MCU_PACKET_SIZE				120

#define HEAD_BYTE_NUMS				1
/* Need to know how long is header of MCU_PACKET */

typedef struct {
	char data_type;
	char payload_len;
	char payload_context[PAYLOAD_SIZE];
} MCU_SUB_PACKET;



MCU_SUB_PACKET init_mcu_sub_packet(unsigned char arg_data_type, char arg_content[]) 
{
	MCU_SUB_PACKET mcu_sub_packet = {0};

	/* Create */
	mcu_sub_packet.data_type = arg_data_type;
	mcu_sub_packet.payload_len = strlen(arg_content);
	strncpy(mcu_sub_packet.payload_context, arg_content, PAYLOAD_SIZE);
	mcu_sub_packet.payload_context[mcu_sub_packet.payload_len] = '\0';

	return mcu_sub_packet;
}

int decodeMcuPacket(char *mcu_packet) 
{
	char out_buffer[128] = {0};
	int read_count = mcu_packet[0] - HEAD_BYTE_NUMS;
	int sub_mcu_packet_start_count = 1;
	
	/* For sub_mcu_packet */
	unsigned char sub_mcu_packet_payload_len;
	
	while (read_count > 0) {

		memset(&out_buffer, 0, sizeof(out_buffer));

		switch (mcu_packet[sub_mcu_packet_start_count]) {
			case MCU_DATA_FW_VER:
				printf("case: MCU_DATA_FW_VER\n");
				printf("type is %x\n", mcu_packet[sub_mcu_packet_start_count]);
				sub_mcu_packet_payload_len = mcu_packet[sub_mcu_packet_start_count + 1];
				printf("len is %d\n", sub_mcu_packet_payload_len);
				memcpy(out_buffer, (const char *)&mcu_packet[sub_mcu_packet_start_count+2], sub_mcu_packet_payload_len);
				out_buffer[sub_mcu_packet_payload_len] = '\0';
				printf("MCU_DATA_FW_VER is %s\n", out_buffer);
				printf("read_count is %d\n", read_count);
				

				break;
			case MCU_DATA_CAM_NAME:
				printf("case: MCU_DATA_CAM_NAME\n");
				printf("type is %x\n", mcu_packet[sub_mcu_packet_start_count]);
				sub_mcu_packet_payload_len = mcu_packet[sub_mcu_packet_start_count + 1];
				printf("len is %d\n", sub_mcu_packet_payload_len);
				memcpy(out_buffer, (const char *)&mcu_packet[sub_mcu_packet_start_count+2], sub_mcu_packet_payload_len);
				out_buffer[sub_mcu_packet_payload_len] = '\0';
				printf("MCU_DATA_CAM_NAME is %s\n", out_buffer);
				printf("read_count is %d\n", read_count);
				break;
			case MCU_DATA_WIFI_STATUS:
				printf("case: MCU_DATA_WIFI_STATUS\n");
				printf("type is %x\n", mcu_packet[sub_mcu_packet_start_count]);
				sub_mcu_packet_payload_len = mcu_packet[sub_mcu_packet_start_count + 1];
				printf("len is %d\n", sub_mcu_packet_payload_len);
				memcpy(out_buffer, (const char *)&mcu_packet[sub_mcu_packet_start_count+2], sub_mcu_packet_payload_len);
				out_buffer[sub_mcu_packet_payload_len] = '\0';
				printf("MCU_DATA_WIFI_STATUS is %s\n", out_buffer);
				printf("read_count is %d\n", read_count);
				break;
			default:
				printf("case: No match\n");
				break;

		}

		read_count = read_count - 2 - sub_mcu_packet_payload_len;
		printf("main read_count is %d\n", read_count);
		sub_mcu_packet_start_count = sub_mcu_packet_start_count + 2 + sub_mcu_packet_payload_len;
	}

	printf("read_count is %d\n", read_count);
	

//	printf("This is %s\n", mcu_packet);
	return 0;
}

/* TODO Check MCU PAKCET SIZE shoud not more than 120 bytes */
/*  ENCODE */
int encodeToMcuPacket(vector<MCU_SUB_PACKET> vectorMcuPacket, char mcu_packet[])
{
	char *mcu_packet_header_buffer_iter = mcu_packet;
	char *sub_mcu_packet_buffer_iter = mcu_packet + HEAD_BYTE_NUMS;

	for (int i = 0; i < vectorMcuPacket.size(); i++) {
		
//		printf("vectorMcuPacket[%d].data_type is %x\n", i, vectorMcuPacket.at(i).data_type);
//		printf("vectorMcuPacket[%d].payload_len is %d\n", i, vectorMcuPacket.at(i).payload_len);
//		printf("vectorMcuPacket[%d].payload_context is %s\n", i, vectorMcuPacket.at(i).payload_context);

		memcpy(sub_mcu_packet_buffer_iter,
				(const char*)&vectorMcuPacket.at(i).data_type,
				sizeof(vectorMcuPacket.at(i).data_type));
		sub_mcu_packet_buffer_iter = sub_mcu_packet_buffer_iter + sizeof(vectorMcuPacket.at(i).data_type);

		memcpy(sub_mcu_packet_buffer_iter, 
				(const char*)&vectorMcuPacket.at(i).payload_len,
				sizeof(vectorMcuPacket.at(i).payload_len));
		sub_mcu_packet_buffer_iter = sub_mcu_packet_buffer_iter + sizeof(vectorMcuPacket.at(i).payload_len);

		memcpy(sub_mcu_packet_buffer_iter,
			(const char*)&vectorMcuPacket.at(i).payload_context, 
			strlen((const char*)&vectorMcuPacket.at(i).payload_context));
		sub_mcu_packet_buffer_iter = sub_mcu_packet_buffer_iter + strlen((const char*)&vectorMcuPacket.at(i).payload_context);
	}

	printf("count is %d\n", (sub_mcu_packet_buffer_iter - &mcu_packet[HEAD_BYTE_NUMS]));
	*mcu_packet_header_buffer_iter = HEAD_BYTE_NUMS + sub_mcu_packet_buffer_iter - &mcu_packet[HEAD_BYTE_NUMS];
	printf("mcu_payload is %d\n", mcu_packet[0]);
	return 0;
}




/* OUTCODE */



int main(void)
{
	/* STEP1. Produce the mcu sub packet */
	char fw_ver[10] = "1.0.0";
	char cam_name[10] = "gj_cam";
	char wifi_status[10] = "yes";

	vector<MCU_SUB_PACKET> vectorMcuPacket;
	vectorMcuPacket.push_back(init_mcu_sub_packet(MCU_DATA_FW_VER, fw_ver));
	vectorMcuPacket.push_back(init_mcu_sub_packet(MCU_DATA_CAM_NAME, cam_name));
	vectorMcuPacket.push_back(init_mcu_sub_packet(MCU_DATA_WIFI_STATUS, wifi_status));

	/* STEP2. Encode multiple mcu sub packets into a mcu packet */
	char mcu_packet[MCU_PACKET_SIZE] = {0};
	encodeToMcuPacket(vectorMcuPacket,  mcu_packet);

	/* STEP3. Decode the mcu packet into multiple mcu sub packets*/
	decodeMcuPacket(mcu_packet);

	return 0;
}
