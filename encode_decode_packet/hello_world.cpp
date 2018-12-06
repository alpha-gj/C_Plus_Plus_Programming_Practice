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

/* For mcu packet*/
#define MCU_PACKET_HEADER_BYTES				1
// 1 byte: how long is mcu packet

/* For sub mcu packet*/
#define SUB_MCU_PACKET_HEADER_BYTES			2
#define OFFSET_SUB_MCU_HEADER_DATA_TYPE		0
#define OFFSET_SUB_MCU_HEADER_PAYLOAD_LEN	1
// 1 byte: data type of sub mcu packet
// 1 byte: payload len of sub mcu packet

typedef struct {
	char data_type;
	char payload_len;
	char payload_context[PAYLOAD_SIZE];
} MCU_SUB_PACKET;

/*  DECODE */
int decodeMcuPacket(char mcu_packet[]) 
{
	char out_buffer[MCU_PACKET_SIZE] = {0};
	int total_sub_mcu_packets_len = mcu_packet[0] - MCU_PACKET_HEADER_BYTES;
	int sub_mcu_packet_start_count = MCU_PACKET_HEADER_BYTES; //first sub mcu packet start
	unsigned char sub_mcu_packet_payload_len = 0;
	int	read_count = 0;

	while (read_count < total_sub_mcu_packets_len) {

		memset(&out_buffer, 0, sizeof(out_buffer));

		switch (mcu_packet[sub_mcu_packet_start_count]) {

			case MCU_DATA_FW_VER:
				printf("case: MCU_DATA_FW_VER\n");
				break;

			case MCU_DATA_CAM_NAME:
				printf("case: MCU_DATA_CAM_NAME\n");
				break;

			case MCU_DATA_WIFI_STATUS:
				printf("case: MCU_DATA_WIFI_STATUS\n");
				break;

			default:
				printf("case: No match, exit\n");
				exit(0);
		}

		/* Printf sub muc packet */
		printf("data type: %x\n", mcu_packet[sub_mcu_packet_start_count]);

		sub_mcu_packet_payload_len = mcu_packet[sub_mcu_packet_start_count + OFFSET_SUB_MCU_HEADER_PAYLOAD_LEN];
		printf("patload_len is %d\n", sub_mcu_packet_payload_len);

		memcpy(out_buffer, (const char *)&mcu_packet[sub_mcu_packet_start_count + SUB_MCU_PACKET_HEADER_BYTES], sub_mcu_packet_payload_len);
		out_buffer[sub_mcu_packet_payload_len] = '\0';
		printf("patload_context is %s\n\n", out_buffer);

		/* decide Next sub mcu packet location */
		read_count = read_count + SUB_MCU_PACKET_HEADER_BYTES + sub_mcu_packet_payload_len;
		sub_mcu_packet_start_count = sub_mcu_packet_start_count + SUB_MCU_PACKET_HEADER_BYTES + sub_mcu_packet_payload_len;

	}

	return 0;
}

/*  ENCODE */
int encodeToMcuPacket(vector<MCU_SUB_PACKET> vectorMcuPacket, char mcu_packet[])
{
	char *mcu_packet_header_buffer_iter = mcu_packet;
	char *sub_mcu_packet_buffer_iter = mcu_packet + MCU_PACKET_HEADER_BYTES;

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
	int total_sub_mcu_packet_len = sub_mcu_packet_buffer_iter - &mcu_packet[MCU_PACKET_HEADER_BYTES];

	/* Set the mcu_packet len to header location of mcu packet */
	*mcu_packet_header_buffer_iter = MCU_PACKET_HEADER_BYTES + total_sub_mcu_packet_len;
	
	fprintf(stderr, "MCU packet len is %d, include %d byte of header and %d byte of payload (all sub mcu packet len)\n",
			mcu_packet[0], 
			MCU_PACKET_HEADER_BYTES, 
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
	fprintf(stderr, "\n ****** Encode ******\n");
	char mcu_packet[MCU_PACKET_SIZE] = {0};
	encodeToMcuPacket(vectorMcuPacket,  mcu_packet);

	/* STEP3. Decode the mcu packet into multiple mcu sub packets*/
	fprintf(stderr, "\n ****** Decode ******\n");
	decodeMcuPacket(mcu_packet);

	return 0;
}
