
#include <stdint.h>
#include "ring_buffer.h"
#include <string.h>

int ring_buffer_init(ring_buffer_t *rbuffer,  uint32_t total_size)
{
    rbuffer->lock = NULL;
    rbuffer->lock = xSemaphoreCreateMutex();
    if(NULL == rbuffer->lock)
    {
        printf("ring buffer lock creat error\n");
        return RETURN_ERR;
    }
    
    rbuffer->base_addr = malloc(total_size);
    printf("rbuffer->base_addr addr_check: %p\n",rbuffer->base_addr);
    if(rbuffer->base_addr == NULL)
    {
        printf("malloc_psram faile\n");
        return RETURN_ERR;
    }

    rbuffer->total_size = total_size;
    
    rbuffer->data_cnt = 0;    
    rbuffer->wp = 0;
    rbuffer->rp = 0;
    
    //rbuffer->wp_th = total_size*7/8;
    return RETURN_OK;
}

int ring_buffer_write(ring_buffer_t *rbuffer, uint8_t *data_addr, uint32_t size)
{
    uint32_t copy_lens;
    uint8_t *copy_addr;

    xSemaphoreTake(rbuffer->lock,portMAX_DELAY);
    if((rbuffer->data_cnt + size) > (rbuffer->total_size))
    {
       // x3_printf("ring buffer is no enough space!\n");
        xSemaphoreGive(rbuffer->lock);
        return RETURN_ERR;
    }
    if((rbuffer->wp + size) > rbuffer->total_size)
    {
        copy_lens = rbuffer->total_size - rbuffer->wp;

        
        memcpy((void *)(rbuffer->base_addr + rbuffer->wp), (void *)data_addr, copy_lens);
        copy_addr = data_addr + copy_lens;
        copy_lens = size - copy_lens;
        memcpy((void *)(rbuffer->base_addr), (void *)copy_addr, copy_lens);
    } 
    else
    {
        memcpy((void *)(rbuffer->base_addr + rbuffer->wp), (void *)data_addr, size);
    }

    rbuffer->wp += size;
    if(rbuffer->wp >= rbuffer->total_size)
    {
       rbuffer->wp -= rbuffer->total_size; 
    }

    rbuffer->data_cnt += size;
    xSemaphoreGive(rbuffer->lock);
    return RETURN_OK;
}


int ring_buffer_read(ring_buffer_t *rbuffer, uint8_t *data_addr, uint32_t size)
{
    uint32_t copy_lens;
    uint8_t *copy_addr;
    
    xSemaphoreTake(rbuffer->lock,portMAX_DELAY);
    if(rbuffer->data_cnt <= 0)
    {
       // x3_printf("ring buffer is no enough data!\n");
        xSemaphoreGive(rbuffer->lock);
        return RETURN_ERR;
    }
    
    if(size >= rbuffer->data_cnt)
    {
        size = rbuffer->data_cnt;
        if((rbuffer->rp + size) > rbuffer->total_size)
        {
            copy_lens = rbuffer->total_size - rbuffer->rp;
            memcpy((void *)data_addr, (void *)(rbuffer->base_addr + rbuffer->rp), copy_lens);     
            copy_addr = data_addr + copy_lens;
            copy_lens = size - copy_lens;
            memcpy((void *)(copy_addr), (void *)(rbuffer->base_addr), copy_lens);
        } 
        else
        {
            memcpy((void *)data_addr, (void *)(rbuffer->base_addr + rbuffer->rp), size);
        }
        
        rbuffer->rp += rbuffer->data_cnt; 
        rbuffer->data_cnt = 0;
        if(rbuffer->rp >= rbuffer->total_size)
        {
           rbuffer->rp -= rbuffer->total_size; 
        }
        xSemaphoreGive(rbuffer->lock);
        return size;
    }

    if((rbuffer->rp + size) > rbuffer->total_size)
    {
        copy_lens = rbuffer->total_size - rbuffer->rp;
        memcpy((void *)data_addr, (void *)(rbuffer->base_addr + rbuffer->rp), copy_lens);     
        copy_addr = data_addr + copy_lens;
        copy_lens = size - copy_lens;
        memcpy((void *)(copy_addr), (void *)(rbuffer->base_addr), copy_lens);
    } 
    else
    {
        memcpy((void *)data_addr, (void *)(rbuffer->base_addr + rbuffer->rp), size);
    }

    rbuffer->rp += size;
    if(rbuffer->rp >= rbuffer->total_size)
    {
       rbuffer->rp -= rbuffer->total_size; 
    }

    rbuffer->data_cnt -= size;

    xSemaphoreGive(rbuffer->lock);
    return size;
}


uint32_t ring_buffer_get_size(ring_buffer_t *rbuffer)
{
    if(rbuffer != NULL)
    {
        return rbuffer->data_cnt;
    }
    else
    {
        return RETURN_ERR;
    }
    
}



uint32_t ring_buffer_get_leftsize(ring_buffer_t *rbuffer)
{
    if(rbuffer != NULL)
    {
        return rbuffer->total_size - rbuffer->data_cnt;
    }
    else
    {
        return RETURN_ERR;
    }
    
}

int ring_buffer_clear(ring_buffer_t *rbuffer)
{
    if(NULL == rbuffer->lock)
    {
        printf("ring buffer lock error\n");
        return RETURN_ERR;
    }
    
    memset(rbuffer->base_addr,0x00,rbuffer->data_cnt);
   
    rbuffer->data_cnt = 0;    
    rbuffer->wp = 0;
    rbuffer->rp = 0; 
    
    return RETURN_OK;
}


int ring_buffer_free(ring_buffer_t *rbuffer)
{
    if(NULL == rbuffer->lock)
    {
        printf("ring buffer lock error\n");
        return RETURN_ERR;
    }
    
    if(rbuffer->base_addr)
    {
        free(rbuffer->base_addr);
        rbuffer->base_addr = NULL;
    }
   
    rbuffer->data_cnt = 0;    
    rbuffer->wp = 0;
    rbuffer->rp = 0; 
    
    return RETURN_OK;
}



