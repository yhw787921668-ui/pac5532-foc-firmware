// PAC52xx SDK Usage Notes
// 
// - When using the pac52xx_sdk, include the pac5xxx.h or pac52XX.h file to access PAC52xx register structures
//     and definitions.
// - Add PAC52xx driver header files as needed for access to driver levels functions (see examples below).
// - It's also important to include one of the device header files before accessing the AFE registers
//     that are included in pac5xxx_tile... headers (see example below).  Each analog module in the AFE 
//     is considered a "Tile".
// - Including the device header file defines the appropriate PAC52xx device (E.g. PAC5223)
//     and the associated CAFE Architecture (CAFE_ARCH1 or CAFE_ARCH2).

#include "pac5xxx.h"                        // pac5xxx.h or pac52XX.h also include most peripheral header files
#include "pac5xxx_driver_gpio.h"
#include "pac5xxx_driver_system.h"

// Include one of the following device headers before accessing AFE registers included in pac5xxx_tile... headers
//#include "pac5210.h"
//#include "pac5220.h"
//#include "pac5222.h"
//#include "pac5225.h"
#include "pac5223.h"
//#include "pac5232.h"
//#include "pac5250.h"
//#include "pac5253.h"
//#include "pac5255.h"
//#include "pac5256.h"
//#include "pac5285.h"

