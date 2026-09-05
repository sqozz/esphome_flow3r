#include "max98091.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace max98091 {

static const char *TAG = "max98091.component";
  void max98091Component::write_data(uint8_t reg, uint8_t data) {
    if (this->write(&reg, data) != i2c::ERROR_OK) {
      this->mark_failed();
      return;
    }
  }

  void max98091Component::setup() {
    this->write_data(SOFTWARE_RESET, 1<<7);
    this->write_data(DEVICE_SHUTDOWN, 0);
    // pclk = mclk / 1
    this->write_data(SYSTEM_CLOCK, MAX98091_BITS(SYSTEM_CLOCK_PSCLK, 1));
    // music, dc filter in record and playback
    this->write_data(FILTER_CONFIGURATION, (1 << 7) | (1 << 6) | (1 << 5) | (1 << 2));
    // Sets up DAI for left-justified slave mode operation.
    this->write_data(DAI_INTERFACE, 1 << 2);
    // Sets up the DAC to speaker path
    this->write_data(DAC_PATH, 1 << 5);
    // Somehow this was needed to get an input signal to the ADC, even though
    // all other registers should be taken care of later. Don't know why.
    // Sets up the line in to adc path
    this->write_data(LINE_TO_ADC, 1 << 6);
    // SDOUT, SDIN enabled
    this->write_data(IO_CONFIGURATION, (1 << 1) | (1 << 0));
    // bandgap bias
    this->write_data(BIAS_CONTROL, 1 << 0);
    // high performane mode
    this->write_data(DAC_CONTROL, 1 << 0);
    // enable micbias, line input amps, ADCs
    this->write_data(INPUT_ENABLE, (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0));
    // IN3 SE -> Line A, IN4 SE -> Line B
    this->write_data(LINE_INPUT_CONFIG, (1 << 3) | (1 << 2));
    // 64x oversampling, dithering, high performance ADC
    this->write_data(ADC_CONTROL, (1 << 1) | (1 << 0));
    this->write_data(DIGITAL_MIC_ENABLE, 0);
    // IN5/IN6 to MIC1
    this->write_data(INPUT_MODE, (1 << 0));

    this->write_data(LEFT_SPK_MIXER,
                   MAX98091_BOOL(LEFT_SPK_MIXER_LINE_A, false) |
                       MAX98091_ON(LEFT_SPK_MIXER_LEFT_DAC));
    this->write_data(RIGHT_SPK_MIXER,
                   MAX98091_BOOL(RIGHT_SPK_MIXER_LINE_B, false) |
                       MAX98091_ON(RIGHT_SPK_MIXER_RIGHT_DAC));

    this->write_data(MAX98091_LEFT_ADC_MIXER, 0);
    this->write_data(MAX98091_RIGHT_ADC_MIXER, 0);
/*
    flow3r_bsp_max98091_line_in_set_hardware_thru(0);
    flow3r_bsp_max98091_headset_set_gain_dB(0);
    flow3r_bsp_max98091_input_set_source(flow3r_bsp_audio_input_source_none);
*/
    // output enable: enable dacs
    this->write_data(OUTPUT_ENABLE, (1 << 1) | (1 << 0));
    // power up
    this->write_data(DEVICE_SHUTDOWN, 1 << 7);
    // enable outputs, dacs
    this->write_data(OUTPUT_ENABLE, (1 << 7) | (1 << 6) | (1 << 5) | (1 << 4) | (1 << 1) | (1 << 0));
    // disable all digital filters except for dc blocking
    this->write_data(DSP_FILTER_ENABLE, 0x0);
    // jack detect enable
    this->write_data(JACK_DETECT, 1 << 7);
    ESP_LOGD("MAX98091", "Init done...");

    this->write_data(MAX98091_LEFT_SPK_VOLUME,
              MAX98091_BOOL(LEFT_SPK_VOLUME_SPLM, false) |
              MAX98091_BITS(LEFT_SPK_VOLUME_SPVOLL, 0x2C));
    this->write_data(MAX98091_RIGHT_SPK_VOLUME,
              MAX98091_BOOL(RIGHT_SPK_VOLUME_SPRM, false) |
              MAX98091_BITS(RIGHT_SPK_VOLUME_SPVOLR, 0x2C));
  }

  void max98091Component::loop() {

  }

  void max98091Component::dump_config() {
    ESP_LOGCONFIG(TAG, "max98091 I2C component");
  }

}  // namespace max98091
}  // namespace esphome

