/*
 * File: Driver_Strategy.h
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

#ifndef RTW_HEADER_Driver_Strategy_h_
#define RTW_HEADER_Driver_Strategy_h_
#ifndef Driver_Strategy_COMMON_INCLUDES_
# define Driver_Strategy_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* Driver_Strategy_COMMON_INCLUDES_ */

/* Model Code Variants */

/* Macros for accessing real-time model data structure */

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
  real_T m_bpLambda[3];                /* '<S10>/ID_MAP_LIMIT' */
  real_T m_bpLambda_b[3];              /* '<S10>/IQ_MAP_LIMIT' */
  real_T acc_out;                      /* '<S8>/Chart1' */
  real_T delta_r_out;                  /* '<Root>/Enable_delay' */
  real_T UnitDelay_DSTATE;             /* '<S6>/Unit Delay' */
  real_T UnitDelay1_DSTATE;            /* '<S6>/Unit Delay1' */
  real_T UnitDelay2_DSTATE;            /* '<S6>/Unit Delay2' */
  real_T UnitDelay3_DSTATE;            /* '<S6>/Unit Delay3' */
  real_T UnitDelay_DSTATE_c;           /* '<S8>/Unit Delay' */
  real_T UnitDelay2_DSTATE_e;          /* '<S8>/Unit Delay2' */
  real_T UnitDelay1_DSTATE_i;          /* '<S8>/Unit Delay1' */
  real_T acc_delta;                    /* '<S8>/Chart1' */
  real_T temp_max;                     /* '<Root>/Acc_selfcheck' */
  real_T temp_min;                     /* '<Root>/Acc_selfcheck' */
  real_T temp_delta;                   /* '<Root>/Acc_selfcheck' */
  uint32_T m_bpIndex[3];               /* '<S10>/ID_MAP_LIMIT' */
  uint32_T m_bpIndex_n[3];             /* '<S10>/IQ_MAP_LIMIT' */
  uint16_T timer;                      /* '<S8>/Chart1' */
  uint16_T time_counter;               /* '<Root>/Enable_delay' */
  uint16_T timer_h;                    /* '<Root>/Acc_selfcheck' */
  uint8_T is_active_c2_Driver_Strategy;/* '<S8>/Chart1' */
  uint8_T is_c2_Driver_Strategy;       /* '<S8>/Chart1' */
  uint8_T is_active_c3_Driver_Strategy;/* '<Root>/Enable_delay' */
  uint8_T is_c3_Driver_Strategy;       /* '<Root>/Enable_delay' */
  uint8_T is_active_c4_Driver_Strategy;/* '<Root>/Acc_selfcheck' */
  uint8_T is_c4_Driver_Strategy;       /* '<Root>/Acc_selfcheck' */
  boolean_T enable_pre;                /* '<Root>/Acc_selfcheck' */
} DW_Driver_Strategy_T;

/* Constant parameters (default storage) */
typedef struct {
  /* Pooled Parameter (Expression: [0:2:40])
   * Referenced by:
   *   '<S10>/ID_MAP_LIMIT'
   *   '<S10>/IQ_MAP_LIMIT'
   */
  real_T pooled3[21];

  /* Pooled Parameter (Expression: [200:200:4400])
   * Referenced by:
   *   '<S10>/ID_MAP_LIMIT'
   *   '<S10>/IQ_MAP_LIMIT'
   */
  real_T pooled4[22];

  /* Pooled Parameter (Expression: [225,360,405])
   * Referenced by:
   *   '<S10>/ID_MAP_LIMIT'
   *   '<S10>/IQ_MAP_LIMIT'
   */
  real_T pooled5[3];

  /* Expression: [0 0 100 100 0   0]
   * Referenced by: '<S7>/Output_Rate_Limit'
   */
  real_T Output_Rate_Limit_tableData[6];

  /* Expression: [0 4 40  170 230 240]
   * Referenced by: '<S7>/Output_Rate_Limit'
   */
  real_T Output_Rate_Limit_bp01Data[6];

  /* Pooled Parameter (Expression: [0 300 320])
   * Referenced by:
   *   '<S12>/ramp_down_curve2'
   *   '<S12>/ramp_down_curve3'
   */
  real_T pooled7[3];

  /* Expression: [135 50 50]
   * Referenced by: '<S12>/ramp_down_curve2'
   */
  real_T ramp_down_curve2_tableData[3];

  /* Expression: [0 100 200 250]
   * Referenced by: '<S8>/hold_time_curve'
   */
  real_T hold_time_curve_bp01Data[4];

  /* Pooled Parameter (Expression: )
   * Referenced by:
   *   '<S10>/ID_MAP_LIMIT'
   *   '<S10>/IQ_MAP_LIMIT'
   */
  uint32_T pooled11[3];

  /* Computed Parameter: IQ_MAP_LIMIT_tableData
   * Referenced by: '<S10>/IQ_MAP_LIMIT'
   */
  int16_T IQ_MAP_LIMIT_tableData[1386];

  /* Computed Parameter: ID_MAP_LIMIT_tableData
   * Referenced by: '<S10>/ID_MAP_LIMIT'
   */
  int16_T ID_MAP_LIMIT_tableData[1386];

  /* Computed Parameter: hold_time_curve_tableData
   * Referenced by: '<S8>/hold_time_curve'
   */
  uint16_T hold_time_curve_tableData[4];
} ConstP_Driver_Strategy_T;

/* Block signals and states (default storage) */
extern DW_Driver_Strategy_T Driver_Strategy_DW;

/* Constant parameters (default storage) */
extern const ConstP_Driver_Strategy_T Driver_Strategy_ConstP;

/*
 * Exported Global Signals
 *
 * Note: Exported global signals are block signals with an exported global
 * storage class designation.  Code generation will declare the memory for
 * these signals and export their symbols.
 *
 */
extern real_T mod_espd;                /* '<Root>/Input' */
extern uint8_T mod_gear;               /* '<Root>/Input2' */
extern real_T mod_acc_data_read;       /* '<Root>/Input1' */
extern real_T mod_bus_volt_read;       /* '<Root>/Input3' */
extern uint32_T mod_fault;             /* '<Root>/Input5' */
extern uint8_T mod_direction;          /* '<Root>/Input4' */
extern real_T Acc_r;                   /* '<S6>/Divide' */
extern real_T delta_r_in;              /* '<Root>/Product' */
extern real_T mod_id_cmd;              /* '<Root>/Saturation1' */
extern real_T mod_iq_cmd;              /* '<S3>/Switch2' */
extern real_T Acc_r_init;              /* '<Root>/Acc_selfcheck' */
extern uint8_T Acc_state_out;          /* '<Root>/Acc_selfcheck' */
extern boolean_T mod_motor_enable;     /* '<S4>/Logical Operator1' */
extern boolean_T mod_init_false;       /* '<Root>/Relational Operator' */

/* Model entry point functions */
extern void Driver_Strategy_initialize(void);
extern void Driver_Strategy_step(void);
extern void Driver_Strategy_terminate(void);

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S10>/Data Type Conversion' : Eliminate redundant data type conversion
 * Block '<S10>/Data Type Conversion1' : Eliminate redundant data type conversion
 * Block '<S10>/Data Type Conversion2' : Eliminate redundant data type conversion
 * Block '<S10>/Data Type Conversion3' : Eliminate redundant data type conversion
 * Block '<S10>/Data Type Conversion4' : Eliminate redundant data type conversion
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'Driver_Strategy'
 * '<S1>'   : 'Driver_Strategy/Acc_Rate'
 * '<S2>'   : 'Driver_Strategy/Acc_selfcheck'
 * '<S3>'   : 'Driver_Strategy/Bus_Volt_Fit'
 * '<S4>'   : 'Driver_Strategy/Enable1'
 * '<S5>'   : 'Driver_Strategy/Enable_delay'
 * '<S6>'   : 'Driver_Strategy/Low Pass Filter'
 * '<S7>'   : 'Driver_Strategy/Output_Rate_Limit'
 * '<S8>'   : 'Driver_Strategy/Respond'
 * '<S9>'   : 'Driver_Strategy/Subsystem'
 * '<S10>'  : 'Driver_Strategy/Subsystem1'
 * '<S11>'  : 'Driver_Strategy/Respond/Chart1'
 * '<S12>'  : 'Driver_Strategy/Respond/Subsystem'
 */
#endif                                 /* RTW_HEADER_Driver_Strategy_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
