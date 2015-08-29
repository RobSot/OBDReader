#ifndef DEFINES_H
#define DEFINES_H

/* Macros genéricas */
#define NUM_OF_PIDS                 224
#define ISNT_AV_PID_COMMAND(x)      ((x != 0x20) && (x != 0x40) && (x != 0x60) && (x != 0x80) && (x != 0xA0) && (x != 0xC0) && (x != 0xE0))
#define ISNT_USELESS_PID(x)         ((x != 0x01) && (x != 0x02) && (x != 0x13) && (x != 0x1C) && (x != 0x41) && (x != 0x4F) && (x != 0x50) && (x != 0x51))
#define IS_FUEL_SYSTEM_MODES(x)     ((x == 1) || (x == 2) || (x == 4) || (x == 8) || (x == 16))
#define IS_OXIGEN_SYSTEM_MODES(x)     ((x == 1) || (x == 2) || (x == 4) || (x == 8))

/* Macros de Error*/
#define OBDREADER_NOT_RESPONDING1       1
#define OBDREADER_NOT_RESPONDING2       2
#define PID_ERROR                       3

/*** Comandos puerto serie ***
******************************/
#define CMD_START           'e'
#define CMD_STOP            's'
#define CMD_OBD             0x02
#define CMD_STATUS_LED      0x03

/**** Modos OBD ****
********************/
#define MODE_00             0x00
#define MODE_01             0x01
#define MODE_02             0x02
#define MODE_03             0x03
#define MODE_04             0x04
#define MODE_05             0x05
#define MODE_06             0x06
#define MODE_07             0x07
#define MODE_08             0x08
#define MODE_09             0x09

/******* PIDs OBD *******
*************************/
#define AV_PIDS_01_20                       0x00
#define MONITOR_STATUS                      0x01
#define FREEZE_DTC                          0x02
#define FUEL_SYSTEM_STATUS                  0x03
#define ENGINE_LOAD                         0x04
#define COOLANT_TEMP                        0x05
#define SHORT_TERM_FUEL_BANK1               0x06
#define LONG_TERM_FUEL_BANK1                0x07
#define SHORT_TERM_FUEL_BANK2               0x08
#define LONG_TERM_FUEL_BANK2                0x09
#define FUEL_PRESSURE                       0x0A
#define INTAKE_MANIFOLD_ABS_PRESSURE        0x0B
#define ENGINE_RPM                          0x0C
#define VEHICLE_SPEED                       0x0D
#define TIMING_ADVANCE                      0x0E
#define INTAKE_AIR_TEMP                     0x0F
#define MAF                                 0x10
#define THROTTLE_POSITION                   0x11
#define COMMANDED_SECONDARY_AIR_STATUS      0x12
#define OXYGEN_SENSORS_PRESENT              0x13
#define OXYGEN_BANK1_SENSOR1                0x14
#define OXYGEN_BANK1_SENSOR2                0x15
#define OXYGEN_BANK1_SENSOR3                0x16
#define OXYGEN_BANK1_SENSOR4                0x17
#define OXYGEN_BANK2_SENSOR1                0x18
#define OXYGEN_BANK2_SENSOR2                0x19
#define OXYGEN_BANK2_SENSOR3                0x1A
#define OXYGEN_BANK2_SENSOR4                0x1B
#define OBD_STANDARS_VEHICLE_COFORMS        0x1C
#define OXYGEN_SENSORS_PRESENT_2            0x1D
#define AUXILIARY_INPUT_STATUS              0x1E
#define RUN_TIME_SINCE_ENGINE_START         0x1F
#define AV_PIDS_21_40                       0x20
#define DISTANCE_TRAVELED_WITH_MIL_ON       0x21
#define FUEL_RAIL_PRESSURE                  0x22
#define FUEL_RAIL_PRESSURE_DIRECT_INJECT    0x23
#define O2_S1_LAMBDA_RATIO_AND_VOLTAGE      0x24
#define O2_S2_LAMBDA_RATIO_AND_VOLTAGE      0x25
#define O2_S3_LAMBDA_RATIO_AND_VOLTAGE      0x26
#define O2_S4_LAMBDA_RATIO_AND_VOLTAGE      0x27
#define O2_S5_LAMBDA_RATIO_AND_VOLTAGE      0x28
#define O2_S6_LAMBDA_RATIO_AND_VOLTAGE      0x29
#define O2_S7_LAMBDA_RATIO_AND_VOLTAGE      0x2A
#define O2_S8_LAMBDA_RATIO_AND_VOLTAGE      0x2B
#define COMMANDED_EGR                       0x2C
#define EGR_ERROR                           0x2D
#define COMMANDED_EVAPORATIVE_PURGE         0x2E
#define FUEL_LEVEL_INPUT                    0x2F
#define WARM_UPS_SINCE_DTC_CLEARED          0x30
#define DISTANCE_SINCE_DTC_CLEARED          0x31
#define EVAP_SYSTEM_VAPOR_PRESSURE          0x32
#define BAROMETRIC_PRESSURE                 0x33
#define O2_S1_LAMBDA_RATIO_AND_CURRENT      0x34
#define O2_S2_LAMBDA_RATIO_AND_CURRENT      0x35
#define O2_S3_LAMBDA_RATIO_AND_CURRENT      0x36
#define O2_S4_LAMBDA_RATIO_AND_CURRENT      0x37
#define O2_S5_LAMBDA_RATIO_AND_CURRENT      0x38
#define O2_S6_LAMBDA_RATIO_AND_CURRENT      0x39
#define O2_S7_LAMBDA_RATIO_AND_CURRENT      0x3A
#define O2_S8_LAMBDA_RATIO_AND_CURRENT      0x3B
#define CATALYST_TEMP_BANK1_SENSOR1         0x3C
#define CATALYST_TEMP_BANK2_SENSOR1         0x3D
#define CATALYST_TEMP_BANK1_SENSOR2         0x3E
#define CATALYST_TEMP_BANK2_SENSOR2         0x3F
#define AV_PIDS_41_60                       0x40
#define MONITOR_STATUS_THIS_DRIVE_CYCLE     0x41
#define CONTROL_MODULE_VOLTAGE              0x42
#define ABSOLUTE_LOAD_VALUE                 0x43
#define FUEL_AIR_COMMANDED_RATIO            0x44
#define RELATIVE_THROTTLE_POSSITION         0x45
#define AMBIENT_AIR_TEMPERATURE             0x46
#define ABSOLUTE_THROTTLE_POSITION_B        0x47
#define ABSOLUTE_THROTTLE_POSITION_C        0x48
#define ACCELERATOR_PEDAL_POSITION_D        0x49
#define ACCELERATOR_PEDAL_POSITION_E        0x4A
#define ACCELERATOR_PEDAL_POSITION_F        0x4B
#define COMMANDED_THROTTLE_ACTUATOR         0x4C
#define TIME_RUN_WITH_MIL_ON                0x4D
#define TIME_SINCE_DTC_CLEARED              0x4E
#define MAXIMUM_VALUE_FOR_SOME_SENSORS      0x4F
#define MAXIMUM_VALUE_FOR_MAF               0x50
#define FUEL_TYPE                           0x51
#define ETHANOL_FUEL_PERCENT                0x52
#define ABS_EVAP_SYSTEM_VAPOR_PRESSURE      0x53
#define EVAP_SYSTEM_VAPOR_PRESSURE_2        0x54
/*#define  0x55
#define  0x56
#define  0x57
#define  0x58*/
#define ABSOLUTE_FUEL_RAIL_PRESSURE         0x59
#define RELATIVE_ACCELERATOR_PEDAL_POS      0x5A
#define HYBRID_BATTERY_PACK_REMAINING       0x5B
#define ENGINE_OIL_TEMP                     0x5C
#define FUEL_INJECTION_TIMMING              0x5D
#define ENGINE_FUEL_RATE                    0x5E
#define EMISSION_REQUIREMENTS               0x5F
#define AV_PIDS_61_80                       0x60
#define DRIVERS_DEMAND_PERCENT_TORQUE       0x61
#define ACTUAL_ENGINE_PERCENT_TORQUE        0x62
#define ENGINE_REFERENCE_TORQUE             0x63
#define ENGINE_PERCENT_TORQUE_DATA          0x64
#define AV_PIDS_81_A0 0x80
#define AV_PIDS_A1_C0 0xA0
#define AV_PIDS_C1_E0 0xC0


/******** Strings ********
*************************/
#define EXCELL_PID_HEADER "Timestamp;01;02;03;04;05;06;07;08;09;0A;0B;0C;0D;0E;0F;10;11;12;13;14;15;16;17;18;19;1A;1B;1C;1D;1E;1F;21;22;23;24;25;26;27;28;29;2A;2B;2C;2D;2E;2F;30;31;32;33;34;35;36;37;38;39;3A;3B;3C;3D;3E;3F;41;42;43;44;45;46;47;48;49;4A;4B;4C;4D;4E;4F;50;51;52;53;54;55;56;57;58;59;5A;5B;5C;5D;5E;5F;61;62;63;64;65;66;67;68;69;6A;6B;6C;6D;6E;6F;70;71;72;73;74;75;76;77;78;79;7A;7B;7C;7D;7E;7F;81;82;83;84;85;86;87;88;89;8A;8B;8C;8D;8E;8F;90;91;92;93;94;95;96;97;98;99;9A;9B;9C;9D;9E;9F;A1;A2;A3;A4;A5;A6;A7;A8;A9;AA;AB;AC;AD;AE;AF;B0;B1;B2;B3;B4;B5;B6;B7;B8;B9;BA;BB;BC;BD;BE;BF;C1;C2;C3;C4;C5;C6;C7;C8;C9;CA;CB;CC;CD;CE;CF;D0;D1;D2;D3;D4;D5;D6;D7;D8;D9;DA;DB;DC;DD;DE;DF\n"
#define OBDREADER_VERSION "OBDReader v1.0.0"


#endif // DEFINES_H
