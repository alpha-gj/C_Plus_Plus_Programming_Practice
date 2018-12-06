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

/*  ENCODE */
int encodeToMcuPacket(vector<MCU_SUB_PACKET> vectorMcuPacket, char mcu_packet[])
{
	char *mcu_packet_header_buffer_iter = mcu_packet;
	char *sub_mcu_packet_buffer_iter = mcu_packet + HEAD_BYTE_NUMS;

	for (int i = 0; i < vectorMcuPacket.size(); i++) {
		
//		printf("vectorMcuPacket[%d].data_type is %x\n", i, vectorMcuPacket.at(i).data_type);
//		printf("vectorMcuPacket[%d].payload_len is %d\n", i, vectorMcuPacket.at(i).payload_len);
//		printf("vectorMcuPacket[%d].payload_context is %s\n", i, vectorMcuPacket.at(i).payload_context);

		/* Add data type */
		memcpy(sub_mcu_packet_buffer_iter,
				(const char*)&vectorMcuPacket.at(i).data_type,
				sizeof(vectorMcuPacket.at(i).data_type));
		sub_mcu_packet_buffer_iter = sub_mcu_packet_buffer_iter + sizeof(vectorMcuPacket.at(i).data_type);

		/* Add payload len */
		memcpy(sub_mcu_packet_buffer_iter, 
				(const char*)&vectorMcuPacket.at(i).payload_len,
				sizeof(vectorMcuPacket.at(i).payload_len));
		sub_mcu_packet_buffer_iter = sub_mcu_packet_buffer_iter + sizeof(vectorMcuPacket.at(i).payload_len);

		/* Add payload context */
		memcpy(sub_mcu_packet_buffer_iter,
			(const char*)&vectorMcuPacket.at(i).payload_context, 
			strlen((const char*)&vectorMcuPacket.at(i).payload_context));
		sub_mcu_packet_buffer_iter = sub_mcu_packet_buffer_iter + strlen((const char*)&vectorMcuPacket.at(i).payload_context);
	}

	/* Cal the total len of sub mcu packets */
	int total_sub_mcu_packet_len = sub_mcu_packet_buffer_iter - &mcu_packet[HEAD_BYTE_NUMS];

	/* Set the mcu_packet len to header location of mcu packet */
	*mcu_packet_header_buffer_iter = HEAD_BYTE_NUMS + total_sub_mcu_packet_len;
	
	fprintf(stderr, "MCU packet len is %d, include %d byte of header and %d byte of payload (all sub mcu packet len)\n",
			mcu_packet[0], 
			HEAD_BYTE_NUMS, 
			total_sub_mcu_packet_len);

	/* Check the mcu packet size */
	if (mcu_packet[0] > MCU_PACKET_SIZE) {
		fprintf(stderr, "MCU packet size is too big!\n");
		return -1;
	} else {
		fprintf(stderr, "MCU packet size is ok!\n");
		return 0;
	}
}

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
