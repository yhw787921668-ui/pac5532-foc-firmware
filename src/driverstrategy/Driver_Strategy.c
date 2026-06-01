/*
 * File: Driver_Strategy.c
 *
 * Code generated for Simulink model 'Driver_Strategy'.
 *
 * Model version                  : 1.494
 * Simulink Coder version         : 9.3 (R2020a) 18-Nov-2019
 * C/C++ source code generated on : Sun Jul 21 16:10:40 2024
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objective: ROM efficiency
 * Validation result: Not run
 */

#include "Driver_Strategy.h"

/* Named constants for Chart: '<Root>/Acc_selfcheck' */
#define Driver_Strategy_IN_Step1       ((uint8_T)1U)
#define Driver_Strategy_IN_Step2       ((uint8_T)2U)
#define Driver_Strategy_IN_Step3       ((uint8_T)3U)
#define Driver_Strategy_IN_Step4       ((uint8_T)4U)

/* Named constants for Chart: '<Root>/Enable_delay' */
#define Driver_Stra_IN_Timer_Accumulate ((uint8_T)3U)
#define Driver_Strategy_IN_Enable_State ((uint8_T)1U)
#define Driver_Strategy_IN_Output_State ((uint8_T)2U)

/* Exported block signals */
real_T mod_espd;                       /* '<Root>/Input' */
uint8_T mod_gear;                      /* '<Root>/Input2' */
real_T mod_acc_data_read;              /* '<Root>/Input1' */
real_T mod_bus_volt_read;              /* '<Root>/Input3' */
uint32_T mod_fault;                    /* '<Root>/Input5' */
uint8_T mod_direction;                 /* '<Root>/Input4' */
real_T Acc_r;                          /* '<S6>/Divide' */
real_T delta_r_in;                     /* '<Root>/Product' */
real_T mod_id_cmd;                     /* '<Root>/Saturation1' */
real_T mod_iq_cmd;                     /* '<S3>/Switch2' */
real_T Acc_r_init;                     /* '<Root>/Acc_selfcheck' */
uint8_T Acc_state_out;                 /* '<Root>/Acc_selfcheck' */
boolean_T mod_motor_enable;            /* '<S4>/Logical Operator1' */
boolean_T mod_init_false;              /* '<Root>/Relational Operator' */

/* Constant parameters (default storage) */
const ConstP_Driver_Strategy_T Driver_Strategy_ConstP = {
  /* Pooled Parameter (Expression: [0:2:40])
   * Referenced by:
   *   '<S10>/ID_MAP_LIMIT'
   *   '<S10>/IQ_MAP_LIMIT'
   */
  { 0.0, 2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0, 20.0, 22.0, 24.0,
    26.0, 28.0, 30.0, 32.0, 34.0, 36.0, 38.0, 40.0 },

  /* Pooled Parameter (Expression: [200:200:4400])
   * Referenced by:
   *   '<S10>/ID_MAP_LIMIT'
   *   '<S10>/IQ_MAP_LIMIT'
   */
  { 200.0, 400.0, 600.0, 800.0, 1000.0, 1200.0, 1400.0, 1600.0, 1800.0, 2000.0,
    2200.0, 2400.0, 2600.0, 2800.0, 3000.0, 3200.0, 3400.0, 3600.0, 3800.0,
    4000.0, 4200.0, 4400.0 },

  /* Pooled Parameter (Expression: [225,360,405])
   * Referenced by:
   *   '<S10>/ID_MAP_LIMIT'
   *   '<S10>/IQ_MAP_LIMIT'
   */
  { 225.0, 360.0, 405.0 },

  /* Expression: [0 0 100 100 0   0]
   * Referenced by: '<S7>/Output_Rate_Limit'
   */
  { 0.0, 0.0, 100.0, 100.0, 0.0, 0.0 },

  /* Expression: [0 4 40  170 230 240]
   * Referenced by: '<S7>/Output_Rate_Limit'
   */
  { 0.0, 4.0, 40.0, 170.0, 230.0, 240.0 },

  /* Pooled Parameter (Expression: [0 300 320])
   * Referenced by:
   *   '<S12>/ramp_down_curve2'
   *   '<S12>/ramp_down_curve3'
   */
  { 0.0, 300.0, 320.0 },

  /* Expression: [135 50 50]
   * Referenced by: '<S12>/ramp_down_curve2'
   */
  { 135.0, 50.0, 50.0 },

  /* Expression: [0 100 200 250]
   * Referenced by: '<S8>/hold_time_curve'
   */
  { 0.0, 100.0, 200.0, 250.0 },

  /* Pooled Parameter (Expression: )
   * Referenced by:
   *   '<S10>/ID_MAP_LIMIT'
   *   '<S10>/IQ_MAP_LIMIT'
   */
  { 1U, 21U, 462U },

  /* Computed Parameter: IQ_MAP_LIMIT_tableData
   * Referenced by: '<S10>/IQ_MAP_LIMIT'
   */
  { 0, 27, 52, 77, 102, 127, 152, 178, 204, 231, 257, 284, 312, 340, 340, 340,
    340, 340, 340, 340, 340, 0, 27, 52, 77, 102, 127, 152, 178, 204, 231, 257,
    284, 312, 312, 312, 312, 312, 312, 312, 312, 312, 0, 27, 52, 77, 102, 127,
    153, 178, 205, 231, 257, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 0,
    27, 52, 77, 102, 127, 153, 179, 205, 232, 258, 258, 258, 258, 258, 258, 258,
    258, 258, 258, 258, 0, 27, 52, 77, 102, 128, 153, 179, 205, 231, 231, 231,
    231, 231, 231, 231, 231, 231, 231, 231, 231, 0, 28, 52, 77, 103, 128, 153,
    179, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 0, 28,
    53, 78, 103, 128, 153, 179, 179, 179, 179, 179, 179, 179, 179, 179, 179, 179,
    179, 179, 179, 0, 28, 53, 78, 103, 128, 154, 154, 154, 154, 154, 154, 154,
    154, 154, 154, 154, 154, 154, 154, 154, 0, 28, 53, 78, 103, 128, 153, 153,
    153, 153, 153, 153, 153, 153, 153, 153, 153, 153, 153, 153, 153, 0, 28, 53,
    78, 103, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 0, 28, 53, 78, 102, 126, 126, 126, 126, 126, 126, 126, 126,
    126, 126, 126, 126, 126, 126, 126, 126, 0, 28, 53, 77, 101, 101, 101, 101,
    101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 0, 28, 52,
    76, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 0, 27, 52, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 0, 27, 51, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,
    75, 75, 75, 75, 75, 75, 75, 0, 27, 51, 75, 75, 75, 75, 75, 75, 75, 75, 75,
    75, 75, 75, 75, 75, 75, 75, 75, 75, 0, 27, 51, 51, 51, 51, 51, 51, 51, 51,
    51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 0, 27, 51, 51, 51, 51, 51, 51,
    51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 0, 27, 51, 51, 51, 51,
    51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 0, 27, 51, 51,
    51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 0, 27,
    51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,
    0, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
    27, 27, 0, 27, 52, 77, 102, 127, 152, 178, 204, 231, 257, 284, 312, 340, 376,
    398, 429, 429, 429, 429, 429, 0, 27, 52, 77, 102, 127, 152, 178, 204, 231,
    257, 284, 312, 340, 376, 399, 399, 399, 399, 399, 399, 0, 27, 52, 77, 102,
    127, 153, 178, 205, 231, 257, 284, 312, 340, 376, 376, 376, 376, 376, 376,
    376, 0, 27, 52, 77, 102, 127, 153, 179, 205, 232, 258, 285, 312, 341, 341,
    341, 341, 341, 341, 341, 341, 0, 27, 52, 77, 102, 128, 153, 179, 205, 231,
    258, 285, 313, 313, 313, 313, 313, 313, 313, 313, 313, 0, 28, 52, 77, 103,
    128, 153, 179, 205, 231, 258, 285, 285, 285, 285, 285, 285, 285, 285, 285,
    285, 0, 28, 53, 78, 103, 128, 153, 179, 205, 231, 258, 285, 285, 285, 285,
    285, 285, 285, 285, 285, 285, 0, 28, 53, 78, 103, 128, 154, 179, 206, 232,
    258, 258, 258, 258, 258, 258, 258, 258, 258, 258, 258, 0, 28, 53, 78, 103,
    128, 154, 180, 206, 232, 232, 232, 232, 232, 232, 232, 232, 232, 232, 232,
    232, 0, 28, 53, 78, 103, 128, 154, 180, 206, 232, 232, 232, 232, 232, 232,
    232, 232, 232, 232, 232, 232, 0, 28, 53, 78, 103, 129, 154, 180, 206, 206,
    206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 0, 28, 53, 78, 104,
    129, 154, 180, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207,
    207, 0, 29, 54, 79, 104, 129, 154, 180, 180, 180, 180, 180, 180, 180, 180,
    180, 180, 180, 180, 180, 180, 0, 29, 54, 79, 104, 129, 155, 181, 181, 181,
    181, 181, 181, 181, 181, 181, 181, 181, 181, 181, 181, 0, 29, 54, 79, 104,
    129, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
    155, 0, 29, 54, 79, 104, 129, 153, 153, 153, 153, 153, 153, 153, 153, 153,
    153, 153, 153, 153, 153, 153, 0, 29, 54, 79, 104, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 0, 29, 54, 79, 103,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 0, 29, 54, 78, 103, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 0, 29, 53, 78, 102, 102, 102, 102, 102, 102,
    102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 0, 29, 53, 78, 102,
    102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102,
    102, 0, 29, 53, 77, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101, 101,
    101, 101, 101, 101, 101, 101, 0, 27, 52, 77, 102, 127, 152, 178, 204, 231,
    257, 284, 312, 340, 376, 398, 429, 461, 461, 461, 461, 0, 27, 52, 77, 102,
    127, 152, 178, 204, 231, 257, 284, 312, 340, 376, 399, 430, 430, 430, 430,
    430, 0, 27, 52, 77, 102, 127, 153, 178, 205, 231, 257, 284, 312, 340, 376,
    399, 399, 399, 399, 399, 399, 0, 27, 52, 77, 102, 127, 153, 179, 205, 232,
    258, 285, 312, 341, 370, 370, 370, 370, 370, 370, 370, 0, 27, 52, 77, 102,
    128, 153, 179, 205, 231, 258, 285, 313, 341, 341, 341, 341, 341, 341, 341,
    341, 0, 28, 52, 77, 103, 128, 153, 179, 205, 231, 258, 285, 313, 313, 313,
    313, 313, 313, 313, 313, 313, 0, 28, 53, 78, 103, 128, 153, 179, 205, 231,
    258, 285, 313, 313, 313, 313, 313, 313, 313, 313, 313, 0, 28, 53, 78, 103,
    128, 154, 179, 206, 232, 258, 286, 286, 286, 286, 286, 286, 286, 286, 286,
    286, 0, 28, 53, 78, 103, 128, 154, 180, 206, 232, 259, 259, 259, 259, 259,
    259, 259, 259, 259, 259, 259, 0, 28, 53, 78, 103, 128, 154, 180, 206, 232,
    259, 259, 259, 259, 259, 259, 259, 259, 259, 259, 259, 0, 28, 53, 78, 103,
    129, 154, 180, 206, 232, 232, 232, 232, 232, 232, 232, 232, 232, 232, 232,
    232, 0, 28, 53, 78, 104, 129, 154, 180, 207, 207, 207, 207, 207, 207, 207,
    207, 207, 207, 207, 207, 207, 0, 29, 54, 79, 104, 129, 154, 180, 207, 207,
    207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 0, 29, 54, 79, 104,
    129, 155, 181, 181, 181, 181, 181, 181, 181, 181, 181, 181, 181, 181, 181,
    181, 0, 29, 54, 79, 104, 129, 155, 181, 181, 181, 181, 181, 181, 181, 181,
    181, 181, 181, 181, 181, 181, 0, 29, 54, 79, 104, 130, 155, 181, 181, 181,
    181, 181, 181, 181, 181, 181, 181, 181, 181, 181, 181, 0, 29, 54, 79, 104,
    130, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155,
    155, 0, 29, 54, 79, 105, 130, 154, 154, 154, 154, 154, 154, 154, 154, 154,
    154, 154, 154, 154, 154, 154, 0, 30, 54, 80, 104, 129, 129, 129, 129, 129,
    129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 0, 30, 55, 79, 104,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 0, 30, 54, 79, 103, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 0, 29, 54, 78, 103, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 },

  /* Computed Parameter: ID_MAP_LIMIT_tableData
   * Referenced by: '<S10>/ID_MAP_LIMIT'
   */
  { 0, 0, -1, -2, -4, -6, -11, -12, -13, -15, -28, -29, -36, -44, -44, -44, -44,
    -44, -44, -44, -44, 0, 0, -2, -2, -4, -6, -11, -12, -14, -15, -28, -29, -36,
    -36, -36, -36, -36, -36, -36, -36, -36, 0, -1, -2, -3, -4, -7, -11, -12, -14,
    -16, -29, -29, -29, -29, -29, -29, -29, -29, -29, -29, -29, 0, -1, -2, -3,
    -4, -7, -11, -12, -14, -16, -29, -29, -29, -29, -29, -29, -29, -29, -29, -29,
    -29, 0, -1, -2, -3, -4, -7, -11, -13, -14, -23, -23, -23, -23, -23, -23, -23,
    -23, -23, -23, -23, -23, 0, -1, -2, -3, -4, -7, -11, -13, -14, -14, -14, -14,
    -14, -14, -14, -14, -14, -14, -14, -14, -14, 0, -1, -2, -3, -5, -7, -12, -13,
    -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, 0, -1, -2,
    -3, -5, -7, -12, -12, -12, -12, -12, -12, -12, -12, -12, -12, -12, -12, -12,
    -12, -12, 0, -1, -2, -3, -5, -8, -23, -23, -23, -23, -23, -23, -23, -23, -23,
    -23, -23, -23, -23, -23, -23, 0, -1, -2, -3, -13, -36, -36, -36, -36, -36,
    -36, -36, -36, -36, -36, -36, -36, -36, -36, -36, -36, 0, -1, -8, -24, -43,
    -69, -69, -69, -69, -69, -69, -69, -69, -69, -69, -69, -69, -69, -69, -69,
    -69, -7, -19, -32, -48, -69, -69, -69, -69, -69, -69, -69, -69, -69, -69,
    -69, -69, -69, -69, -69, -69, -69, -26, -39, -52, -69, -92, -92, -92, -92,
    -92, -92, -92, -92, -92, -92, -92, -92, -92, -92, -92, -92, -92, -42, -56,
    -69, -87, -87, -87, -87, -87, -87, -87, -87, -87, -87, -87, -87, -87, -87,
    -87, -87, -87, -87, -56, -70, -84, -103, -103, -103, -103, -103, -103, -103,
    -103, -103, -103, -103, -103, -103, -103, -103, -103, -103, -103, -69, -83,
    -97, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118,
    -118, -118, -118, -118, -118, -118, -80, -94, -109, -109, -109, -109, -109,
    -109, -109, -109, -109, -109, -109, -109, -109, -109, -109, -109, -109, -109,
    -109, -89, -104, -120, -120, -120, -120, -120, -120, -120, -120, -120, -120,
    -120, -120, -120, -120, -120, -120, -120, -120, -120, -98, -114, -129, -129,
    -129, -129, -129, -129, -129, -129, -129, -129, -129, -129, -129, -129, -129,
    -129, -129, -129, -129, -105, -122, -138, -138, -138, -138, -138, -138, -138,
    -138, -138, -138, -138, -138, -138, -138, -138, -138, -138, -138, -138, -112,
    -129, -146, -146, -146, -146, -146, -146, -146, -146, -146, -146, -146, -146,
    -146, -146, -146, -146, -146, -146, -146, -118, -136, -136, -136, -136, -136,
    -136, -136, -136, -136, -136, -136, -136, -136, -136, -136, -136, -136, -136,
    -136, -136, 0, 0, -1, -2, -4, -6, -11, -12, -13, -15, -28, -29, -36, -44,
    -54, -79, -94, -94, -94, -94, -94, 0, 0, -2, -2, -4, -6, -11, -12, -14, -15,
    -28, -29, -36, -44, -54, -79, -79, -79, -79, -79, -79, 0, -1, -2, -3, -4, -7,
    -11, -12, -14, -16, -29, -29, -36, -44, -54, -54, -54, -54, -54, -54, -54, 0,
    -1, -2, -3, -4, -7, -11, -12, -14, -16, -29, -66, -36, -45, -45, -45, -45,
    -45, -45, -45, -45, 0, -1, -2, -3, -4, -7, -11, -13, -14, -23, -29, -66, -36,
    -36, -36, -36, -36, -36, -36, -36, -36, 0, -1, -2, -3, -4, -7, -11, -13, -14,
    -23, -29, -67, -67, -67, -67, -67, -67, -67, -67, -67, -67, 0, -1, -2, -3,
    -5, -7, -12, -13, -14, -23, -30, -67, -67, -67, -67, -67, -67, -67, -67, -67,
    -67, 0, -1, -2, -3, -5, -7, -12, -13, -14, -23, -30, -30, -30, -30, -30, -30,
    -30, -30, -30, -30, -30, 0, -1, -2, -3, -5, -8, -12, -13, -14, -24, -24, -24,
    -24, -24, -24, -24, -24, -24, -24, -24, -24, 0, -1, -2, -3, -5, -8, -12, -13,
    -14, -24, -24, -24, -24, -24, -24, -24, -24, -24, -24, -24, -24, 0, -1, -2,
    -3, -5, -8, -12, -13, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15,
    -15, -15, 0, -1, -2, -3, -5, -8, -12, -13, -15, -15, -15, -15, -15, -15, -15,
    -15, -15, -15, -15, -15, -15, 0, -1, -2, -3, -5, -8, -12, -13, -13, -13, -13,
    -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, 0, -1, -2, -3, -5, -8, -12,
    -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, 0, -1,
    -2, -3, -5, -9, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15,
    -15, -15, -15, 0, -1, -6, -3, -5, -17, -40, -40, -40, -40, -40, -40, -40,
    -40, -40, -40, -40, -40, -40, -40, -40, 0, -1, -6, -4, -19, -38, -38, -38,
    -38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38, 0, -1, -9,
    -21, -36, -57, -57, -57, -57, -57, -57, -57, -57, -57, -57, -57, -57, -57,
    -57, -57, -57, -4, -14, -23, -36, -52, -73, -73, -73, -73, -73, -73, -73,
    -73, -73, -73, -73, -73, -73, -73, -73, -73, -17, -27, -36, -49, -67, -67,
    -67, -67, -67, -67, -67, -67, -67, -67, -67, -67, -67, -67, -67, -67, -67,
    -28, -38, -48, -62, -80, -80, -80, -80, -80, -80, -80, -80, -80, -80, -80,
    -80, -80, -80, -80, -80, -80, -39, -49, -59, -73, -92, -92, -92, -92, -92,
    -92, -92, -92, -92, -92, -92, -92, -92, -92, -92, -92, -92, 0, 0, -1, -2, -4,
    -6, -11, -12, -13, -15, -28, -29, -36, -44, -54, -79, -94, -112, -112, -112,
    -112, 0, 0, -2, -2, -4, -6, -11, -12, -14, -15, -28, -29, -36, -44, -54, -79,
    -95, -95, -95, -95, -95, 0, -1, -2, -3, -4, -7, -11, -12, -14, -16, -29, -29,
    -36, -44, -54, -79, -79, -79, -79, -79, -79, 0, -1, -2, -3, -4, -7, -11, -12,
    -14, -16, -29, -66, -36, -45, -54, -54, -54, -54, -54, -54, -54, 0, -1, -2,
    -3, -4, -7, -11, -13, -14, -23, -29, -66, -36, -45, -45, -45, -45, -45, -45,
    -45, -45, 0, -1, -2, -3, -4, -7, -11, -13, -14, -23, -29, -67, -37, -37, -37,
    -37, -37, -37, -37, -37, -37, 0, -1, -2, -3, -5, -7, -12, -13, -14, -23, -30,
    -67, -37, -37, -37, -37, -37, -37, -37, -37, -37, 0, -1, -2, -3, -5, -7, -12,
    -13, -14, -23, -30, -67, -67, -67, -67, -67, -67, -67, -67, -67, -67, 0, -1,
    -2, -3, -5, -8, -12, -13, -14, -24, -30, -30, -30, -30, -30, -30, -30, -30,
    -30, -30, -30, 0, -1, -2, -3, -5, -8, -12, -13, -14, -24, -30, -30, -30, -30,
    -30, -30, -30, -30, -30, -30, -30, 0, -1, -2, -3, -5, -8, -12, -13, -15, -24,
    -24, -24, -24, -24, -24, -24, -24, -24, -24, -24, -24, 0, -1, -2, -3, -5, -8,
    -12, -13, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, -15, 0,
    -1, -2, -3, -5, -8, -12, -13, -15, -15, -15, -15, -15, -15, -15, -15, -15,
    -15, -15, -15, -15, 0, -1, -2, -3, -5, -8, -12, -13, -13, -13, -13, -13, -13,
    -13, -13, -13, -13, -13, -13, -13, -13, 0, -1, -2, -3, -5, -9, -12, -13, -13,
    -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, 0, -1, -6, -3,
    -5, -9, -12, -14, -14, -14, -14, -14, -14, -14, -14, -14, -14, -14, -14, -14,
    -14, 0, -1, -6, -4, -5, -11, -14, -14, -14, -14, -14, -14, -14, -14, -14,
    -14, -14, -14, -14, -14, -14, 0, -1, -6, -4, -6, -14, -35, -35, -35, -35,
    -35, -35, -35, -35, -35, -35, -35, -35, -35, -35, -35, 0, -1, -6, -4, -15,
    -33, -33, -33, -33, -33, -33, -33, -33, -33, -33, -33, -33, -33, -33, -33,
    -33, 0, -1, -6, -16, -30, -49, -49, -49, -49, -49, -49, -49, -49, -49, -49,
    -49, -49, -49, -49, -49, -49, 0, -9, -17, -29, -45, -65, -65, -65, -65, -65,
    -65, -65, -65, -65, -65, -65, -65, -65, -65, -65, -65, -12, -20, -29, -42,
    -58, -79, -79, -79, -79, -79, -79, -79, -79, -79, -79, -79, -79, -79, -79,
    -79, -79 },

  /* Computed Parameter: hold_time_curve_tableData
   * Referenced by: '<S8>/hold_time_curve'
   */
  { 100U, 50U, 50U, 50U }
};

/* Block signals and states (default storage) */
DW_Driver_Strategy_T Driver_Strategy_DW;
static real_T look1_tu16DdId_binlc(real_T u0, const real_T bp0[], const uint16_T
  table[], uint32_T maxIndex);
static real_T look1_binlx(real_T u0, const real_T bp0[], const real_T table[],
  uint32_T maxIndex);
static uint32_T plook_binc(real_T u, const real_T bp[], uint32_T maxIndex,
  real_T *fraction);
static real_T intrp3d_s16ddl(const uint32_T bpIndex[], const real_T frac[],
  const int16_T table[], const uint32_T stride[]);
static uint32_T binsearch_u32d(real_T u, const real_T bp[], uint32_T startIndex,
  uint32_T maxIndex);
static real_T look1_tu16DdId_binlc(real_T u0, const real_T bp0[], const uint16_T
  table[], uint32_T maxIndex)
{
  real_T frac;
  uint32_T iRght;
  uint32_T iLeft;
  uint32_T bpIdx;

  /* Column-major Lookup 1-D
     Search method: 'binary'
     Use previous index: 'off'
     Interpolation method: 'Linear point-slope'
     Extrapolation method: 'Clip'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Clip'
     Use previous index: 'off'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u0 <= bp0[0U]) {
    iLeft = 0U;
    frac = 0.0;
  } else if (u0 < bp0[maxIndex]) {
    /* Binary Search */
    bpIdx = (maxIndex >> 1ULL);
    iLeft = 0U;
    iRght = maxIndex;
    while ((iRght - iLeft) > 1U) {
      if (u0 < bp0[bpIdx]) {
        iRght = bpIdx;
      } else {
        iLeft = bpIdx;
      }

      bpIdx = ((iRght + iLeft) >> 1ULL);
    }

    frac = (u0 - bp0[iLeft]) / (bp0[iLeft + 1U] - bp0[iLeft]);
  } else {
    iLeft = maxIndex - 1U;
    frac = 1.0;
  }

  /* Column-major Interpolation 1-D
     Interpolation method: 'Linear point-slope'
     Use last breakpoint for index at or above upper limit: 'off'
     Overflow mode: 'wrapping'
   */
  return ((((real_T)table[iLeft + 1U]) - ((real_T)table[iLeft])) * frac) +
    ((real_T)table[iLeft]);
}

static real_T look1_binlx(real_T u0, const real_T bp0[], const real_T table[],
  uint32_T maxIndex)
{
  real_T frac;
  uint32_T iRght;
  uint32_T iLeft;
  uint32_T bpIdx;

  /* Column-major Lookup 1-D
     Search method: 'binary'
     Use previous index: 'off'
     Interpolation method: 'Linear point-slope'
     Extrapolation method: 'Linear'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Linear'
     Use previous index: 'off'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u0 <= bp0[0U]) {
    iLeft = 0U;
    frac = (u0 - bp0[0U]) / (bp0[1U] - bp0[0U]);
  } else if (u0 < bp0[maxIndex]) {
    /* Binary Search */
    bpIdx = (maxIndex >> 1ULL);
    iLeft = 0U;
    iRght = maxIndex;
    while ((iRght - iLeft) > 1U) {
      if (u0 < bp0[bpIdx]) {
        iRght = bpIdx;
      } else {
        iLeft = bpIdx;
      }

      bpIdx = ((iRght + iLeft) >> 1ULL);
    }

    frac = (u0 - bp0[iLeft]) / (bp0[iLeft + 1U] - bp0[iLeft]);
  } else {
    iLeft = maxIndex - 1U;
    frac = (u0 - bp0[maxIndex - 1U]) / (bp0[maxIndex] - bp0[maxIndex - 1U]);
  }

  /* Column-major Interpolation 1-D
     Interpolation method: 'Linear point-slope'
     Use last breakpoint for index at or above upper limit: 'off'
     Overflow mode: 'wrapping'
   */
  return ((table[iLeft + 1U] - table[iLeft]) * frac) + table[iLeft];
}

static uint32_T plook_binc(real_T u, const real_T bp[], uint32_T maxIndex,
  real_T *fraction)
{
  uint32_T bpIndex;

  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Clip'
     Use previous index: 'off'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u <= bp[0U]) {
    bpIndex = 0U;
    *fraction = 0.0;
  } else if (u < bp[maxIndex]) {
    bpIndex = binsearch_u32d(u, bp, maxIndex >> 1ULL, maxIndex);
    *fraction = (u - bp[bpIndex]) / (bp[bpIndex + 1U] - bp[bpIndex]);
  } else {
    bpIndex = maxIndex - 1U;
    *fraction = 1.0;
  }

  return bpIndex;
}

static real_T intrp3d_s16ddl(const uint32_T bpIndex[], const real_T frac[],
  const int16_T table[], const uint32_T stride[])
{
  real_T yL_2d;
  uint32_T offset_2d;
  real_T yL_1d;
  uint32_T offset_0d;

  /* Column-major Interpolation 3-D
     Interpolation method: 'Linear point-slope'
     Use last breakpoint for index at or above upper limit: 'off'
     Overflow mode: 'wrapping'
   */
  offset_2d = ((bpIndex[2U] * stride[2U]) + (bpIndex[1U] * stride[1U])) +
    bpIndex[0U];
  yL_1d = ((((real_T)table[offset_2d + 1U]) - ((real_T)table[offset_2d])) *
           frac[0U]) + ((real_T)table[offset_2d]);
  offset_0d = offset_2d + stride[1U];
  yL_2d = (((((((real_T)table[offset_0d + 1U]) - ((real_T)table[offset_0d])) *
              frac[0U]) + ((real_T)table[offset_0d])) - yL_1d) * frac[1U]) +
    yL_1d;
  offset_2d += stride[2U];
  yL_1d = ((((real_T)table[offset_2d + 1U]) - ((real_T)table[offset_2d])) *
           frac[0U]) + ((real_T)table[offset_2d]);
  offset_0d = offset_2d + stride[1U];
  return ((((((((((real_T)table[offset_0d + 1U]) - ((real_T)table[offset_0d])) *
                frac[0U]) + ((real_T)table[offset_0d])) - yL_1d) * frac[1U]) +
            yL_1d) - yL_2d) * frac[2U]) + yL_2d;
}

static uint32_T binsearch_u32d(real_T u, const real_T bp[], uint32_T startIndex,
  uint32_T maxIndex)
{
  uint32_T bpIndex;
  uint32_T iRght;
  uint32_T bpIdx;

  /* Binary Search */
  bpIdx = startIndex;
  bpIndex = 0U;
  iRght = maxIndex;
  while ((iRght - bpIndex) > 1U) {
    if (u < bp[bpIdx]) {
      iRght = bpIdx;
    } else {
      bpIndex = bpIdx;
    }

    bpIdx = ((iRght + bpIndex) >> 1ULL);
  }

  return bpIndex;
}

/* Model step function */
void Driver_Strategy_step(void)
{
  real_T frac;
  uint32_T bpIndices[3];
  real_T fractions[3];
  uint32_T bpIndices_0[3];
  real_T fractions_0[3];
  real_T rtb_MultiportSwitch_j;
  real_T rtb_Saturation;
  real_T rtb_Saturation_b;
  real_T rtb_Gain3;
  real_T rtb_Switch;
  real_T rtb_Gain1;
  uint32_T tmp;
  real_T mod_id_cmd_0;
  real_T mod_acc_data_read_0;
  uint8_T is_c2_Driver_Strategy;
  real_T Acc_r_init_0;
  real_T acc_out;
  real_T UnitDelay_DSTATE_c;
  real_T Acc_r_0;
  real_T UnitDelay2_DSTATE;
  real_T UnitDelay1_DSTATE;
  real_T UnitDelay_DSTATE;
  uint16_T timer_h;
  real_T temp_max;
  real_T temp_min;
  uint16_T timer;
  real_T acc_delta;
  real_T UnitDelay1_DSTATE_i;
  real_T delta_r_out;
  uint16_T time_counter;
  uint8_T is_c3_Driver_Strategy;
  boolean_T mod_motor_enable_0;
  uint8_T Acc_state_out_0;
  boolean_T enable_pre;
  uint8_T is_c4_Driver_Strategy;
  real_T mod_iq_cmd_0;
  boolean_T guard1 = false;
  is_c4_Driver_Strategy = Driver_Strategy_DW.is_c4_Driver_Strategy;
  enable_pre = Driver_Strategy_DW.enable_pre;
  is_c3_Driver_Strategy = Driver_Strategy_DW.is_c3_Driver_Strategy;
  time_counter = Driver_Strategy_DW.time_counter;
  delta_r_out = Driver_Strategy_DW.delta_r_out;

  /* Update for UnitDelay: '<S8>/Unit Delay1' */
  UnitDelay1_DSTATE_i = Driver_Strategy_DW.UnitDelay1_DSTATE_i;
  acc_delta = Driver_Strategy_DW.acc_delta;
  timer = Driver_Strategy_DW.timer;
  temp_min = Driver_Strategy_DW.temp_min;
  temp_max = Driver_Strategy_DW.temp_max;
  timer_h = Driver_Strategy_DW.timer_h;

  /* UnitDelay: '<S6>/Unit Delay' */
  UnitDelay_DSTATE = Driver_Strategy_DW.UnitDelay_DSTATE;

  /* UnitDelay: '<S6>/Unit Delay1' */
  UnitDelay1_DSTATE = Driver_Strategy_DW.UnitDelay1_DSTATE;

  /* UnitDelay: '<S6>/Unit Delay2' */
  UnitDelay2_DSTATE = Driver_Strategy_DW.UnitDelay2_DSTATE;
  UnitDelay_DSTATE_c = Driver_Strategy_DW.UnitDelay_DSTATE_c;
  acc_out = Driver_Strategy_DW.acc_out;
  Acc_r_init_0 = Acc_r_init;
  is_c2_Driver_Strategy = Driver_Strategy_DW.is_c2_Driver_Strategy;

  /* Update for Inport: '<Root>/Input1' */
  mod_acc_data_read_0 = mod_acc_data_read;

  /* Fcn: '<Root>/Fcn' incorporates:
   *  Fcn: '<Root>/Fcn1'
   *  Inport: '<Root>/Input'
   */
  rtb_Gain1 = (mod_espd / 65535.0) * 0.7958;

  /* Product: '<S6>/Divide' incorporates:
   *  Constant: '<S6>/Constant'
   *  Inport: '<Root>/Input1'
   *  Sum: '<S6>/Add'
   *  UnitDelay: '<S6>/Unit Delay3'
   */
  Acc_r_0 = ((((mod_acc_data_read_0 + UnitDelay_DSTATE) + UnitDelay1_DSTATE) +
              UnitDelay2_DSTATE) + Driver_Strategy_DW.UnitDelay3_DSTATE) / 5.0;

  /* Saturate: '<S8>/Saturation' */
  if (Acc_r_0 > 600.0) {
    rtb_Saturation = 600.0;
  } else if (Acc_r_0 < 175.0) {
    rtb_Saturation = 175.0;
  } else {
    rtb_Saturation = Acc_r_0;
  }

  /* End of Saturate: '<S8>/Saturation' */

  /* Switch: '<S8>/Switch1' incorporates:
   *  Constant: '<S8>/Constant4'
   *  Fcn: '<Root>/Fcn'
   *  Lookup_n-D: '<S12>/ramp_down_curve2'
   *  MultiPortSwitch: '<S12>/Multiport Switch'
   *  RelationalOperator: '<S8>/Relational Operator2'
   *  UnitDelay: '<S8>/Unit Delay2'
   */
  if (rtb_Saturation > Driver_Strategy_DW.UnitDelay2_DSTATE_e) {
    rtb_Switch = 550.0;
  } else {
    rtb_Switch = look1_binlx(rtb_Gain1, Driver_Strategy_ConstP.pooled7,
      Driver_Strategy_ConstP.ramp_down_curve2_tableData, 2U);
  }

  /* End of Switch: '<S8>/Switch1' */

  /* Sum: '<S8>/Add1' incorporates:
   *  Constant: '<S8>/Constant2'
   *  Product: '<S8>/Divide'
   *  Product: '<S8>/Divide1'
   *  Sum: '<S8>/Add'
   */
  UnitDelay_DSTATE_c += (1.0 / rtb_Switch) * (Acc_r_0 - UnitDelay_DSTATE_c);

  /* Switch: '<S8>/Switch' incorporates:
   *  Constant: '<S8>/Constant3'
   *  RelationalOperator: '<S8>/Relational Operator'
   */
  if (rtb_Saturation > 0.0) {
    rtb_Switch = UnitDelay_DSTATE_c;
  } else {
    rtb_Switch = rtb_Saturation;
  }

  /* End of Switch: '<S8>/Switch' */

  /* Chart: '<S8>/Chart1' incorporates:
   *  DataTypeConversion: '<S8>/Data Type Conversion'
   *  Fcn: '<Root>/Fcn'
   *  Lookup_n-D: '<S8>/hold_time_curve'
   */
  if (((uint32_T)Driver_Strategy_DW.is_active_c2_Driver_Strategy) == 0U) {
    Driver_Strategy_DW.is_active_c2_Driver_Strategy = 1U;
    is_c2_Driver_Strategy = Driver_Strategy_IN_Step1;
    timer = 0U;
  } else if (((int32_T)is_c2_Driver_Strategy) == 1) {
    if ((boolean_T)((int32_T)(((int32_T)((acc_delta < 1.0) ? 1 : 0)) & ((int32_T)
           ((((int32_T)timer) > 10) ? 1 : 0))))) {
      acc_out = UnitDelay1_DSTATE_i;
      is_c2_Driver_Strategy = Driver_Strategy_IN_Step2;
    } else {
      tmp = ((uint32_T)timer) + 1U;
      if (tmp > 65535U) {
        tmp = 65535U;
      }

      timer = (uint16_T)tmp;
      acc_out = rtb_Switch;
      acc_delta = UnitDelay1_DSTATE_i - rtb_Switch;
    }
  } else {
    /* case IN_Step2: */
    if ((boolean_T)((int32_T)(((int32_T)((timer > ((uint16_T)
              look1_tu16DdId_binlc(rtb_Gain1,
               Driver_Strategy_ConstP.hold_time_curve_bp01Data,
               Driver_Strategy_ConstP.hold_time_curve_tableData, 3U))) ? 1 : 0))
          | ((int32_T)((acc_delta > 1.0) ? 1 : 0))))) {
      is_c2_Driver_Strategy = Driver_Strategy_IN_Step1;
      timer = 0U;
    } else {
      acc_delta = rtb_Switch - UnitDelay1_DSTATE_i;
      tmp = ((uint32_T)timer) + 1U;
      if (tmp > 65535U) {
        tmp = 65535U;
      }

      timer = (uint16_T)tmp;
    }
  }

  /* End of Chart: '<S8>/Chart1' */

  /* Chart: '<Root>/Acc_selfcheck' */
  if (((uint32_T)Driver_Strategy_DW.is_active_c4_Driver_Strategy) == 0U) {
    Driver_Strategy_DW.is_active_c4_Driver_Strategy = 1U;
    Acc_r_init_0 = 300.0;
    temp_max = 100.0;
    temp_min = 300.0;
    enable_pre = false;
    timer_h = 0U;
    is_c4_Driver_Strategy = Driver_Strategy_IN_Step1;
    Acc_state_out_0 = 1U;
  } else {
    guard1 = false;
    switch (is_c4_Driver_Strategy) {
     case Driver_Strategy_IN_Step1:
      Acc_state_out_0 = 1U;
      if (((int32_T)timer_h) >= 500) {
        is_c4_Driver_Strategy = Driver_Strategy_IN_Step2;
        Acc_state_out_0 = 2U;
        Driver_Strategy_DW.temp_delta = temp_max - temp_min;
      } else if ((boolean_T)((int32_T)(((int32_T)((Acc_r_0 < temp_min) ? 1 : 0))
        & ((int32_T)((((int32_T)timer_h) >= 250) ? 1 : 0))))) {
        temp_min = Acc_r_0;
        tmp = ((uint32_T)timer_h) + 1U;
        if (tmp > 65535U) {
          tmp = 65535U;
        }

        timer_h = (uint16_T)tmp;
        guard1 = true;
      } else if ((boolean_T)((int32_T)(((int32_T)((Acc_r_0 > temp_max) ? 1 : 0))
        & ((int32_T)((((int32_T)timer_h) >= 250) ? 1 : 0))))) {
        temp_max = Acc_r_0;
        tmp = ((uint32_T)timer_h) + 1U;
        if (tmp > 65535U) {
          tmp = 65535U;
        }

        timer_h = (uint16_T)tmp;
        guard1 = true;
      } else {
        tmp = ((uint32_T)timer_h) + 1U;
        if (tmp > 65535U) {
          tmp = 65535U;
        }

        timer_h = (uint16_T)tmp;
      }
      break;

     case Driver_Strategy_IN_Step2:
      if (Driver_Strategy_DW.temp_delta > 20.0) {
        is_c4_Driver_Strategy = Driver_Strategy_IN_Step3;
        Acc_state_out_0 = 3U;
        enable_pre = false;
      } else {
        is_c4_Driver_Strategy = Driver_Strategy_IN_Step4;
        Acc_state_out_0 = 4U;
        Acc_r_init_0 = temp_min;
        enable_pre = true;
      }
      break;

     case Driver_Strategy_IN_Step3:
      Acc_state_out_0 = 3U;
      break;

     default:
      /* case IN_Step4: */
      Acc_state_out_0 = 4U;
      break;
    }

    if (guard1) {
      is_c4_Driver_Strategy = Driver_Strategy_IN_Step1;
      Acc_state_out_0 = 1U;
    }
  }

  /* End of Chart: '<Root>/Acc_selfcheck' */

  /* MultiPortSwitch: '<S1>/Multiport Switch' incorporates:
   *  Constant: '<S1>/Constant1'
   *  Constant: '<S1>/Constant2'
   *  Constant: '<S1>/Constant3'
   *  Constant: '<S1>/Constant4'
   *  Constant: '<S1>/Constant5'
   *  Constant: '<S1>/Constant6'
   *  Inport: '<Root>/Input2'
   */
  switch (mod_gear) {
   case 0:
    mod_id_cmd_0 = 0.001;
    break;

   case 1:
    mod_id_cmd_0 = 0.2;
    break;

   case 2:
    mod_id_cmd_0 = 0.4;
    break;

   case 3:
    mod_id_cmd_0 = 0.6;
    break;

   case 4:
    mod_id_cmd_0 = 0.8;
    break;

   default:
    mod_id_cmd_0 = 1.0;
    break;
  }

  /* End of MultiPortSwitch: '<S1>/Multiport Switch' */

  /* Logic: '<S4>/Logical Operator1' incorporates:
   *  Constant: '<S4>/Constant3'
   *  Inport: '<Root>/Input4'
   *  Inport: '<Root>/Input5'
   *  RelationalOperator: '<S4>/Relational Operator'
   *  RelationalOperator: '<S4>/Relational Operator1'
   */
  mod_motor_enable_0 = ((boolean_T)((int32_T)(((int32_T)((((int32_T)
    mod_direction) == 0) ? 1 : 0)) & ((int32_T)((mod_fault <= 1U) ? 1 : 0))))) &
    enable_pre;

  /* Chart: '<Root>/Enable_delay' incorporates:
   *  Product: '<S1>/Product1'
   */
  if (((uint32_T)Driver_Strategy_DW.is_active_c3_Driver_Strategy) == 0U) {
    Driver_Strategy_DW.is_active_c3_Driver_Strategy = 1U;
    is_c3_Driver_Strategy = Driver_Strategy_IN_Enable_State;
    time_counter = 0U;
    delta_r_out = 0.0;
  } else {
    switch (is_c3_Driver_Strategy) {
     case Driver_Strategy_IN_Enable_State:
      if (mod_motor_enable_0) {
        is_c3_Driver_Strategy = Driver_Stra_IN_Timer_Accumulate;
      }
      break;

     case Driver_Strategy_IN_Output_State:
      if ((boolean_T)((int32_T)((mod_motor_enable_0 ? 1 : 0) ^ 1))) {
        is_c3_Driver_Strategy = Driver_Strategy_IN_Enable_State;
        time_counter = 0U;
        delta_r_out = 0.0;
      } else {
        /* Fcn: '<S1>/Fcn' incorporates:
         *  Constant: '<S1>/Constant8'
         *  Sum: '<S1>/Add'
         */
        rtb_Saturation_b = (((acc_out - Acc_r_init_0) - 20.0) * 0.214425) *
          0.070422;

        /* Saturate: '<S1>/Saturation' */
        if (rtb_Saturation_b > 56.8) {
          rtb_Saturation_b = 56.8;
        } else {
          if (rtb_Saturation_b < 0.0) {
            rtb_Saturation_b = 0.0;
          }
        }

        /* End of Saturate: '<S1>/Saturation' */
        delta_r_out = mod_id_cmd_0 * rtb_Saturation_b;
      }
      break;

     default:
      /* case IN_Timer_Accumulate: */
      if (((int32_T)time_counter) >= 500) {
        is_c3_Driver_Strategy = Driver_Strategy_IN_Output_State;
      } else {
        tmp = ((uint32_T)time_counter) + 1U;
        if (tmp > 65535U) {
          tmp = 65535U;
        }

        time_counter = (uint16_T)tmp;
      }
      break;
    }
  }

  /* End of Chart: '<Root>/Enable_delay' */

  /* Product: '<Root>/Product' incorporates:
   *  Fcn: '<Root>/Fcn'
   *  Gain: '<S7>/Gain2'
   *  Lookup_n-D: '<S7>/Output_Rate_Limit'
   *  MultiPortSwitch: '<S7>/Multiport Switch'
   */
  delta_r_in = (0.01 * look1_binlx(rtb_Gain1,
    Driver_Strategy_ConstP.Output_Rate_Limit_bp01Data,
    Driver_Strategy_ConstP.Output_Rate_Limit_tableData, 5U)) * delta_r_out;

  /* Gain: '<S10>/Gain2' */
  rtb_Saturation_b = 10.0 * delta_r_in;

  /* Saturate: '<S10>/Saturation' */
  if (rtb_Saturation_b > 50.0) {
    rtb_Saturation_b = 50.0;
  } else {
    if (rtb_Saturation_b < 0.0) {
      rtb_Saturation_b = 0.0;
    }
  }

  /* End of Saturate: '<S10>/Saturation' */

  /* MultiPortSwitch: '<S9>/Multiport Switch' incorporates:
   *  Fcn: '<S9>/Fcn2'
   *  Inport: '<Root>/Input3'
   */
  rtb_MultiportSwitch_j = mod_bus_volt_read * 0.07258;

  /* Gain: '<S10>/Gain3' */
  rtb_Gain3 = 10.0 * rtb_MultiportSwitch_j;

  /* Lookup_n-D: '<S10>/ID_MAP_LIMIT' */
  bpIndices[0U] = plook_binc(rtb_Saturation_b, Driver_Strategy_ConstP.pooled3,
    20U, &frac);
  fractions[0U] = frac;
  bpIndices[1U] = plook_binc(rtb_Gain1, Driver_Strategy_ConstP.pooled4, 21U,
    &frac);
  fractions[1U] = frac;
  bpIndices[2U] = plook_binc(rtb_Gain3, Driver_Strategy_ConstP.pooled5, 2U,
    &frac);
  fractions[2U] = frac;
  mod_id_cmd_0 = intrp3d_s16ddl(bpIndices, fractions,
    Driver_Strategy_ConstP.ID_MAP_LIMIT_tableData,
    Driver_Strategy_ConstP.pooled11);

  /* Gain: '<S10>/Gain' */
  mod_id_cmd_0 *= 0.1;

  /* Switch: '<S3>/Switch2' incorporates:
   *  Constant: '<S3>/bus_volt_min'
   *  RelationalOperator: '<S3>/Relational Operator3'
   */
  if (rtb_MultiportSwitch_j >= 20.0) {
    /* Lookup_n-D: '<S10>/IQ_MAP_LIMIT' */
    bpIndices_0[0U] = plook_binc(rtb_Saturation_b,
      Driver_Strategy_ConstP.pooled3, 20U, &frac);
    fractions_0[0U] = frac;
    bpIndices_0[1U] = plook_binc(rtb_Gain1, Driver_Strategy_ConstP.pooled4, 21U,
      &frac);
    fractions_0[1U] = frac;
    bpIndices_0[2U] = plook_binc(rtb_Gain3, Driver_Strategy_ConstP.pooled5, 2U,
      &frac);
    fractions_0[2U] = frac;
    rtb_Gain1 = intrp3d_s16ddl(bpIndices_0, fractions_0,
      Driver_Strategy_ConstP.IQ_MAP_LIMIT_tableData,
      Driver_Strategy_ConstP.pooled11);

    /* Saturate: '<Root>/Saturation2' incorporates:
     *  Gain: '<S10>/Gain1'
     */
    mod_iq_cmd_0 = 0.1 * rtb_Gain1;
  } else {
    /* Saturate: '<Root>/Saturation2' incorporates:
     *  Constant: '<S3>/Constant6'
     */
    mod_iq_cmd_0 = 0.0;
  }

  /* RelationalOperator: '<Root>/Relational Operator' incorporates:
   *  Constant: '<Root>/Constant1'
   */
  mod_init_false = (((int32_T)Acc_state_out_0) == 3);

  /* Update for UnitDelay: '<S6>/Unit Delay3' */
  Driver_Strategy_DW.UnitDelay3_DSTATE = UnitDelay2_DSTATE;

  /* Update for UnitDelay: '<S8>/Unit Delay2' */
  Driver_Strategy_DW.UnitDelay2_DSTATE_e = rtb_Saturation;

  /* Saturate: '<Root>/Saturation1' */
  if (mod_id_cmd_0 < -1.0) {
    mod_id_cmd_0 = -1.0;
  }

  /* End of Saturate: '<Root>/Saturation1' */
  UnitDelay2_DSTATE = UnitDelay1_DSTATE;
  UnitDelay1_DSTATE = UnitDelay_DSTATE;

  /* Update for Inport: '<Root>/Input1' */
  UnitDelay_DSTATE = mod_acc_data_read_0;

  /* Update for UnitDelay: '<S8>/Unit Delay1' */
  UnitDelay1_DSTATE_i = rtb_Switch;

  /* Product: '<S1>/Product1' */
  mod_id_cmd = mod_id_cmd_0;
  Driver_Strategy_DW.is_c2_Driver_Strategy = is_c2_Driver_Strategy;
  Acc_r_init = Acc_r_init_0;
  Driver_Strategy_DW.acc_out = acc_out;
  Driver_Strategy_DW.UnitDelay_DSTATE_c = UnitDelay_DSTATE_c;
  Acc_r = Acc_r_0;

  /* UnitDelay: '<S6>/Unit Delay2' */
  Driver_Strategy_DW.UnitDelay2_DSTATE = UnitDelay2_DSTATE;

  /* UnitDelay: '<S6>/Unit Delay1' */
  Driver_Strategy_DW.UnitDelay1_DSTATE = UnitDelay1_DSTATE;

  /* UnitDelay: '<S6>/Unit Delay' */
  Driver_Strategy_DW.UnitDelay_DSTATE = UnitDelay_DSTATE;
  Driver_Strategy_DW.timer_h = timer_h;
  Driver_Strategy_DW.temp_max = temp_max;
  Driver_Strategy_DW.temp_min = temp_min;
  Driver_Strategy_DW.timer = timer;
  Driver_Strategy_DW.acc_delta = acc_delta;

  /* Update for UnitDelay: '<S8>/Unit Delay1' */
  Driver_Strategy_DW.UnitDelay1_DSTATE_i = UnitDelay1_DSTATE_i;
  Driver_Strategy_DW.delta_r_out = delta_r_out;
  Driver_Strategy_DW.time_counter = time_counter;
  Driver_Strategy_DW.is_c3_Driver_Strategy = is_c3_Driver_Strategy;
  mod_motor_enable = mod_motor_enable_0;
  Acc_state_out = Acc_state_out_0;
  Driver_Strategy_DW.enable_pre = enable_pre;
  Driver_Strategy_DW.is_c4_Driver_Strategy = is_c4_Driver_Strategy;

  /* Switch: '<S3>/Switch2' incorporates:
   *  Saturate: '<Root>/Saturation2'
   */
  mod_iq_cmd = mod_iq_cmd_0;
}

/* Model initialize function */
void Driver_Strategy_initialize(void)
{
  /* (no initialization code required) */
}

/* Model terminate function */
void Driver_Strategy_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
