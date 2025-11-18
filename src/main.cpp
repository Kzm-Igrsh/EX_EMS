#include <M5Unified.h>

// PWMピン設定
const int PWM0_PIN = 33;
const int PWM1_PIN = 32;

// PWMチャンネル設定
const int PWM0_CH = 0;
const int PWM1_CH = 1;
const int PWM_FREQ = 10000;  // 10kHz
const int PWM_RES = 10;      // 10bit (0-1023)

// スライダーの値（0-100）
int sliderCtrl = 0;
int sliderChop = 0;

// タッチ状態
bool touching = false;
int lastY = 0;

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
  // 0-100を0-1023にマッピング
  int duty = map(sliderCtrl, 0, 100, 0, 1023);
  ledcWrite(PWM0_CH, duty);
  Serial.printf("PWM0 (Pin%d): duty=%d (%d%%)\n", PWM0_PIN, duty, sliderCtrl);
}

void updatePWM1() {
  // 0-100を0-1023にマッピング
  int duty = map(sliderChop, 0, 100, 0, 1023);
  ledcWrite(PWM1_CH, duty);
  Serial.printf("PWM1 (Pin%d): duty=%d (%d%%)\n", PWM1_PIN, duty, sliderChop);
}

void handleTouch() {
  auto t = M5.Touch.getDetail();
  
  if (t.isPressed() || t.isHolding()) {
    int x = t.x;
    int y = t.y;
    
    // Ctrl スライダー（Y: 50-70）
    if (y >= 50 && y <= 70 && x >= 10 && x <= 310) {
      sliderCtrl = map(x, 10, 310, 0, 100);
      sliderCtrl = constrain(sliderCtrl, 0, 100);
      updatePWM0();
      drawUI();
    }
    
    // Chop スライダー（Y: 170-190）
    if (y >= 170 && y <= 190 && x >= 10 && x <= 310) {
      sliderChop = map(x, 10, 310, 0, 100);
      sliderChop = constrain(sliderChop, 0, 100);
      updatePWM1();
      drawUI();
    }
  }
}

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  
  Serial.begin(115200);
  Serial.println("EMS Control UI");
  
  // PWM初期化
  ledcSetup(PWM0_CH, PWM_FREQ, PWM_RES);
  ledcAttachPin(PWM0_PIN, PWM0_CH);
  ledcWrite(PWM0_CH, 0);
  
  ledcSetup(PWM1_CH, PWM_FREQ, PWM_RES);
  ledcAttachPin(PWM1_PIN, PWM1_CH);
  ledcWrite(PWM1_CH, 0);
  
  Serial.printf("PWM0: Pin=%d, Freq=%dHz\n", PWM0_PIN, PWM_FREQ);
  Serial.printf("PWM1: Pin=%d, Freq=%dHz\n", PWM1_PIN, PWM_FREQ);
  
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
  
  delay(10);
}