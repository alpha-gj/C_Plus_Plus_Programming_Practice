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

typedef struct {
	unsigned char data_type;
	unsigned char payload_len;
	unsigned char payload_context[PAYLOAD_SIZE];
} MCU_SUB_PACKET;



MCU_SUB_PACKET init_mcu_sub_packet(unsigned char arg_data_type, char* arg_content) 
{
	MCU_SUB_PACKET mcu_sub_packet;
	memset(&mcu_sub_packet, 0, sizeof(mcu_sub_packet));

	/* Create */
	mcu_sub_packet.data_type = arg_data_type;
	mcu_sub_packet.payload_len = strlen(arg_content);
	strncpy((char* )mcu_sub_packet.payload_context, arg_content, PAYLOAD_SIZE);
	mcu_sub_packet.payload_context[mcu_sub_packet.payload_len] = '\0';

	return mcu_sub_packet;
}

int decodeToMcuPacket(unsigned char *mcu_packet) 
{
	unsigned char out_buffer[128] = {0};
	read_count = mcu_packet[0];

	sub_mcu_packet_start_count = 1;
	
	while (read_count != 0) {

		memset(&out_buffer, 0, sizeof(out_buffer));

		switch (mcu_packet[sub_mcu_packet_start_count]) {
			case MCU_DATA_FW_VER:
				printf("MCU_DATA_FW_VER");

				break;
			case MCU_DATA_CAM_NAME:
				printf("MCU_DATA_CAM_NAME");

				break;

		}

	}
	

	printf("This is %s\n", mcu_packet);
	return 0;
}

/* TODO Check MCU PAKCET SIZE shoud not more than 120 bytes */
/*  ENCODE */
int encodeToMcuPacket(vector<MCU_SUB_PACKET> vectorMcuPacket)
{
	unsigned char mcu_packet[MCU_PACKET_SIZE] = {0};
//	unsigned char *mcu_packet_buffer_iter = NULL;
//	mcu_packet_buffer_iter = mcu_packet;

#define HEAD_COUNT 1
	int copy_count = 0 + HEAD_COUNT;

	for (int i = 0; i < vectorMcuPacket.size(); i++) {
		
		printf("vectorMcuPacket[%d].data_type is %x\n", i, vectorMcuPacket.at(i).data_type);
		printf("vectorMcuPacket[%d].payload_len is %d\n", i, vectorMcuPacket.at(i).payload_len);
		printf("vectorMcuPacket[%d].payload_context is %s\n", i, vectorMcuPacket.at(i).payload_context);

		memcpy((unsigned char *)&mcu_packet + copy_count, (unsigned char *)&vectorMcuPacket.at(i).data_type, sizeof(vectorMcuPacket.at(i).data_type));
		copy_count = copy_count + sizeof(vectorMcuPacket.at(i).data_type);

		memcpy((unsigned char *)&mcu_packet + copy_count, (unsigned char *)&vectorMcuPacket.at(i).data_type, sizeof(vectorMcuPacket.at(i).payload_len));
		copy_count = copy_count + sizeof(vectorMcuPacket.at(i).payload_len);

		memcpy((unsigned char *)&mcu_packet + copy_count, (unsigned char *)&vectorMcuPacket.at(i).data_type, strlen((const char*)&vectorMcuPacket.at(i).payload_context));
		copy_count = copy_count + strlen((const char*)&vectorMcuPacket.at(i).payload_context);
	}

	printf("count is %d\n", copy_count);
	mcu_packet[0] = copy_count - HEAD_COUNT;
	printf("mcu_payload is %d\n", mcu_packet[0]);
	decodeToMcuPacket(mcu_packet);
	return 0;
}




/* OUTCODE */



int main(void)
{
	char *ptr_context = NULL;
	char fw_ver[10] = "1.0.0";
	char cam_name[10] = "gj_cam";

	vector<MCU_SUB_PACKET> vectorMcuPacket;

	/* */
	ptr_context = fw_ver;
	vectorMcuPacket.push_back(init_mcu_sub_packet(MCU_DATA_FW_VER, ptr_context));

	ptr_context = cam_name;
	vectorMcuPacket.push_back(init_mcu_sub_packet(MCU_DATA_CAM_NAME, ptr_context));

	int ret = encodeToMcuPacket(vectorMcuPacket);

	return 0;
}
