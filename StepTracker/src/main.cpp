#include <Arduino.h>
#include "hw_config.h"
#include "SelfTest.h"
#include "Calibration.h"
#include "StepCounter.h"
#include "PaceClassifier.h"
#include "DisplayUI.h"

#define SIMULATION_MODE false
#define RUN_FAKE_RANDOM_DATA false

BoardConfig boardConfig{
  32, // redLedPin
  33, // yellowLedPin
  25, // greenLedPin
  5,  // stPin
  26, // xAdcPin
  27, // yAdcPin
  14, // zAdcPin
  18, // pushButtonAPin
  19, // pushButtonBPin
  21, // lcdSDAPin
  22  // lcdSCLPin
};

SelfTest selfTest(boardConfig);
Calibration calibration(boardConfig);
StepCounter stepCounter;
PaceClassifier paceClassifier;

CalibrationResult calResult;

unsigned long lastButtonPressMs = 0;
const unsigned long buttonDebounceMs = 300;

DisplayUI display(boardConfig.lcdSDAPin, boardConfig.lcdSCLPin, 0x27);

enum class FakeActivity {
  STATIONARY,
  WALKING,
  RUNNING
};

FakeActivity currentFakeActivity = FakeActivity::WALKING;

unsigned long lastActivityChangeMs = 0;
unsigned long activityDurationMs = 5000;

FakeActivity randomActivityDifferentFromCurrent() {
  FakeActivity newActivity;

  do {
      int r = random(0, 3);

      if (r == 0) {
          newActivity = FakeActivity::STATIONARY;
      } 
      else if (r == 1) {
          newActivity = FakeActivity::WALKING;
      } 
      else {
          newActivity = FakeActivity::RUNNING;
      }

  } while (newActivity == currentFakeActivity);

  return newActivity;
}

const char* fakeActivityToString(FakeActivity activity) {
  switch (activity) {
      case FakeActivity::STATIONARY:
          return "STATIONARY";
      case FakeActivity::WALKING:
          return "WALKING";
      case FakeActivity::RUNNING:
          return "RUNNING";
      default:
          return "UNKNOWN";
  }
}

void updateFakeActivity(unsigned long nowMs) {
  if (nowMs - lastActivityChangeMs >= activityDurationMs) {
      currentFakeActivity = randomActivityDifferentFromCurrent();

      activityDurationMs = random(4000, 8000);
      lastActivityChangeMs = nowMs;

      Serial.print("Fake activity changed to: ");
      Serial.println(fakeActivityToString(currentFakeActivity));
  }
}

AccelData getFakeAccel(unsigned long nowMs) {
  updateFakeActivity(nowMs);

  double noise = random(-5, 6) / 100.0; // -0.05g to +0.05g

  if (currentFakeActivity == FakeActivity::STATIONARY) {
      return {
          0.0 + noise,
          0.0 + noise,
          1.0 + noise
      };
  }

  if (currentFakeActivity == FakeActivity::WALKING) {
      if (nowMs % 800 < 180) {
          return {
              0.10 + noise,
              0.05 + noise,
              1.80 + noise
          };
      }

      return {
          0.02 + noise,
          0.01 + noise,
          1.00 + noise
      };
  }

  // RUNNING
  if (nowMs % 400 < 100) {
      return {
          0.10 + noise,
          0.05 + noise,
          1.85 + noise
      };
  }

  return {
      0.03 + noise,
      0.02 + noise,
      1.00 + noise
  };
}

void resetStepTracking() {
  stepCounter.reset();
  paceClassifier.reset(millis());

  Serial.println("SW1 pressed: step count reset");

  display.showStartup();
  delay(500);
}

void recalibrateSensor() {
  Serial.println("SW2 pressed: recalibration started");

  digitalWrite(boardConfig.greenLedPin, LOW);
  digitalWrite(boardConfig.yellowLedPin, HIGH);

  display.showCalibration(false);

#if SIMULATION_MODE
  Serial.println("Simulation mode: calibration skipped");
#else
  calResult = calibration.runInteractive(Serial);

  if (!calResult.valid) {
      Serial.println("Recalibration FAILED");

      digitalWrite(boardConfig.redLedPin, HIGH);
      digitalWrite(boardConfig.yellowLedPin, LOW);

      display.showCalibration(false);
      delay(1000);
      return;
  }

  Serial.println("Recalibration PASSED");
  calibration.printResult(calResult, Serial);
#endif

  stepCounter.reset();
  paceClassifier.reset(millis());

  digitalWrite(boardConfig.redLedPin, LOW);
  digitalWrite(boardConfig.yellowLedPin, LOW);
  digitalWrite(boardConfig.greenLedPin, HIGH);

  display.showCalibration(true);
  delay(1000);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  initializeBoardPins(boardConfig);
  display.begin();
  display.showStartup();
  delay(1000);
  
  randomSeed(micros());

#if SIMULATION_MODE
  Serial.println("SIMULATION MODE ENABLED");
  Serial.println("Self-test bypassed");
  Serial.println("Calibration bypassed");
#else
  Serial.println("Running self-test...");

  bool selfTestPassed = selfTest.run(3.3);

  if (!selfTestPassed) {
      Serial.println("Self-test FAILED");
      digitalWrite(boardConfig.redLedPin, HIGH);

      while (true) {
          delay(1000);
      }
  }

  Serial.println("Self-test PASSED");

  Serial.println("Starting calibration...");
  calResult = calibration.runInteractive(Serial);

  if (!calResult.valid) {
      Serial.println("Calibration FAILED");
      digitalWrite(boardConfig.redLedPin, HIGH);

      while (true) {
          delay(1000);
      }
  }

  Serial.println("Calibration PASSED");
#endif

  stepCounter.reset();
  paceClassifier.reset(millis());

  digitalWrite(boardConfig.greenLedPin, HIGH);
}

void loop() {
  unsigned long now = millis();

  if (now - lastButtonPressMs > buttonDebounceMs) {
    if (digitalRead(boardConfig.pushButtonAPin) == HIGH) {
        lastButtonPressMs = now;
        resetStepTracking();
        return;
    }
  
    if (digitalRead(boardConfig.pushButtonBPin) == HIGH) {
        lastButtonPressMs = now;
        recalibrateSensor();
        return;
    }
  }

  AccelData accel;

#if SIMULATION_MODE
  accel = getFakeAccel(now);
#else
  int xRaw = analogRead(boardConfig.xAdcPin);
  int yRaw = analogRead(boardConfig.yAdcPin);
  int zRaw = analogRead(boardConfig.zAdcPin);

  accel.xG = calibration.toG(xRaw, calResult.x);
  accel.yG = calibration.toG(yRaw, calResult.y);
  accel.zG = calibration.toG(zRaw, calResult.z);
#endif

  bool stepDetected = stepCounter.update(accel, now);

  if (stepDetected) {
      paceClassifier.onStep();
      digitalWrite(boardConfig.yellowLedPin, HIGH);
  } else {
      digitalWrite(boardConfig.yellowLedPin, LOW);
  }

  paceClassifier.update(now);

  double stepsPerMinute = paceClassifier.getCadence();
  double stepsPerSecond = stepsPerMinute / 60.0;

  display.update(
      stepCounter.getStepCount(),
      stepsPerMinute,
      stepsPerSecond,
      paceClassifier.getPaceType(),
      stepDetected
  );

#if SIMULATION_MODE  
  Serial.print("Fake:");
  Serial.print(fakeActivityToString(currentFakeActivity));
#else
  Serial.print("RealSensor");
#endif
  Serial.print(" | X:");
  Serial.print(accel.xG, 2);

  Serial.print(" Y:");
  Serial.print(accel.yG, 2);

  Serial.print(" Z:");
  Serial.print(accel.zG, 2);

  Serial.print(" | Move:");
  Serial.print(stepCounter.getMovementIntensity(), 2);

  Serial.print(" | Steps:");
  Serial.print(stepCounter.getStepCount());

  Serial.print(" | Cadence:");
  Serial.print(paceClassifier.getCadence(), 1);

  Serial.print(" | Classified:");
  Serial.println(paceClassifier.getPaceString());

  delay(100);
}