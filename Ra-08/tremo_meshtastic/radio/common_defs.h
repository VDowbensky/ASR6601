#ifndef _COMMON_DEFS_H_
#define _COMMON_DEFS_H_

#define TCXO_1V6								0
#define TCXO_1V7								1
#define TCXO_1V8								2
#define TCXO_2V2								3
#define TCXO_2V4								4
#define TCXO_2V7								5
#define TCXO_3V0								6
#define TCXO_3V3								7

#define LORA_BW_7p8             0
#define LORA_BW_10p4            0x08
#define LORA_BW_15p6            0x01
#define LORA_BW_20p8            0x09
#define LORA_BW_31p3            0x02
#define LORA_BW_41p7            0x0a
#define LORA_BW_62p5            0x03
#define LORA_BW_125             0x04
#define LORA_BW_250             0x05
#define LORA_BW_500             0x06
#define LORA_BW_203          		0x0d
#define LORA_BW_406          		0x0e
#define LORA_BW_812          		0x0f

#define LORA_SF_6	             	6
#define LORA_SF_7	             	7
#define LORA_SF_8	             	8
#define LORA_SF_9	             	9
#define LORA_SF_10	            10
#define LORA_SF_11	            11
#define LORA_SF_12	            12

#define LORA_CR_4_5             1
#define LORA_CR_4_6             2
#define LORA_CR_4_7             3
#define LORA_CR_4_8             4
#define LORA_CR_4_5_LI          5
#define LORA_CR_4_6_LI          6
#define LORA_CR_4_8_LI          7

#define LORALOW_DR_OPT_DISABLE   	0x00
#define LORA_LOW_DR_OPT_ENABLE    0x01

#define FSK_BW_4p8              0x1f
#define FSK_BW_5p8              0x17
#define FSK_BW_7p3              0x0f
#define FSK_BW_9p7              0x1e
#define FSK_BW_11p7             0x16
#define FSK_BW_14p6             0x0e
#define FSK_BW_19p5             0x1d
#define FSK_BW_23p4             0x15
#define FSK_BW_29p3             0x0d
#define FSK_BW_39               0x1c
#define FSK_BW_46p9             0x14
#define FSK_BW_58p6             0x0c
#define FSK_BW_78p2             0x1b
#define FSK_BW_93p8             0x13
#define FSK_BW_117p3            0x0b
#define FSK_BW_156p2            0x1a
#define FSK_BW_187p2            0x12
#define FSK_BW_234p3            0x0a
#define FSK_BW_312              0x19
#define FSK_BW_373p6            0x11
#define FSK_BW_467              0x09
#define FSK_BW_INVALID          0x00

#define FSK_BT0                 0
#define FSK_BT0p3               0x08
#define FSK_BT0p5               0x09
#define FSK_BT0p7               0x0a
#define FSK_BT1                 0x0b
#define FSK_BT_RCOS0p7					0x16

#define FSK_PREDET_0					 0x00
#define FSK_PREDET_8					 0x04
#define FSK_PREDET_16					 0x05
#define FSK_PREDET_24					 0x06
#define FSK_PREDET_32					 0x07

#define FSK_ADDR_FILT_DISABLED		0x00
#define FSK_ADDR_FILT_NODE				0x01
#define FSK_ADDR_FILT_NODE_BR			0x02

#define FSK_CRC_OFF             1
#define FSK_CRC_1B              0
#define FSK_CRC_2B              2
#define FSK_CRC_1B_INV          4
#define FSK_CRC_2B_INV          6

#endif
