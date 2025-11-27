#include <M5Unified.h>

// PWMピン設定
const int PWM0_PIN = 33;
const int PWM1_PIN = 32;

// PWMチャンネル設定
const int PWM0_CH = 0;   // High Speed Channel (Timer 0に対応)
const int PWM1_CH = 8;   // Low Speed Channel (Timer 2/3に対応) ← ここを変更！
const int PWM0_FREQ = 50;    // 50Hz (Ctrl)
const int PWM1_FREQ = 1000;  // 1000Hz (Chop)
const int PWM_RES = 10;      // 10bit (0-1023)

// スライダーの値（0-100）
int sliderCtrl = 0;
int sliderChop = 0;

void drawUI() {
  M5.Display.clear(WHITE);
  M5.Display.setTextColor(BLACK);
  
  // Ctrl スライダー
  M5.Display.setTextSize(2);
  M5.Display.setCursor(22, 19);
  M5.Display.println("Ctrl");
  
  M5.Display.setCursor(225, 19);
  M5.Display.printf("%d  ", sliderCtrl);
  
  // Ctrl スライダーバー
  M5.Display.fillRect(10, 50, 300, 20, 0xA0A0A0);
  M5.Display.fillRect(10, 50, (sliderCtrl * 300) / 100, 20, 0x08A2B0);
  
  // Chop スライダー
  M5.Display.setCursor(10, 138);
  M5.Display.println("Chop");
  
  M5.Display.setCursor(225, 138);
  M5.Display.printf("%d  ", sliderChop);
  
  // Chop スライダーバー
  M5.Display.fillRect(10, 170, 300, 20, 0xA0A0A0);
  M5.Display.fillRect(10, 170, (sliderChop * 300) / 100, 20, 0x08A2B0);
}

void updateCtrlUI() {
  M5.Display.setTextColor(BLACK, WHITE);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(225, 19);
  M5.Display.printf("%d  ", sliderCtrl);
  M5.Display.fillRect(10, 50, 300, 20, 0xA0A0A0);
  M5.Display.fillRect(10, 50, (sliderCtrl * 300) / 100, 20, 0x08A2B0);
}

void updateChopUI() {
  M5.Display.setTextColor(BLACK, WHITE);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(225, 138);
  M5.Display.printf("%d  ", sliderChop);
  M5.Display.fillRect(10, 170, 300, 20, 0xA0A0A0);
  M5.Display.fillRect(10, 170, (sliderChop * 300) / 100, 20, 0x08A2B0);
}

void updatePWM0() {
  // 0-100を0-1023にマッピング (Pythonでの体感を再現するためフルレンジ使用)
  int dutyValue = map(sliderCtrl, 0, 100, 0, 1023);
  ledcWrite(PWM0_CH, dutyValue);
  Serial.printf("PWM0(HighSpeed): val=%d -> duty=%d\n", sliderCtrl, dutyValue);
}

void updatePWM1() {
  // 0-100を0-1023にマッピング
  int dutyValue = map(sliderChop, 0, 100, 0, 1023);
  ledcWrite(PWM1_CH, dutyValue);
  Serial.printf("PWM1(LowSpeed): val=%d -> duty=%d\n", sliderChop, dutyValue);
}

void handleTouch() {
  auto t = M5.Touch.getDetail();
  
  if (t.isPressed() || t.isHolding()) {
    int x = t.x;
    int y = t.y;
    
    // Ctrl スライダー（Y: 50-70）
    if (y >= 50 && y <= 70 && x >= 10 && x <= 310) {
      int newValue = map(x, 10, 310, 0, 100);
      newValue = constrain(newValue, 0, 100);
      if (newValue != sliderCtrl) {
        sliderCtrl = newValue;
        updatePWM0();
        updateCtrlUI();
      }
    }
    
    // Chop スライダー（Y: 170-190）
    if (y >= 170 && y <= 190 && x >= 10 && x <= 310) {
      int newValue = map(x, 10, 310, 0, 100);
      newValue = constrain(newValue, 0, 100);
      if (newValue != sliderChop) {
        sliderChop = newValue;
        updatePWM1();
        updateChopUI();
      }
    }
  }
}

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  
  Serial.begin(115200);
  
  // PWM初期化
  ledcSetup(PWM0_CH, PWM0_FREQ, PWM_RES);
  ledcAttachPin(PWM0_PIN, PWM0_CH);
  ledcWrite(PWM0_CH, 0);
  
  ledcSetup(PWM1_CH, PWM1_FREQ, PWM_RES);
  ledcAttachPin(PWM1_PIN, PWM1_CH);
  ledcWrite(PWM1_CH, 0);
  
  sliderCtrl = 0;
  sliderChop = 0;
  
  drawUI();
}

void loop() {
  M5.update();
  handleTouch();
  delay(20);
}