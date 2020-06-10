/*
 * CmpVL53L0X.c
 *
 * Created: 26/05/2020 15:18:32
 *  Author: louis
 */ 

#include "CmpVL53L0X.h"
#include "Drv/DrvTick.h"
#include "Drv/DrvTwi.h"


////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////

////////////////////////////////////////PRIVATE DEFINES///////////////////////////////////////////


////////////////////////////////////////PRIVATE VARIABLES/////////////////////////////////////////
Int8U stop = 0x00U;
Int16U VL53L0XDistance = 0x00U;
Int32U io_timeout;
Boolean did_timeout;
Int32U timeoutCmpVL53L0X = 0U;
Int32U measurement_timing_budget_us = 0U;
////////////////////////////////////////PRIVATE STRUCTURES////////////////////////////////////////
typedef struct 
{
	Boolean tcc;
	Boolean msrc;
	Boolean dss;
	Boolean pre_range;
	Boolean final_range;
}SequenceStepEnables;
typedef struct 
{
	Int16U pre_range_vcsel_period_pclks, final_range_vcsel_period_pclks;
	Int16U msrc_dss_tcc_mclks, pre_range_mclks, final_range_mclks;
	Int32U msrc_dss_tcc_us,    pre_range_us,    final_range_us;
}SequenceStepTimeouts;
typedef enum  
{ 
	VcselPeriodPreRange, 
	VcselPeriodFinalRange 
}VcselPeriodType;


////////////////////////////////////////PRIVATE MACROS///////////////////////////////////////////
#define startTimeout() (timeoutCmpVL53L0X = DrvTickGetTimeMs())
#define checkTimeoutExpired() (io_timeout > 0 && ((Int32U)(DrvTickGetTimeMs() - timeoutCmpVL53L0X) > io_timeout))
#define decodeVcselPeriod(reg_val)      (((reg_val) + 1) << 1)
#define encodeVcselPeriod(period_pclks) (((period_pclks) >> 1) - 1)
#define calcMacroPeriod(vcsel_period_pclks) ((((Int32U)2304 * (vcsel_period_pclks) * 1655) + 500) / 1000)
inline Int32U CmpVL53L0XGetTimeout(void) { return io_timeout; }
	
////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////

void CmpVL53L0XWriteReg(Int8U reg, Int8U value);
void CmpVL53L0XWriteReg16Bit(Int8U reg, Int16U value);
void CmpVL53L0XWriteReg32Bit(Int8U reg, Int32U value);
Int8U CmpVL53L0XReadReg(Int8U reg);
Int16U CmpVL53L0XReadReg16Bit(Int8U reg);
Int32U CmpVL53L0XReadReg32Bit(Int8U reg);
void CmpVL53L0XWriteMulti(Int8U reg, Int8U * src, Int8U count);
void CmpVL53L0XReadMulti(Int8U reg, Int8U * dst, Int8U count);
void CmpVL53L0XTune( void );
void CmpVL53L0XGetSequenceStepEnables(SequenceStepEnables *enables);
void CmpVL53L0XGetSequenceStepTimeouts(SequenceStepEnables const * enables, SequenceStepTimeouts * timeouts);
Int16U CmpVL53L0XDecodeTimeout(Int16U reg_val);
Int16U CmpVL53L0XEncodeTimeout(Int32U timeout_mclks);
Int8U CmpVL53L0XGetVcselPulsePeriod(VcselPeriodType type);
Int32U CmpVL53L0XTimeoutMclksToMicroseconds(Int16U timeout_period_mclks, Int8U vcsel_period_pclks);
Int32U CmpVL53L0XTimeoutMicrosecondsToMclks(Int32U timeout_period_us, Int8U vcsel_period_pclks);
Boolean  CmpVL53L0XSetSignalRateLimit(float limit_Mcps);
float  CmpVL53L0XGetSignalRateLimit(void);
Boolean CmpVL53L0XGetSpadInfo(Int8U * count, Boolean * type_is_aperture);
Boolean CmpVL53L0XSetMeasurementTimingBudget(Int32U budget_us);
Int32U CmpVL53L0XGetMeasurementTimingBudget(void);
Boolean CmpVL53L0XPerformSingleRefCalibration(Int8U vhv_init_byte);
Boolean CmpVL53L0XSetVcselPulsePeriod(VcselPeriodType type, Int8U period_pclks);


////////////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////////
Boolean CmpVL53L0XInit( float limit_Mcps, Int32U budget_us )
{
	// check model ID register (value specified in datasheet)
	if (CmpVL53L0XReadReg(VL53L0X_IDENTIFICATION_MODEL_ID) != 0xEE) { return FALSE; }

	// VL53L0X_DataInit() begin

	// sensor uses 1V8 mode for I/O by default; switch to 2V8 mode if necessary
	CmpVL53L0XWriteReg(VL53L0X_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV,
	CmpVL53L0XReadReg(VL53L0X_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV) | 0x01); // set bit 0

	// "Set I2C standard mode"
	CmpVL53L0XWriteReg(0x88, 0x00);

	CmpVL53L0XWriteReg(0x80, 0x01);
	CmpVL53L0XWriteReg(0xFF, 0x01);
	CmpVL53L0XWriteReg(0x00, 0x00);
	stop = CmpVL53L0XReadReg(0x91);
	CmpVL53L0XWriteReg(0x00, 0x01);
	CmpVL53L0XWriteReg(0xFF, 0x00);
	CmpVL53L0XWriteReg(0x80, 0x00);

	// disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks
	CmpVL53L0XWriteReg(VL53L0X_MSRC_CONFIG_CONTROL, CmpVL53L0XReadReg(VL53L0X_MSRC_CONFIG_CONTROL) | 0x12);

	// set final range signal rate limit to 0.25 MCPS (million counts per second)
	CmpVL53L0XSetSignalRateLimit(0.25);

	CmpVL53L0XWriteReg(VL53L0X_SYSTEM_SEQUENCE_CONFIG, 0xFF);

	// VL53L0X_DataInit() end

	// VL53L0X_StaticInit() begin

	Int8U spad_count;
	Boolean spad_type_is_aperture;
	if (!CmpVL53L0XGetSpadInfo(&spad_count, &spad_type_is_aperture)) { return FALSE; }

	// The SPAD map (RefGoodSpadMap) is read by VL53L0X_get_info_from_device() in
	// the API, but the same data seems to be more easily readable from
	// GLOBAL_CONFIG_SPAD_ENABLES_REF_0 through _6, so read it from there
	Int8U ref_spad_map[6];
	CmpVL53L0XReadMulti(VL53L0X_GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

	// -- VL53L0X_set_reference_spads() begin (assume NVM values are valid)

	CmpVL53L0XWriteReg(0xFF, 0x01);
	CmpVL53L0XWriteReg(VL53L0X_DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00);
	CmpVL53L0XWriteReg(VL53L0X_DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C);
	CmpVL53L0XWriteReg(0xFF, 0x00);
	CmpVL53L0XWriteReg(VL53L0X_GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4);

	uint8_t first_spad_to_enable = spad_type_is_aperture ? 12 : 0; // 12 is the first aperture spad
	uint8_t spads_enabled = 0;

	for (uint8_t i = 0; i < 48; i++)
	{
		if (i < first_spad_to_enable || spads_enabled == spad_count)
		{
			// This bit is lower than the first one that should be enabled, or
			// (reference_spad_count) bits have already been enabled, so zero this bit
			ref_spad_map[i / 8] &= ~(1 << (i % 8));
		}
		else if ((ref_spad_map[i / 8] >> (i % 8)) & 0x1)
		{
			spads_enabled++;
		}
	}

	CmpVL53L0XWriteMulti(VL53L0X_GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);
	
	CmpVL53L0XTune();
	
	
	// "Set interrupt config to new sample ready"
	// -- VL53L0X_SetGpioConfig() begin

	CmpVL53L0XWriteReg(VL53L0X_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);
	CmpVL53L0XWriteReg(VL53L0X_GPIO_HV_MUX_ACTIVE_HIGH, CmpVL53L0XReadReg(VL53L0X_GPIO_HV_MUX_ACTIVE_HIGH) & ~0x10); // active low
	CmpVL53L0XWriteReg(VL53L0X_SYSTEM_INTERRUPT_CLEAR, 0x01);
	
	
	
	measurement_timing_budget_us = CmpVL53L0XGetMeasurementTimingBudget();

	// "Disable MSRC and TCC by default"
	// MSRC = Minimum Signal Rate Check
	// TCC = Target CentreCheck
	// -- VL53L0X_SetSequenceStepEnable() begin

	CmpVL53L0XWriteReg(VL53L0X_SYSTEM_SEQUENCE_CONFIG, 0xE8);

	// -- VL53L0X_SetSequenceStepEnable() end

	// "Recalculate timing budget"
	CmpVL53L0XSetMeasurementTimingBudget(measurement_timing_budget_us);

	// VL53L0X_StaticInit() end

	// VL53L0X_PerformRefCalibration() begin (VL53L0X_perform_ref_calibration())

	// -- VL53L0X_perform_vhv_calibration() begin

	CmpVL53L0XWriteReg(VL53L0X_SYSTEM_SEQUENCE_CONFIG, 0x01);
	if (!CmpVL53L0XPerformSingleRefCalibration(0x40)) { return FALSE; }

	// -- VL53L0X_perform_vhv_calibration() end

	// -- VL53L0X_perform_phase_calibration() begin

	CmpVL53L0XWriteReg(VL53L0X_SYSTEM_SEQUENCE_CONFIG, 0x02);
	if (!CmpVL53L0XPerformSingleRefCalibration(0x00)) { return FALSE; }

	// -- VL53L0X_perform_phase_calibration() end

	// "restore the previous Sequence Config"
	CmpVL53L0XWriteReg(VL53L0X_SYSTEM_SEQUENCE_CONFIG, 0xE8);

	// VL53L0X_PerformRefCalibration() end
	CmpVL53L0XSetVcselPulsePeriod ( VcselPeriodPreRange, 18);

	CmpVL53L0XSetVcselPulsePeriod ( VcselPeriodFinalRange, 14);

	return TRUE;
}

void CmpVL53L0XSetTimeout(Int32U timeout) 
{
	 io_timeout = timeout; 
}

Int16U CmpVL53L0XGetDistance( void )
{
	CmpVL53L0XWriteReg(0x80, 0x01);
	CmpVL53L0XWriteReg(0xFF, 0x01);
	CmpVL53L0XWriteReg(0x00, 0x00);
	CmpVL53L0XWriteReg(0x91, stop);
	CmpVL53L0XWriteReg(0x00, 0x01);
	CmpVL53L0XWriteReg(0xFF, 0x00);
	CmpVL53L0XWriteReg(0x80, 0x00);

	CmpVL53L0XWriteReg(VL53L0X_SYSRANGE_START, 0x01);

	// "Wait until start bit has been cleared"
	startTimeout();
	while (CmpVL53L0XReadReg(VL53L0X_SYSRANGE_START) & 0x01)
	{
		if (checkTimeoutExpired())
		{
			did_timeout = FALSE;
			return 65535;
		}
	}

	return CmpVL53L0XGetDistanceContinuous();
}

Int16U CmpVL53L0XGetDistanceContinuous( void )
{
	// assumptions: Linearity Corrective Gain is 1000 (default);
	// fractional ranging is not enabled
	uint16_t range = CmpVL53L0XReadReg16Bit(VL53L0X_RESULT_RANGE_STATUS + 10);

	CmpVL53L0XWriteReg(VL53L0X_SYSTEM_INTERRUPT_CLEAR, 0x01);

	return range;
}

Boolean CmpVL53L0XTimeoutOccurred(void)
{
	Boolean tmp = did_timeout;
	did_timeout = FALSE;
	return tmp;
}


Boolean CmpVL53L0XAvailable(void)
{
	if ((CmpVL53L0XReadReg(VL53L0X_RESULT_INTERRUPT_STATUS) & 0x07) == 0)
	{
		return FALSE;
	}
	return TRUE;
}

////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
void CmpVL53L0XStartContinuous( Int32U period_ms )
{
	CmpVL53L0XWriteReg(0x80, 0x01);
	CmpVL53L0XWriteReg(0xFF, 0x01);
	CmpVL53L0XWriteReg(0x00, 0x00);
	CmpVL53L0XWriteReg(0x91, stop);
	CmpVL53L0XWriteReg(0x00, 0x01);
	CmpVL53L0XWriteReg(0xFF, 0x00);
	CmpVL53L0XWriteReg(0x80, 0x00);

	if (period_ms != 0)
	{
		// continuous timed mode

		// VL53L0X_SetInterMeasurementPeriodMilliSeconds() begin

		uint16_t osc_calibrate_val = CmpVL53L0XReadReg16Bit(VL53L0X_OSC_CALIBRATE_VAL);

		if (osc_calibrate_val != 0)
		{
			period_ms *= osc_calibrate_val;
		}

		CmpVL53L0XWriteReg32Bit(VL53L0X_SYSTEM_INTERMEASUREMENT_PERIOD, period_ms);

		// VL53L0X_SetInterMeasurementPeriodMilliSeconds() end

		CmpVL53L0XWriteReg(VL53L0X_SYSRANGE_START, 0x04); // VL53L0X_REG_SYSRANGE_MODE_TIMED
	}
	else
	{
		// continuous back-to-back mode
		CmpVL53L0XWriteReg(VL53L0X_SYSRANGE_START, 0x02); // VL53L0X_REG_SYSRANGE_MODE_BACKTOBACK
	}
}

void CmpVL53L0XStopContinuous( void )
{
	CmpVL53L0XWriteReg(VL53L0X_SYSRANGE_START, 0x01); // VL53L0X_REG_SYSRANGE_MODE_SINGLESHOT

	CmpVL53L0XWriteReg(0xFF, 0x01);
	CmpVL53L0XWriteReg(0x00, 0x00);
	CmpVL53L0XWriteReg(0x91, 0x00);
	CmpVL53L0XWriteReg(0x00, 0x01);
	CmpVL53L0XWriteReg(0xFF, 0x00);
}

Boolean CmpVL53L0XSetMeasurementTimingBudget(Int32U budget_us)
{
	SequenceStepEnables enables;
	SequenceStepTimeouts timeouts;

	Int16U const StartOverhead     = 1910;
	Int16U const EndOverhead        = 960;
	Int16U const MsrcOverhead       = 660;
	Int16U const TccOverhead        = 590;
	Int16U const DssOverhead        = 690;
	Int16U const PreRangeOverhead   = 660;
	Int16U const FinalRangeOverhead = 550;

	Int32U const MinTimingBudget = 20000;

	if (budget_us < MinTimingBudget) { return FALSE; }

	Int32U used_budget_us = StartOverhead + EndOverhead;

	CmpVL53L0XGetSequenceStepEnables(&enables);
	CmpVL53L0XGetSequenceStepTimeouts(&enables, &timeouts);

	if (enables.tcc)
	{
		used_budget_us += (timeouts.msrc_dss_tcc_us + TccOverhead);
	}

	if (enables.dss)
	{
		used_budget_us += 2 * (timeouts.msrc_dss_tcc_us + DssOverhead);
	}
	else if (enables.msrc)
	{
		used_budget_us += (timeouts.msrc_dss_tcc_us + MsrcOverhead);
	}

	if (enables.pre_range)
	{
		used_budget_us += (timeouts.pre_range_us + PreRangeOverhead);
	}

	if (enables.final_range)
	{
		used_budget_us += FinalRangeOverhead;

		// "Note that the final range timeout is determined by the timing
		// budget and the sum of all other timeouts within the sequence.
		// If there is no room for the final range timeout, then an error
		// will be set. Otherwise the remaining time will be applied to
		// the final range."

		if (used_budget_us > budget_us)
		{
			// "Requested timeout too big."
			return FALSE;
		}

		Int32U final_range_timeout_us = budget_us - used_budget_us;

		// set_sequence_step_timeout() begin
		// (SequenceStepId == VL53L0X_SEQUENCESTEP_FINAL_RANGE)

		// "For the final range timeout, the pre-range timeout
		//  must be added. To do this both final and pre-range
		//  timeouts must be expressed in macro periods MClks
		//  because they have different vcsel periods."

		Int32U final_range_timeout_mclks =
		CmpVL53L0XTimeoutMicrosecondsToMclks(final_range_timeout_us,
		timeouts.final_range_vcsel_period_pclks);

		if (enables.pre_range)
		{
			final_range_timeout_mclks += timeouts.pre_range_mclks;
		}

		CmpVL53L0XWriteReg16Bit(VL53L0X_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI,CmpVL53L0XEncodeTimeout(final_range_timeout_mclks));

		// set_sequence_step_timeout() end
		measurement_timing_budget_us = budget_us; // store for internal reuse
	}
	return TRUE;
}

Int32U CmpVL53L0XGetMeasurementTimingBudget(void)
{
	SequenceStepEnables enables;
	SequenceStepTimeouts timeouts;

	Int16U const StartOverhead     = 1910;
	Int16U const EndOverhead        = 960;
	Int16U const MsrcOverhead       = 660;
	Int16U const TccOverhead        = 590;
	Int16U const DssOverhead        = 690;
	Int16U const PreRangeOverhead   = 660;
	Int16U const FinalRangeOverhead = 550;

	// "Start and end overhead times always present"
	Int32U budget_us = StartOverhead + EndOverhead;

	CmpVL53L0XGetSequenceStepEnables(&enables);
	CmpVL53L0XGetSequenceStepTimeouts(&enables, &timeouts);

	if (enables.tcc)
	{
		budget_us += (timeouts.msrc_dss_tcc_us + TccOverhead);
	}

	if (enables.dss)
	{
		budget_us += 2 * (timeouts.msrc_dss_tcc_us + DssOverhead);
	}
	else if (enables.msrc)
	{
		budget_us += (timeouts.msrc_dss_tcc_us + MsrcOverhead);
	}

	if (enables.pre_range)
	{
		budget_us += (timeouts.pre_range_us + PreRangeOverhead);
	}

	if (enables.final_range)
	{
		budget_us += (timeouts.final_range_us + FinalRangeOverhead);
	}

	measurement_timing_budget_us = budget_us; // store for internal reuse
	return budget_us;
}

Boolean CmpVL53L0XGetSpadInfo(Int8U * count, Boolean * type_is_aperture)
{
	uint8_t tmp;

	CmpVL53L0XWriteReg(0x80, 0x01);
	CmpVL53L0XWriteReg(0xFF, 0x01);
	CmpVL53L0XWriteReg(0x00, 0x00);

	CmpVL53L0XWriteReg(0xFF, 0x06);
	CmpVL53L0XWriteReg(0x83, CmpVL53L0XReadReg(0x83) | 0x04);
	CmpVL53L0XWriteReg(0xFF, 0x07);
	CmpVL53L0XWriteReg(0x81, 0x01);

	CmpVL53L0XWriteReg(0x80, 0x01);

	CmpVL53L0XWriteReg(0x94, 0x6b);
	CmpVL53L0XWriteReg(0x83, 0x00);
	startTimeout();
	while (CmpVL53L0XReadReg(0x83) == 0x00)
	{
		if (checkTimeoutExpired()) { return FALSE; }
	}
	CmpVL53L0XWriteReg(0x83, 0x01);
	tmp = CmpVL53L0XReadReg(0x92);

	*count = tmp & 0x7f;
	*type_is_aperture = (tmp >> 7) & 0x01;

	CmpVL53L0XWriteReg(0x81, 0x00);
	CmpVL53L0XWriteReg(0xFF, 0x06);
	CmpVL53L0XWriteReg(0x83, CmpVL53L0XReadReg(0x83)  & ~0x04);
	CmpVL53L0XWriteReg(0xFF, 0x01);
	CmpVL53L0XWriteReg(0x00, 0x01);

	CmpVL53L0XWriteReg(0xFF, 0x00);
	CmpVL53L0XWriteReg(0x80, 0x00);

	return TRUE;
}

void CmpVL53L0XGetSequenceStepEnables(SequenceStepEnables * enables)
{
	uint8_t sequence_config = CmpVL53L0XReadReg(VL53L0X_SYSTEM_SEQUENCE_CONFIG);

	enables->tcc          = (sequence_config >> 4) & 0x1;
	enables->dss          = (sequence_config >> 3) & 0x1;
	enables->msrc         = (sequence_config >> 2) & 0x1;
	enables->pre_range    = (sequence_config >> 6) & 0x1;
	enables->final_range  = (sequence_config >> 7) & 0x1;
}

void CmpVL53L0XGetSequenceStepTimeouts(SequenceStepEnables const * enables, SequenceStepTimeouts * timeouts)
{
	timeouts->pre_range_vcsel_period_pclks = CmpVL53L0XGetVcselPulsePeriod(VcselPeriodPreRange);

	timeouts->msrc_dss_tcc_mclks = CmpVL53L0XReadReg(VL53L0X_MSRC_CONFIG_TIMEOUT_MACROP) + 1;
	timeouts->msrc_dss_tcc_us =
	CmpVL53L0XTimeoutMclksToMicroseconds(timeouts->msrc_dss_tcc_mclks,
	timeouts->pre_range_vcsel_period_pclks);

	timeouts->pre_range_mclks =
	CmpVL53L0XDecodeTimeout(CmpVL53L0XReadReg16Bit(VL53L0X_PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI));
	timeouts->pre_range_us =
	CmpVL53L0XTimeoutMclksToMicroseconds(timeouts->pre_range_mclks,
	timeouts->pre_range_vcsel_period_pclks);

	timeouts->final_range_vcsel_period_pclks = CmpVL53L0XGetVcselPulsePeriod(VcselPeriodFinalRange);

	timeouts->final_range_mclks =
	CmpVL53L0XDecodeTimeout(CmpVL53L0XReadReg16Bit(VL53L0X_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI));

	if (enables->pre_range)
	{
		timeouts->final_range_mclks -= timeouts->pre_range_mclks;
	}

	timeouts->final_range_us =
	CmpVL53L0XTimeoutMclksToMicroseconds(timeouts->final_range_mclks,
	timeouts->final_range_vcsel_period_pclks);
}

Int16U CmpVL53L0XDecodeTimeout(Int16U reg_val)
{
	// format: "(LSByte * 2^MSByte) + 1"
	return (Int16U)((reg_val & 0x00FF) << (Int16U)((reg_val & 0xFF00) >> 8)) + 1;
}

Int16U CmpVL53L0XEncodeTimeout(Int32U timeout_mclks)
{
	// format: "(LSByte * 2^MSByte) + 1"

	Int32U ls_byte = 0;
	Int16U ms_byte = 0;

	if (timeout_mclks > 0)
	{
		ls_byte = timeout_mclks - 1;

		while ((ls_byte & 0xFFFFFF00) > 0)
		{
			ls_byte >>= 1;
			ms_byte++;
		}

		return (ms_byte << 8) | (ls_byte & 0xFF);
	}
	else { return 0; }
}

Int8U CmpVL53L0XGetVcselPulsePeriod(VcselPeriodType type)
{
	if (type == VcselPeriodPreRange)
	{
		return decodeVcselPeriod(CmpVL53L0XReadReg(VL53L0X_PRE_RANGE_CONFIG_VCSEL_PERIOD));
	}
	else if (type == VcselPeriodFinalRange)
	{
		return decodeVcselPeriod(CmpVL53L0XReadReg(VL53L0X_FINAL_RANGE_CONFIG_VCSEL_PERIOD));
	}
	else { return 255; }
}
Boolean CmpVL53L0XSetVcselPulsePeriod(VcselPeriodType type, Int8U period_pclks)
{
  Int8U vcsel_period_reg = encodeVcselPeriod(period_pclks);

  SequenceStepEnables enables;
  SequenceStepTimeouts timeouts;

  CmpVL53L0XGetSequenceStepEnables(&enables);
  CmpVL53L0XGetSequenceStepTimeouts(&enables, &timeouts);

  // "Apply specific settings for the requested clock period"
  // "Re-calculate and apply timeouts, in macro periods"

  // "When the VCSEL period for the pre or final range is changed,
  // the corresponding timeout must be read from the device using
  // the current VCSEL period, then the new VCSEL period can be
  // applied. The timeout then must be written back to the device
  // using the new VCSEL period.
  //
  // For the MSRC timeout, the same applies - this timeout being
  // dependant on the pre-range vcsel period."


  if (type == VcselPeriodPreRange)
  {
    // "Set phase check limits"
    switch (period_pclks)
    {
      case 12:
        CmpVL53L0XWriteReg(VL53L0X_PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x18);
        break;

      case 14:
        CmpVL53L0XWriteReg(VL53L0X_PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x30);
        break;

      case 16:
        CmpVL53L0XWriteReg(VL53L0X_PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x40);
        break;

      case 18:
        CmpVL53L0XWriteReg(VL53L0X_PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x50);
        break;

      default:
        // invalid period
        return FALSE;
    }
    CmpVL53L0XWriteReg(VL53L0X_PRE_RANGE_CONFIG_VALID_PHASE_LOW, 0x08);

    // apply new VCSEL period
    CmpVL53L0XWriteReg(VL53L0X_PRE_RANGE_CONFIG_VCSEL_PERIOD, vcsel_period_reg);

    // update timeouts

    // set_sequence_step_timeout() begin
    // (SequenceStepId == VL53L0X_SEQUENCESTEP_PRE_RANGE)

    uint16_t new_pre_range_timeout_mclks =
      CmpVL53L0XTimeoutMicrosecondsToMclks(timeouts.pre_range_us, period_pclks);

    CmpVL53L0XWriteReg16Bit(VL53L0X_PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI,
      CmpVL53L0XEncodeTimeout(new_pre_range_timeout_mclks));

    // set_sequence_step_timeout() end

    // set_sequence_step_timeout() begin
    // (SequenceStepId == VL53L0X_SEQUENCESTEP_MSRC)

    uint16_t new_msrc_timeout_mclks =
      CmpVL53L0XTimeoutMicrosecondsToMclks(timeouts.msrc_dss_tcc_us, period_pclks);

    CmpVL53L0XWriteReg(VL53L0X_MSRC_CONFIG_TIMEOUT_MACROP,
      (new_msrc_timeout_mclks > 256) ? 255 : (new_msrc_timeout_mclks - 1));

    // set_sequence_step_timeout() end
  }
  else if (type == VcselPeriodFinalRange)
  {
    switch (period_pclks)
    {
      case 8:
        CmpVL53L0XWriteReg(VL53L0X_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x10);
        CmpVL53L0XWriteReg(VL53L0X_FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
        CmpVL53L0XWriteReg(VL53L0X_GLOBAL_CONFIG_VCSEL_WIDTH, 0x02);
        CmpVL53L0XWriteReg(VL53L0X_ALGO_PHASECAL_CONFIG_TIMEOUT, 0x0C);
        CmpVL53L0XWriteReg(0xFF, 0x01);
        CmpVL53L0XWriteReg(VL53L0X_ALGO_PHASECAL_LIM, 0x30);
        CmpVL53L0XWriteReg(0xFF, 0x00);
        break;

      case 10:
        CmpVL53L0XWriteReg(VL53L0X_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x28);
        CmpVL53L0XWriteReg(VL53L0X_FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
        CmpVL53L0XWriteReg(VL53L0X_GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
        CmpVL53L0XWriteReg(VL53L0X_ALGO_PHASECAL_CONFIG_TIMEOUT, 0x09);
        CmpVL53L0XWriteReg(0xFF, 0x01);
        CmpVL53L0XWriteReg(VL53L0X_ALGO_PHASECAL_LIM, 0x20);
        CmpVL53L0XWriteReg(0xFF, 0x00);
        break;

      case 12:
        CmpVL53L0XWriteReg(VL53L0X_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x38);
        CmpVL53L0XWriteReg(VL53L0X_FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
        CmpVL53L0XWriteReg(VL53L0X_GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
        CmpVL53L0XWriteReg(VL53L0X_ALGO_PHASECAL_CONFIG_TIMEOUT, 0x08);
        CmpVL53L0XWriteReg(0xFF, 0x01);
        CmpVL53L0XWriteReg(VL53L0X_ALGO_PHASECAL_LIM, 0x20);
        CmpVL53L0XWriteReg(0xFF, 0x00);
        break;

      case 14:
        CmpVL53L0XWriteReg(VL53L0X_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x48);
        CmpVL53L0XWriteReg(VL53L0X_FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
        CmpVL53L0XWriteReg(VL53L0X_GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
        CmpVL53L0XWriteReg(VL53L0X_ALGO_PHASECAL_CONFIG_TIMEOUT, 0x07);
        CmpVL53L0XWriteReg(0xFF, 0x01);
        CmpVL53L0XWriteReg(VL53L0X_ALGO_PHASECAL_LIM, 0x20);
        CmpVL53L0XWriteReg(0xFF, 0x00);
        break;

      default:
        // invalid period
        return FALSE;
    }

    // apply new VCSEL period
    CmpVL53L0XWriteReg(VL53L0X_FINAL_RANGE_CONFIG_VCSEL_PERIOD, vcsel_period_reg);

    // update timeouts

    // set_sequence_step_timeout() begin
    // (SequenceStepId == VL53L0X_SEQUENCESTEP_FINAL_RANGE)

    // "For the final range timeout, the pre-range timeout
    //  must be added. To do this both final and pre-range
    //  timeouts must be expressed in macro periods MClks
    //  because they have different vcsel periods."

    uint16_t new_final_range_timeout_mclks =
      CmpVL53L0XTimeoutMicrosecondsToMclks(timeouts.final_range_us, period_pclks);

    if (enables.pre_range)
    {
      new_final_range_timeout_mclks += timeouts.pre_range_mclks;
    }

    CmpVL53L0XWriteReg16Bit(VL53L0X_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI,
      CmpVL53L0XEncodeTimeout(new_final_range_timeout_mclks));

    // set_sequence_step_timeout end
  }
  else
  {
    // invalid type
    return FALSE;
  }

  // "Finally, the timing budget must be re-applied"

  CmpVL53L0XSetMeasurementTimingBudget(measurement_timing_budget_us);

  // "Perform the phase calibration. This is needed after changing on vcsel period."
  // VL53L0X_perform_phase_calibration() begin

  Int8U sequence_config = CmpVL53L0XReadReg(VL53L0X_SYSTEM_SEQUENCE_CONFIG);
  CmpVL53L0XWriteReg(VL53L0X_SYSTEM_SEQUENCE_CONFIG, 0x02);
  CmpVL53L0XPerformSingleRefCalibration(0x0);
  CmpVL53L0XWriteReg(VL53L0X_SYSTEM_SEQUENCE_CONFIG, sequence_config);

  // VL53L0X_perform_phase_calibration() end

  return TRUE;
}

Int32U CmpVL53L0XTimeoutMclksToMicroseconds(Int16U timeout_period_mclks, Int8U vcsel_period_pclks)
{
	Int32U macro_period_ns = calcMacroPeriod(vcsel_period_pclks);

	return ((timeout_period_mclks * macro_period_ns) + 500) / 1000;
}

Int32U CmpVL53L0XTimeoutMicrosecondsToMclks(Int32U timeout_period_us, Int8U vcsel_period_pclks)
{
	Int32U macro_period_ns = calcMacroPeriod(vcsel_period_pclks);

	return (((timeout_period_us * 1000) + (macro_period_ns / 2)) / macro_period_ns);
}

Boolean CmpVL53L0XSetSignalRateLimit(float limit_Mcps)
{
	if (limit_Mcps < 0 || limit_Mcps > 511.99) { return FALSE; }

	// Q9.7 fixed point format (9 integer bits, 7 fractional bits)
	CmpVL53L0XWriteReg16Bit(VL53L0X_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, limit_Mcps * (1 << 7));
	return TRUE;
}
float CmpVL53L0XGetSignalRateLimit(void)
{
	return (float)CmpVL53L0XReadReg16Bit(VL53L0X_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT) / (1 << 7);
}

Boolean CmpVL53L0XPerformSingleRefCalibration(Int8U vhv_init_byte)
{
	CmpVL53L0XWriteReg(VL53L0X_SYSRANGE_START, 0x01 | vhv_init_byte); // VL53L0X_REG_SYSRANGE_MODE_START_STOP

	startTimeout();
	while ((CmpVL53L0XReadReg(VL53L0X_RESULT_INTERRUPT_STATUS) & 0x07) == 0)
	{
		if (checkTimeoutExpired()) { return FALSE; }
	}

	CmpVL53L0XWriteReg(VL53L0X_SYSTEM_INTERRUPT_CLEAR, 0x01);

	CmpVL53L0XWriteReg(VL53L0X_SYSRANGE_START, 0x00);

	return TRUE;
}

void CmpVL53L0XTune( void )
{
	CmpVL53L0XWriteReg(0xFF, 0x01);
	CmpVL53L0XWriteReg(0x00, 0x00);

	CmpVL53L0XWriteReg(0xFF, 0x00);
	CmpVL53L0XWriteReg(0x09, 0x00);
	CmpVL53L0XWriteReg(0x10, 0x00);
	CmpVL53L0XWriteReg(0x11, 0x00);

	CmpVL53L0XWriteReg(0x24, 0x01);
	CmpVL53L0XWriteReg(0x25, 0xFF);
	CmpVL53L0XWriteReg(0x75, 0x00);

	CmpVL53L0XWriteReg(0xFF, 0x01);
	CmpVL53L0XWriteReg(0x4E, 0x2C);
	CmpVL53L0XWriteReg(0x48, 0x00);
	CmpVL53L0XWriteReg(0x30, 0x20);

	CmpVL53L0XWriteReg(0xFF, 0x00);
	CmpVL53L0XWriteReg(0x30, 0x09);
	CmpVL53L0XWriteReg(0x54, 0x00);
	CmpVL53L0XWriteReg(0x31, 0x04);
	CmpVL53L0XWriteReg(0x32, 0x03);
	CmpVL53L0XWriteReg(0x40, 0x83);
	CmpVL53L0XWriteReg(0x46, 0x25);
	CmpVL53L0XWriteReg(0x60, 0x00);
	CmpVL53L0XWriteReg(0x27, 0x00);
	CmpVL53L0XWriteReg(0x50, 0x06);
	CmpVL53L0XWriteReg(0x51, 0x00);
	CmpVL53L0XWriteReg(0x52, 0x96);
	CmpVL53L0XWriteReg(0x56, 0x08);
	CmpVL53L0XWriteReg(0x57, 0x30);
	CmpVL53L0XWriteReg(0x61, 0x00);
	CmpVL53L0XWriteReg(0x62, 0x00);
	CmpVL53L0XWriteReg(0x64, 0x00);
	CmpVL53L0XWriteReg(0x65, 0x00);
	CmpVL53L0XWriteReg(0x66, 0xA0);

	CmpVL53L0XWriteReg(0xFF, 0x01);
	CmpVL53L0XWriteReg(0x22, 0x32);
	CmpVL53L0XWriteReg(0x47, 0x14);
	CmpVL53L0XWriteReg(0x49, 0xFF);
	CmpVL53L0XWriteReg(0x4A, 0x00);

	CmpVL53L0XWriteReg(0xFF, 0x00);
	CmpVL53L0XWriteReg(0x7A, 0x0A);
	CmpVL53L0XWriteReg(0x7B, 0x00);
	CmpVL53L0XWriteReg(0x78, 0x21);

	CmpVL53L0XWriteReg(0xFF, 0x01);
	CmpVL53L0XWriteReg(0x23, 0x34);
	CmpVL53L0XWriteReg(0x42, 0x00);
	CmpVL53L0XWriteReg(0x44, 0xFF);
	CmpVL53L0XWriteReg(0x45, 0x26);
	CmpVL53L0XWriteReg(0x46, 0x05);
	CmpVL53L0XWriteReg(0x40, 0x40);
	CmpVL53L0XWriteReg(0x0E, 0x06);
	CmpVL53L0XWriteReg(0x20, 0x1A);
	CmpVL53L0XWriteReg(0x43, 0x40);

	CmpVL53L0XWriteReg(0xFF, 0x00);
	CmpVL53L0XWriteReg(0x34, 0x03);
	CmpVL53L0XWriteReg(0x35, 0x44);

	CmpVL53L0XWriteReg(0xFF, 0x01);
	CmpVL53L0XWriteReg(0x31, 0x04);
	CmpVL53L0XWriteReg(0x4B, 0x09);
	CmpVL53L0XWriteReg(0x4C, 0x05);
	CmpVL53L0XWriteReg(0x4D, 0x04);

	CmpVL53L0XWriteReg(0xFF, 0x00);
	CmpVL53L0XWriteReg(0x44, 0x00);
	CmpVL53L0XWriteReg(0x45, 0x20);
	CmpVL53L0XWriteReg(0x47, 0x08);
	CmpVL53L0XWriteReg(0x48, 0x28);
	CmpVL53L0XWriteReg(0x67, 0x00);
	CmpVL53L0XWriteReg(0x70, 0x04);
	CmpVL53L0XWriteReg(0x71, 0x01);
	CmpVL53L0XWriteReg(0x72, 0xFE);
	CmpVL53L0XWriteReg(0x76, 0x00);
	CmpVL53L0XWriteReg(0x77, 0x00);

	CmpVL53L0XWriteReg(0xFF, 0x01);
	CmpVL53L0XWriteReg(0x0D, 0x01);

	CmpVL53L0XWriteReg(0xFF, 0x00);
	CmpVL53L0XWriteReg(0x80, 0x01);
	CmpVL53L0XWriteReg(0x01, 0xF8);

	CmpVL53L0XWriteReg(0xFF, 0x01);
	CmpVL53L0XWriteReg(0x8E, 0x01);
	CmpVL53L0XWriteReg(0x00, 0x01);
	CmpVL53L0XWriteReg(0xFF, 0x00);
	CmpVL53L0XWriteReg(0x80, 0x00);
}




// Write an 8-bit register
void CmpVL53L0XWriteReg(Int8U reg, Int8U value)
{
	DrvTwiWriteReg(VL53L0X_ADDRESS, reg, value);
}

// Write a 16-bit register
void CmpVL53L0XWriteReg16Bit(Int8U reg, Int16U value)
{	
	Int8U regBuf [2U];
	regBuf[0]=(Int8U)(value >> 8);
	regBuf[1]=(Int8U)value;
	DrvTwiWriteRegBuf(VL53L0X_ADDRESS, reg, regBuf, 2U);
}

// Write a 32-bit register
void CmpVL53L0XWriteReg32Bit(Int8U reg, Int32U value)
{
	Int8U regBuf [4U];
	regBuf[0]=(Int8U)(value >> 24);
	regBuf[1]=(Int8U)(value >> 16);
	regBuf[2]=(Int8U)(value >> 8);
	regBuf[2]=(Int8U)value;
	DrvTwiWriteRegBuf(VL53L0X_ADDRESS, reg, regBuf, 4U);
}

// Read an 8-bit register
Int8U CmpVL53L0XReadReg(Int8U reg)
{
	Int8U value;
	DrvTwiReadReg(VL53L0X_ADDRESS, reg, &value);
	return value;
}

// Read a 16-bit register
Int16U CmpVL53L0XReadReg16Bit(Int8U reg)
{
	Int16U value;
	Int8U regBuf[2U] = {0x00U};
	DrvTwiReadRegBuf(VL53L0X_ADDRESS, reg, regBuf, 2U);
	value  = (Int16U)( (Int16U)regBuf[0U] << 8); // value high byte
	value |=  regBuf[1U];      // value low byte
	return value;
}

// Read a 32-bit register
Int32U CmpVL53L0XReadReg32Bit(Int8U reg)
{
	Int32U value;
	Int8U regBuf[4U] = {0x00U};
	DrvTwiReadRegBuf(VL53L0X_ADDRESS, reg, regBuf, 4U);
	value  = (Int32U)((Int32U)regBuf[0U] << 24); // value highest byte
	value |= (Int32U)((Int32U)regBuf[1U] << 16);
	value |= (Int32U)((Int16U)regBuf[2U] << 8);
	value |= (Int32U)regBuf[3U];      // value low byte
	return value;
}

// Write an arbitrary number of bytes from the given array to the sensor,
// starting at the given register
void CmpVL53L0XWriteMulti(Int8U reg, Int8U * src, Int8U count)
{
	DrvTwiWriteRegBuf(VL53L0X_ADDRESS, reg, src, count);
}

// Read an arbitrary number of bytes from the sensor, starting at the given
// register, into the given array
void CmpVL53L0XReadMulti(Int8U reg, Int8U *dst, Int8U count)
{
	DrvTwiReadRegBuf(VL53L0X_ADDRESS, reg, dst, count);
}
