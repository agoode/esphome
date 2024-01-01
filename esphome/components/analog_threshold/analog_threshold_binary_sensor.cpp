#include "analog_threshold_binary_sensor.h"
#include "esphome/core/log.h"

namespace esphome::analog_threshold {

static const char *const TAG = "analog_threshold.binary_sensor";

void AnalogThresholdBinarySensor::setup() {
  this->sensor_->add_on_state_callback([this](float sensor_value) {
    if (std::isnan(sensor_value)) {
      // If there is an invalid sensor reading, ignore the change.
      return;
    }

    // Note: Use this->raw_state_ for hysteresis logic, not this->state which is post-filter.

    if (!this->has_state()) {
      // No prior state.
      if ((sensor_value < this->lower_threshold_.value()) || (sensor_value >= this->upper_threshold_.value())) {
        // Outside the hysteresis range, prime the initial state.
        this->raw_state_ = sensor_value >= this->upper_threshold_.value();
        this->publish_state(this->raw_state_);
        return;
      } else {
        // We are starting out in the middle of the hysteresis range and cannot make a determination of the
        // state of the sensor. Do not publish a value.
        return;
      }
    }

    float threshold;
    if (this->raw_state_) {
      // Currently TRUE, use lower_threshold for comparison.
      threshold = this->lower_threshold_.value();
    } else {
      // Currently FALSE, use upper_threshold for comparison.
      threshold = this->upper_threshold_.value();
    }
    this->raw_state_ = sensor_value >= threshold;
    this->publish_state(this->raw_state_);
  });
}

void AnalogThresholdBinarySensor::set_sensor(sensor::Sensor *analog_sensor) { this->sensor_ = analog_sensor; }

void AnalogThresholdBinarySensor::dump_config() {
  LOG_BINARY_SENSOR("", "Analog Threshold Binary Sensor", this);
  LOG_SENSOR("  ", "Sensor", this->sensor_);
  ESP_LOGCONFIG(TAG,
                "  Upper threshold: %.11f\n"
                "  Lower threshold: %.11f",
                this->upper_threshold_.value(), this->lower_threshold_.value());
}

}  // namespace esphome::analog_threshold
