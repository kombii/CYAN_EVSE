#include "sys_queue.h"
uint8_t queue_init(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len)
{
    uint16_t index;

    for(index = 0; index < Len; index++) {
	    PBase[index] = (uint8_t)0x00;
    }

    *Front = *Rear = 0;
    return 1;
}
uint8_t queue_full(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len)
{
    if((((*Rear) + 1) % Len) == *Front) {
        return 1;
    } else {
        return 0;
    }
}
uint8_t queue_empty(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len)
{
    if(*Front == *Rear) {
        return 1;
    } else {
        return 0;
    }
}
uint8_t queue_in(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len, uint8_t *PData)
{
    //DISABLE_ALL_IRQ();

    if(queue_full(Front, Rear, PBase, Len)) {
        return 0;
    }

    PBase[*Rear] = *PData;
    *Rear = ((*Rear) + 1) % Len;

    //ENABLE_ALL_IRQ();

    return 1;
}
uint8_t queue_out(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len, uint8_t *PData)
{
    //DISABLE_ALL_IRQ();

    if(queue_empty(Front, Rear, PBase, Len)) {
        return 0;
    }

    *PData = PBase[*Front];
    *Front = ((*Front) + 1) % Len;

    //ENABLE_ALL_IRQ();

    return 1;
}


