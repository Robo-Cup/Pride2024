#include <gyro.h>

#ifdef FAST_MODE
  // Top frequency is reported to be 1000Hz (but freq is somewhat variable)
  sh2_SensorId_t reportType = SH2_GYRO_INTEGRATED_RV;
  long reportIntervalUs = 2000;
#else
  // Top frequency is about 250Hz but this report is more accurate
  sh2_SensorId_t reportType = SH2_ARVR_STABILIZED_RV;
  long reportIntervalUs = 5000;
#endif

Adafruit_BNO08x  bno08x(-1);
sh2_SensorValue_t sensorValue;
gyro_data Gyro::latest_data;

bool gyroEnabled = false;

void Gyro::setup() {
  if (!bno08x.begin_I2C()) {
    Serial.println("[GYRO] Failed to find BNO08x chip");
  } else {
    gyroEnabled = true;
    Serial.println("[GYRO] Found BNO08x chip");
    setReports(reportType, reportIntervalUs);
  }
}

gyro_data Gyro::getData() {
  if (!gyroEnabled) {
    Serial.println("[GYRO] Gyro not enabled, using 0, 0, 0");
    return {0, 0, 0};
  }

  if (bno08x.wasReset()) {
    Serial.println("[GYRO] BNO08x was reset");
    setReports(reportType, reportIntervalUs);
  }

  if (bno08x.getSensorEvent(&sensorValue)) {
    // in this demo only one report type will be received depending on FAST_MODE define (above)
    switch (sensorValue.sensorId) {
      case SH2_ARVR_STABILIZED_RV:
        quaternionToEulerRV(&sensorValue.un.arvrStabilizedRV, &Gyro::latest_data, false);
      case SH2_GYRO_INTEGRATED_RV:
        // faster (more noise?)
        quaternionToEulerGI(&sensorValue.un.gyroIntegratedRV, &Gyro::latest_data, false);
        break;
    }
    Serial.println("[GYRO] Yaw: " + String(Gyro::latest_data.yaw) + ", Pitch: " + String(Gyro::latest_data.pitch) + ", Roll: " + String(Gyro::latest_data.roll));
  }
  return Gyro::latest_data;
}

void Gyro::setReports(sh2_SensorId_t reportType, long report_interval) {
  if (! bno08x.enableReport(reportType, report_interval)) {
    Serial.println("[GYRO] Could not enable stabilized remote vector");
  } else {
    Serial.println("[GYRO] Enabled stabilized remote vector");
  }
}

void Gyro::quaternionToEuler(float qr, float qi, float qj, float qk, gyro_data* ypr, bool degrees) {
  float sqr = sq(qr);
  float sqi = sq(qi);
  float sqj = sq(qj);
  float sqk = sq(qk);

  ypr->yaw = atan2(2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr));
  ypr->pitch = asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr));
  ypr->roll = atan2(2.0 * (qj * qk + qi * qr), (-sqi - sqj + sqk + sqr));

  if (degrees) {
    ypr->yaw *= RAD_TO_DEG;
    ypr->pitch *= RAD_TO_DEG;
    ypr->roll *= RAD_TO_DEG;
  }
}

void Gyro::quaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, gyro_data* ypr, bool degrees) {
  quaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j, rotational_vector->k, ypr, degrees);
}

void Gyro::quaternionToEulerGI(sh2_GyroIntegratedRV_t* rotational_vector, gyro_data* ypr, bool degrees) {
  quaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j, rotational_vector->k, ypr, degrees);
}