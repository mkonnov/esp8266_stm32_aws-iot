#ifndef DATA_DESCRIPTORS_H
#define DATA_DESCRIPTORS_H

/*
- Alerts (STM32 -> AWS)
- Reports (STM32 -> AWS)
- Commands (AWS -> STM32)
*/

typedef unsigned char byte;

enum AkiMessageAlert {
	AlertNoAlert,
	AlertWaterLeak,
	AlertLowBattery,
	AlertNoSignal,
	AlertWaterflowLeak,
	AlertMovementInactivity,
	AlertMovementActivity,
	AlertNum
};

enum AkiMessageReport {
	ReportInvalid,
	ReportInitial,
	ReportSystem,
	ReportDevices,
	ReportValvesWaterflows,
	ReportNum
};

typedef enum {
	CommandInvalid,
	CommandOpen,
	CommandClose,
	CommandClear,
	CommandReport,
	CommandSetClock,
	CommandSetSystemParams,
	CommandWriteFlash,
	CommandReadFlash,
	CommandJumpBoot,
	CommandNum
} AkiCommand;

struct AkiCommandData {
	AkiCommand command;
	uint8_t data[8];
};

struct SystemParams {

	union {
	byte data[8];
	struct {
                union {
		uint8_t b1;
		struct {
                        unsigned char autoTestEnabled : 1;
			unsigned char snapshotDaily : 1;
			unsigned char motionCloseEnabled : 1;
			unsigned char inactivityReport : 1;
			unsigned char activityReport : 1;
			unsigned char reserved : 3;
		};
		};
		uint8_t waterFlowProgram;
		uint8_t motionProgram;
		uint8_t b4;
		uint8_t b5;
		uint8_t b6;
		uint8_t b7;
		uint8_t b8;
	};
	};
};

struct AkiFlashPayload {

	uint16_t address;
	uint8_t data[256];
	uint8_t crc; // Calculated on data[] only

};

struct ReportSystem {

	uint8_t hwVersion[4];
	uint8_t swVersion[4];

	SystemParams systemParams;

	enum Valve : uint8_t {
                ValveDefect = 0,
                ValveOne = 1,
                ValveTwo = 2,
                ValveBoth = 3
	} valve = ValveDefect;

	enum Peripheral : uint8_t {
                PeripheralNone = 0,
                PeripheralInvalid = 1,
                PeripheralValveMaster = 2,
                PeripheralValveCR05 = 3,
                PeripheralSplitterLegacy = 4,
                PeripheralSplitterCR05 = 5,
	} peripheral = PeripheralNone;

	enum ValvePosition : uint8_t {
                ValvePositionInvalid = 0,
                ValvePositionOpened = 1,
                ValvePositionClosed = 2,
                ValvePositionUnknown = 3
                
        } valve1 = ValvePositionUnknown,
        valve2 = ValvePositionUnknown;

	enum WaterFlow : uint8_t {
                WaterFlowNone = 0,
                WaterFlow1 = 1,
                WaterFlow2 = 2,
                WaterFlowBoth = 3
	} waterFlowPresence = WaterFlowBoth;

	uint32_t wf1 = 0;
	uint32_t wf2 = 0;

	enum MotionProgram : uint8_t {
                MotionProgramDefect = 0,
                MotionProgramNone = 1,
                MotionProgram6 = 6,
                MotionProgram12 = 12,
                MotionProgram24 = 24
	};

	enum WaterFlowProgram : uint8_t {
                WaterFlowProgram0 = 0,
                WaterFlowProgram1 = 1,
                WaterFlowProgram30 = 30,
                WaterFlowProgram45 = 45,
                WaterFlowProgram60 = 60
	};

	enum Update : uint8_t {
                Update1 = 10,
                Update2 = 11,
                Update3 = 12,
                Update4 = 13,
                Update5 = 14,
                Update7 = 15,
                Update14 = 16
	};
};

struct AkiSensor {

	uint16_t id;
	uint16_t probeStatus;

	union {
	uint16_t values;
	struct {
		// - Voltage: 0.8V - 1.8V in 1V/32 increments
		// - Temperature: -10 - 52 degrés (32x 2 Celcius increments)
		//- Humidity: 0-95% in 5.625% increments (16 steps)
		unsigned char voltage : 5;
		unsigned char temperature : 5;
		unsigned char humidity : 5;
	};
	};

};

struct AkiReportDevices {

	uint8_t deviceCount = 0;
	AkiSensor deviceList[];

};

struct AkiReportValvesWaterflows {

	struct {

                uint16_t id;

                uint8_t valves[2] = {0, 0};
                uint32_t waterflows[2] = {0xFFFFFFFF, 0xFFFFFFFF};

	} splitters[4];

};

struct AkiAlertData {
        AkiMessageAlert alert;
        uint8_t data[8];
};

#endif /* DATA_DESCRIPTORS_H */
