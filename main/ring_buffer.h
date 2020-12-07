/**
******************************************************************************
* @file    ring_buffer.h 
* @author  wenfeng.wang@chipintelli.com   
* @version V1.0.0
* @date    2018.06.30
* @brief  help function
******************************************************************************
*/

#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define RETURN_ERR -1
#define RETURN_OK 0

/**************************************************************************
                    type define 
****************************************************************************/
/*need freertos mutex, data can't overlap, need copy times*/
typedef struct
{
    uint8_t *base_addr;
    uint32_t total_size;
    SemaphoreHandle_t lock;

    uint32_t wp;
    uint32_t rp;
    uint32_t data_cnt;
    //uint32_t wp_th;/*no used !!!*/
}ring_buffer_t;

/*
typedef struct
{
    uint32_t wp;
    uint32_t rp;
    uint32_t data_cnt;    
}prev_mp3_data_info_t;

*/

int ring_buffer_init(ring_buffer_t *rbuffer, uint32_t total_size);
int ring_buffer_write(ring_buffer_t *rbuffer, uint8_t *data_addr, uint32_t size);
int ring_buffer_read(ring_buffer_t *rbuffer, uint8_t *data_addr, uint32_t size);
uint32_t ring_buffer_get_size(ring_buffer_t *rbuffer);
uint32_t ring_buffer_get_leftsize(ring_buffer_t *rbuffer);
//extern prev_mp3_data_info_t ring_buffer_get_status(ring_buffer_t *rbuffer);
int ring_buffer_clear(ring_buffer_t *rbuffer);
int ring_buffer_free(ring_buffer_t *rbuffer);



#ifdef __cplusplus
}
#endif

#endif


