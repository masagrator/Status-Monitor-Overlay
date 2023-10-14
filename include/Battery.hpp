///* Notes VoltageAvg
//
//    Vavg time = 175.8ms x 2^(6+VOLT), default: VOLT = 2 (Vavg time = 45s)
//
///End of Notes

typedef enum {
    NoHub  = BIT(0),  //If hub is disconnected
    Rail   = BIT(8),  //At least one Joy-con is charging from rail
    SPDSRC = BIT(12), //OTG
    ACC    = BIT(16)  //Accessory
} BatteryChargeInfoFieldsFlags;

typedef enum {
    NewPDO               = 1, //Received new Power Data Object
    NoPD                 = 2, //No Power Delivery source is detected
    AcceptedRDO          = 3  //Received and accepted Request Data Object
} BatteryChargeInfoFieldsPDControllerState; //BM92T series

typedef enum {
    None         = 0,
    PD           = 1,
    TypeC_1500mA = 2,
    TypeC_3000mA = 3,
    DCP          = 4,
    CDP          = 5,
    SDP          = 6,
    Apple_500mA  = 7,
    Apple_1000mA = 8,
    Apple_2000mA = 9
} BatteryChargeInfoFieldsChargerType;

typedef enum {
    Sink         = 1,
    Source       = 2
} BatteryChargeInfoFieldsPowerRole;

typedef struct {
    int32_t InputCurrentLimit;                                  //Input (Sink) current limit in mA
    int32_t VBUSCurrentLimit;                                   //Output (Source/VBUS/OTG) current limit in mA
    int32_t ChargeCurrentLimit;                                 //Battery charging current limit in mA (512mA when Docked, 768mA when BatteryTemperature < 17.0 C)
    int32_t ChargeVoltageLimit;                                 //Battery charging voltage limit in mV (3952mV when BatteryTemperature >= 51.0 C)
    int32_t unk_x10;                                            //Possibly an emum, getting the same value as PowerRole in all tested cases
    int32_t unk_x14;                                            //Possibly flags
    BatteryChargeInfoFieldsPDControllerState PDControllerState; //Power Delivery Controller State
    int32_t BatteryTemperature;                                 //Battery temperature in milli C
    int32_t RawBatteryCharge;                                   //Raw battery charged capacity per cent-mille (i.e. 100% = 100000 pcm)
    int32_t VoltageAvg;                                         //Voltage avg in mV (more in Notes)
    int32_t BatteryAge;                                         //Battery age (capacity full / capacity design) per cent-mille (i.e. 100% = 100000 pcm)
    BatteryChargeInfoFieldsPowerRole PowerRole;
    BatteryChargeInfoFieldsChargerType ChargerType;
    int32_t ChargerVoltageLimit;                                //Charger and external device voltage limit in mV
    int32_t ChargerCurrentLimit;                                //Charger and external device current limit in mA
    BatteryChargeInfoFieldsFlags Flags;                         //Unknown flags
    char reserved[0x14];                                        //17.0.0+ data
} BatteryChargeInfoFields;

typedef struct {
    int32_t InputCurrentLimit;                                  //Input (Sink) current limit in mA
    int32_t VBUSCurrentLimit;                                   //Output (Source/VBUS/OTG) current limit in mA
    int32_t ChargeCurrentLimit;                                 //Battery charging current limit in mA (512mA when Docked, 768mA when BatteryTemperature < 17.0 C)
    int32_t ChargeVoltageLimit;                                 //Battery charging voltage limit in mV (3952mV when BatteryTemperature >= 51.0 C)
    int32_t unk_x10;                                            //Possibly an emum, getting the same value as PowerRole in all tested cases
    int32_t unk_x14;                                            //Possibly flags
    BatteryChargeInfoFieldsPDControllerState PDControllerState; //Power Delivery Controller State
    int32_t BatteryTemperature;                                 //Battery temperature in milli C
    int32_t RawBatteryCharge;                                   //Raw battery charged capacity per cent-mille (i.e. 100% = 100000 pcm)
    int32_t VoltageAvg;                                         //Voltage avg in mV (more in Notes)
    int32_t BatteryAge;                                         //Battery age (capacity full / capacity design) per cent-mille (i.e. 100% = 100000 pcm)
    char reserved[4];                                           //17.0.0+ data
    BatteryChargeInfoFieldsPowerRole PowerRole;
    BatteryChargeInfoFieldsChargerType ChargerType;
    int32_t ChargerVoltageLimit;                                //Charger and external device voltage limit in mV
    int32_t ChargerCurrentLimit;                                //Charger and external device current limit in mA
    BatteryChargeInfoFieldsFlags Flags;                         //Unknown flags
    char reserved2[0x10];                                        //17.0.0+ data
} BatteryChargeInfoFields17;

Result psmGetBatteryChargeInfoFields(Service* psmService, BatteryChargeInfoFields *out) {
    return serviceDispatchOut(psmService, 17, *out);
}