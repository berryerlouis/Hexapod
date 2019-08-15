/*
 * CmpPCA9685.h
 *
 * Created: 11/02/2014 12:00:20
 *  Author: berryer
 */ 


#ifndef CMPNRF24L01_H_
#define CMPNRF24L01_H_

#include "../Conf/ConfHard.h"

////////////////////////////////////////PUBLIC DEFINES/////////////////////////////////////////
#define NRF24L01_ADDRESS				0x40U


/* Memory Map */
#define NRF24L01_CONFIG					0x00U
#define NRF24L01_EN_AA					0x01U
#define NRF24L01_EN_RXADDR				0x02U
#define NRF24L01_SETUP_AW				0x03U
#define NRF24L01_SETUP_RETR				0x04U
#define NRF24L01_RF_CH					0x05U
#define NRF24L01_RF_SETUP				0x06U
#define NRF24L01_STATUS					0x07U
#define NRF24L01_OBSERVE_TX				0x08U
#define NRF24L01_CD						0x09U
#define NRF24L01_RX_ADDR_P0				0x0AU
#define NRF24L01_RX_ADDR_P1				0x0BU
#define NRF24L01_RX_ADDR_P2				0x0CU
#define NRF24L01_RX_ADDR_P3				0x0DU
#define NRF24L01_RX_ADDR_P4				0x0EU
#define NRF24L01_RX_ADDR_P5				0x0FU
#define NRF24L01_TX_ADDR				0x10U
#define NRF24L01_RX_PW_P0				0x11U
#define NRF24L01_RX_PW_P1				0x12U
#define NRF24L01_RX_PW_P2				0x13U
#define NRF24L01_RX_PW_P3				0x14U
#define NRF24L01_RX_PW_P4				0x15U
#define NRF24L01_RX_PW_P5				0x16U
#define NRF24L01_FIFO_STATUS			0x17U
#define NRF24L01_DYNPD					0x1CU
#define NRF24L01_FEATURE				0x1DU


/* Bit Mnemonics */
#define NRF24L01_MASK_RX_DR				6
#define NRF24L01_MASK_TX_DS				5
#define NRF24L01_MASK_MAX_RT			4
#define NRF24L01_EN_CRC					3
#define NRF24L01_CRCO					2
#define NRF24L01_PWR_UP					1
#define NRF24L01_PRIM_RX				0
#define NRF24L01_ENAA_P5				5
#define NRF24L01_ENAA_P4				4
#define NRF24L01_ENAA_P3				3
#define NRF24L01_ENAA_P2				2
#define NRF24L01_ENAA_P1				1
#define NRF24L01_ENAA_P0				0
#define NRF24L01_ERX_P5					5
#define NRF24L01_ERX_P4					4
#define NRF24L01_ERX_P3					3
#define NRF24L01_ERX_P2					2
#define NRF24L01_ERX_P1					1
#define NRF24L01_ERX_P0					0
#define NRF24L01_AW						0
#define NRF24L01_ARD					4
#define NRF24L01_ARC					0
#define NRF24L01_PLL_LOCK				4
#define NRF24L01_RF_DR					3
#define NRF24L01_RF_PWR					6
#define NRF24L01_RX_DR					6
#define NRF24L01_TX_DS					5
#define NRF24L01_MAX_RT					4
#define NRF24L01_RX_P_NO				1
#define NRF24L01_TX_FULL				0
#define NRF24L01_PLOS_CNT				4
#define NRF24L01_ARC_CNT				0
#define NRF24L01_TX_REUSE				6
#define NRF24L01_FIFO_FULL				5
#define NRF24L01_TX_EMPTY				4
#define NRF24L01_RX_FULL				1
#define NRF24L01_RX_EMPTY				0
#define NRF24L01_DPL_P5					5
#define NRF24L01_DPL_P4					4
#define NRF24L01_DPL_P3					3
#define NRF24L01_DPL_P2					2
#define NRF24L01_DPL_P1					1
#define NRF24L01_DPL_P0					0
#define NRF24L01_EN_DPL					2
#define NRF24L01_EN_ACK_PAY				1
#define NRF24L01_EN_DYN_ACK				0

/* Instruction Mnemonics */
#define NRF24L01_R_REGISTER				0x00
#define NRF24L01_W_REGISTER				0x20
#define NRF24L01_REGISTER_MASK			0x1F
#define NRF24L01_ACTIVATE				0x50
#define NRF24L01_R_RX_PL_WID			0x60
#define NRF24L01_R_RX_PAYLOAD			0x61
#define NRF24L01_W_TX_PAYLOAD			0xA0
#define NRF24L01_W_ACK_PAYLOAD			0xA8
#define NRF24L01_FLUSH_TX				0xE1
#define NRF24L01_FLUSH_RX				0xE2
#define NRF24L01_REUSE_TX_PL			0xE3
#define NRF24L01_NOP					0xFF

/* Non-P omissions */
#define NRF24L01_LNA_HCURR				0

/* P model memory Map */
#define NRF24L01_RPD					0x09

/* P model bit Mnemonics */
#define NRF24L01_RF_DR_LOW				5
#define NRF24L01_RF_DR_HIGH				3
#define NRF24L01_RF_PWR_LOW				1
#define NRF24L01_RF_PWR_HIGH			2

////////////////////////////////////////PUBLIC STRUCTURES/////////////////////////////////////////


////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////

Boolean CmpNRF24L01Init( Int8U addr, EIoPin CS );
Boolean CmpNRF24L01Reset( Int8U addr );
Boolean CmpNRF24L01SetDataRate( Int8U speed );
void CmpNRF24L01PowerUp( void );
void CmpNRF24L01PowerDown( void );
#endif /* CMPNRF24L01_H_ */