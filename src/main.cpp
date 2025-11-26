#include <M5Unified.h>

// PWMピン設定
const int PWM0_PIN = 33;
const int PWM1_PIN = 32;

// PWMチャンネル設定
const int PWM0_CH = 0;
const int PWM1_CH = 1;
const int PWM0_FREQ = 50;    // 50Hz (Ctrl)
const int PWM1_FREQ = 1000;  // 1000Hz (Chop)
const int PWM_RES = 8;       // 8bit (0-255) - MicroPythonのduty()と同じ範囲

// スライダーの値（0-100）
int sliderCtrl = 0;
int sliderChop = 0;

// 前回の値（UI更新判定用）
int prevSliderCtrl = -1;
int prevSliderChop = -1;

void drawUI() {
  M5.Display.clear(WHITE);
  M5.Display.setTextColor(BLACK);
  
  // Ctrl スライダー
  M5.Display.setTextSize(2);
  M5.Display.setCursor(22, 19);
  M5.Display.println("Ctrl");
  
  M5.Display.setCursor(225, 19);
  M5.Display.printf("%d", sliderCtrl);
  
  // Ctrl スライダーバー
  M5.Display.fillRect(10, 50, 300, 20, 0xA0A0A0);
  M5.Display.fillRect(10, 50, (sliderCtrl * 300) / 100, 20, 0x08A2B0);
  
  // Chop スライダー
  M5.Display.setCursor(10, 138);
  M5.Display.println("Chop");
  
  M5.Display.setCursor(225, 138);
  M5.Display.printf("%d", sliderChop);
  
  // Chop スライダーバー
  M5.Display.fillRect(10, 170, 300, 20, 0xA0A0A0);
  M5.Display.fillRect(10, 170, (sliderChop * 300) / 100, 20, 0x08A2B0);
}

void updatePWM0() {
  // MicroPythonのduty()は0-100の範囲（内部で0-1023に変換される）
  // ESP32のledcWriteも同じように0-100をそのまま使用（8bit解像度なので0-255の範囲）
  int dutyValue = map(sliderCtrl, 0, 100, 0, 255);
  ledcWrite(PWM0_CH, dutyValue);
  Serial.printf("PWM0 (Pin%d, %dHz): duty=%d (raw=%d)\n", PWM0_PIN, PWM0_FREQ, sliderCtrl, dutyValue);
}

void updatePWM1() {
  // MicroPythonのduty()は0-100の範囲（内部で0-1023に変換される）
  int dutyValue = map(sliderChop, 0, 100, 0, 255);
  ledcWrite(PWM1_CH, dutyValue);
  Serial.printf("PWM1 (Pin%d, %dHz): duty=%d (raw=%d)\n", PWM1_PIN, PWM1_FREQ, sliderChop, dutyValue);
}

void handleTouch() {
  auto t = M5.Touch.getDetail();
  
  if (t.isPressed() || t.isHolding()) {
    int x = t.x;
    int y = t.y;
    bool needsRedraw = false;
    
    // Ctrl スライダー（Y: 50-70）
    if (y >= 50 && y <= 70 && x >= 10 && x <= 310) {
      int newValue = map(x, 10, 310, 0, 100);
      newValue = constrain(newValue, 0, 100);
      if (newValue != sliderCtrl) {
        sliderCtrl = newValue;
        updatePWM0();
        needsRedraw = true;
      }
    }
    
    // Chop スライダー（Y: 170-190）
    if (y >= 170 && y <= 190 && x >= 10 && x <= 310) {
      int newValue = map(x, 10, 310, 0, 100);
      newValue = constrain(newValue, 0, 100);
      if (newValue != sliderChop) {
        sliderChop = newValue;
        updatePWM1();
        needsRedraw = true;
      }
    }
    
    // 値が変わった時だけUI更新
    if (needsRedraw) {
      drawUI();
    }
  }
}

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  
  Serial.begin(115200);
  Serial.println("EMS Control UI");
  
  // PWM初期化（異なる周波数）
  ledcSetup(PWM0_CH, PWM0_FREQ, PWM_RES);
  ledcAttachPin(PWM0_PIN, PWM0_CH);
  ledcWrite(PWM0_CH, 0);
  
  ledcSetup(PWM1_CH, PWM1_FREQ, PWM_RES);
  ledcAttachPin(PWM1_PIN, PWM1_CH);
  ledcWrite(PWM1_CH, 0);
  
  Serial.printf("PWM0: Pin=%d, Freq=%dHz\n", PWM0_PIN, PWM0_FREQ);
  Serial.printf("PWM1: Pin=%d, Freq=%dHz\n", PWM1_PIN, PWM1_FREQ);
  
  // 初期値設定
  sliderCtrl = 0;
  sliderChop = 0;
  updatePWM0();
  updatePWM1();
  
  // UI描画
  drawUI();
}

void loop() {
  M5.update();
  
  // タッチ処理
  handleTouch();
  
  delay(50);  // UI更新頻度を下げて点滅を防止
}