#ifndef	__STATE_MECH_H
#define	__STATE_MECH_H
#include "TYPESS.H"
//״̬����-----------------------------------------

static bit emgstate;//����״̬���� 1���� 0����
static bit outputen;//������� 1���� 0������
static bit lock;//�ⲿ����ϵͳ 1���� 0����

static bit output;//��� 1��� 0�ر����


static bit overtemp;//���� 1���� 0����
static bit overvoltage;//��ѹ 1���� 0����
static bit undervoltage;//Ƿѹ 1���� 0����
static bit overcurrent;//���� 1���� 0����


static uint32_t cp_voltage;//cp�ߵ�ѹ����0.01v��λ

static uint8_t charge_state;//���״̬

static uint8_t cp_state;//cp�ߵ�λ
//0 6 9 12 255

#endif