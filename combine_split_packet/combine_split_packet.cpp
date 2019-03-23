#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
//#define RF868_MAX_PAYLOAD 120
//u
#define RF868_MAX_PAYLOAD 20

/* CUSTOM STRUCT */ //typedef enum {
//    UNKNOWN,
//    LED,
//    BUTTON,
//} HW_TYPE;
//
//
#define UNKNOWN_HW_TYPE     0
#define LED_TYPE            1
#define BUTTON_TYPE         2

typedef struct {
    int array[RF868_MAX_PAYLOAD*3];
} LED_CTRL;

typedef struct {
    int array[12];
} BUTTON_CTRL;

/* RF868 CUSTOM PACKET */
//typedef enum {
//    COMPLETED,
//    NOT_COMPLETED,
//} PACKET_COMPLETED_STATUS;
//
#define PACKET_HAS_COMPLETED 0
#define PACKET_NOT_COMPLETED 1

typedef struct {
    unsigned char device_id;
    unsigned char seq_num;
    unsigned char hw_type;
    unsigned char packet_completed_status;
    unsigned char payload_size;
} RF868_CUSTOM_HEADER;

typedef struct {
    char buf[1024];
} RF868_CUSTOM_PAYLOAD;

/* MCU RF868 TX */
typedef struct {
    RF868_CUSTOM_HEADER header;
    RF868_CUSTOM_PAYLOAD payload;
} MCU_RF868_TX;


using namespace std;

int main(int /*argc*/ , char * /*args*/[])
{
    int ret = -1;

    /* STEP1. Setup content */
    BUTTON_CTRL button_ctrl;
    for (int i = 0; i < sizeof(button_ctrl.array)/sizeof(button_ctrl.array[0]); i++) {
        button_ctrl.array[i] = i;
        printf("button_ctrl.array[%d]: %d\n", i, button_ctrl.array[i]);
    }

    /* STEP2. Setup Payload: hw type, buf */
    RF868_CUSTOM_PAYLOAD payload;
    memcpy((char *)payload.buf, (const char*)&button_ctrl, sizeof(button_ctrl)); 
    printf("sizeof(button_ctrl): %d\n", sizeof(button_ctrl));

    for (int i = 0; i < sizeof(button_ctrl); i++) {
        printf("payload.buf[%d]: %d\n", i, payload.buf[i]);
    }
    
    /* STEP3. Encode the RF868 PACKET */
    printf ("RF868_MAX_PAYLOAD: %d\n", RF868_MAX_PAYLOAD);

    RF868_CUSTOM_HEADER header {
        .device_id = 1,
        .seq_num = 0,
        .hw_type= UNKNOWN_HW_TYPE,
        .packet_completed_status = PACKET_NOT_COMPLETED,
        .payload_size = 0,
    };
    header.hw_type = BUTTON_TYPE;
    printf("sizeof header: %d\n", sizeof(RF868_CUSTOM_HEADER));

    if (RF868_MAX_PAYLOAD <= sizeof(RF868_CUSTOM_HEADER)) {
        printf("RF868_MAX_PAYLOAD is not enough\n");
        return 0;
    }

    int pre_mcu_payload_size = sizeof(button_ctrl);
    printf("sizeof pre_mcu_payload_size: %d\n", pre_mcu_payload_size);

    char tx_buffer[1024] = {};
    char *tx_buffer_iter = tx_buffer;
    const char *header_iter = (const char *)&header;
    const char *pre_mcu_payload_iter = (const char *)payload.buf;
    int mcu_packet_len = 0; /* multi (len of sub_mcu_header + len sub_mcu_payload) */
    int sub_mcu_payload_size = RF868_MAX_PAYLOAD - sizeof(RF868_CUSTOM_HEADER);
    int to_be_processed_count = pre_mcu_payload_size;

    printf("sub_mcu_payload_size: %d\n", sub_mcu_payload_size);
    printf("to_be_processed_count: %d\n", to_be_processed_count);

    do {
        printf("\n");
        if (to_be_processed_count > sub_mcu_payload_size)  {

            header.packet_completed_status = PACKET_NOT_COMPLETED;
            header.payload_size = sub_mcu_payload_size;

        } else {

            header.packet_completed_status = PACKET_HAS_COMPLETED;
            if ((pre_mcu_payload_size % sub_mcu_payload_size) == 0) {
                header.payload_size = sub_mcu_payload_size;
                printf("sub_mcu_payload_size  pre_mcu_payload_size == 0, so payload_size is %d\n", header.payload_size);
            } else {
                header.payload_size = pre_mcu_payload_size % sub_mcu_payload_size;
                printf("sub_mcu_payload_size  pre_mcu_payload_size != 0, so payload_size is %d\n", header.payload_size);
                printf("remain header.payload_size: %d\n", header.payload_size);
            }
        }

        printf("\n****** packet start ******\n");
        /* process sub header */
        memcpy(tx_buffer_iter, header_iter, sizeof(RF868_CUSTOM_HEADER));
        tx_buffer_iter = tx_buffer_iter + sizeof(RF868_CUSTOM_HEADER);
        printf("sizeof(RF868_CUSTOM_HEADER) is %d\n", sizeof(RF868_CUSTOM_HEADER));

        /* process sub payload */
        memcpy(tx_buffer_iter, pre_mcu_payload_iter, header.payload_size);
        tx_buffer_iter = tx_buffer_iter + header.payload_size;
        pre_mcu_payload_iter = pre_mcu_payload_iter + header.payload_size;
        printf("header.payload_size is %d\n", header.payload_size);
        printf("****** packet end ******\n");

        /* count */
        header.seq_num++;
        mcu_packet_len = mcu_packet_len + sizeof(RF868_CUSTOM_HEADER) + header.payload_size;  
        to_be_processed_count = to_be_processed_count - sub_mcu_payload_size;

    } while (header.packet_completed_status != PACKET_HAS_COMPLETED);

    printf("\n====== TX finish ======\n");
    printf("to_be_processed_count: %d\n", to_be_processed_count);
    printf("mcu_packet_len: %d\n", mcu_packet_len);
    printf("send count: %d\n", header.seq_num);


#if 1
    for (int j = 0; j < mcu_packet_len; j++) {
        printf("tx_buffer[%d]:%d\n", j, tx_buffer[j]);
    }
#endif

    /* === Receive === */
    printf("\n\n====== RECEIVE ======\n");
    const char *rx_buffer_iter = tx_buffer;
    char header_buffer[1024] = {};
    char payload_buffer[1024] = {};
    char *payload_buffer_iter = payload_buffer;
    int payload_count = 0;
    RF868_CUSTOM_HEADER *p_header = NULL;
    do {

        /* Decode sub header */
        memset(&header_buffer, 0, sizeof(header_buffer));
        memcpy(header_buffer, rx_buffer_iter, sizeof(RF868_CUSTOM_HEADER));
        rx_buffer_iter = rx_buffer_iter + sizeof(RF868_CUSTOM_HEADER);
        p_header = (RF868_CUSTOM_HEADER *)&header_buffer;
        printf("p_header->device_id:%d\n", p_header->device_id);
        printf("p_header->seq_num:%d\n", p_header->seq_num);
        printf("p_header->payload_size:%d\n", p_header->payload_size);
        switch (p_header->packet_completed_status) {
            case PACKET_HAS_COMPLETED:
                printf("COMPLETED\n");
                break;
            case PACKET_NOT_COMPLETED :
                printf("NOT COMPLETED\n");
                break;
            default:
                printf("Not match complete status\n");
                break;
        };

        /* Decode sub payload and composit it */
        memcpy(payload_buffer_iter, rx_buffer_iter, p_header->payload_size);
        payload_buffer_iter = payload_buffer_iter + p_header->payload_size;
        rx_buffer_iter = rx_buffer_iter + p_header->payload_size;
        
        //TODO check seq_num

        /* count */
        payload_count = payload_count + p_header->payload_size;

    } while (p_header->packet_completed_status != PACKET_HAS_COMPLETED /* && seq_num != failed */);

    printf("payload_count:%d\n", payload_count);

#if 1
    if (p_header->hw_type == BUTTON_TYPE) {
        BUTTON_CTRL *rx_button_iter = (BUTTON_CTRL *)payload_buffer;

        for (int k = 0; k < sizeof(button_ctrl.array)/sizeof(button_ctrl.array[0]); k++) {
            printf("rx_button_iter->array[%d]: %d\n", k, rx_button_iter->array[k]);
        }
    }
#endif
    

//    ret = AHAL_MCU_SendData(AHAL_MCU_ID_RF, 1, (AHAL_VOID *)&hw_ctrl, sizeof(hw_ctrl));
//    if (ret != AHAL_RET_SUCCESS) {
//        printf("AHAL_MCU_SendData fail\n");
//        return -1;
//    }

    return 0;

}

