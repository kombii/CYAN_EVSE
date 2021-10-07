#ifndef __SYS_QUEUE_H
#define __SYS_QUEUE_H

#include "TYPESS.H"

extern uint8_t queue_init(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len);
extern uint8_t queue_full(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len);
extern uint8_t queue_empty(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len);
extern uint8_t queue_in(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len, uint8_t *PData);
extern uint8_t queue_out(uint16_t *Front, uint16_t *Rear, uint8_t *PBase, uint16_t Len, uint8_t *PData);

//queue init.
#define QUEUE_INIT(q)       queue_init((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)))
//queue full?
#define QUEUE_FULL(q)       queue_full((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)))
//queue empty?
#define QUEUE_EMPTY(q)      queue_empty((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)))
//put pdata in queue
#define QUEUE_IN(q, apdata)  queue_in((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)),(uint8_t *) &apdata)
//get byte from queue
#define QUEUE_OUT(q, apdata) queue_out((uint16_t *) &(q.Front),(uint16_t *) &(q.Rear),(uint8_t *) &(q.PBase[0]),(uint16_t) (sizeof(q.PBase)),(uint8_t *) &apdata)

//8 Bytes Queue Struct
 typedef struct queue8 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[8 + 1];
} QUEUE8_S;

//16 Bytes Queue Struct
 typedef struct queue16 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[16 + 1];
} QUEUE16_S;

//32 Bytes Queue Struct
 typedef struct queue32 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[32 + 1];
} QUEUE32_S;

//64 Bytes Queue Struct
 typedef struct queue64 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[64 + 1];
} QUEUE64_S;

//128 Bytes Queue Struct
 typedef struct queue128 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[128 + 1];
} QUEUE128_S;

//256 Bytes Queue Struct
 typedef struct queue256 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[256 + 1];
} QUEUE256_S;

 typedef struct queue1024 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[1024 + 1];
} QUEUE1024_S;

 typedef struct queue2048 {
    uint16_t    Front;
    uint16_t    Rear;
    uint8_t     PBase[2048 + 1];
} QUEUE2048_S;


#endif /* __SYS_QUEUE_H */

