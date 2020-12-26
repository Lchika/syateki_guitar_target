#include <memory>
#include <array>
#include <M5Stack.h>
#include <LovyanGFX.hpp>
#include <target.hpp>
#include <irRayDetector.hpp>
#include <fullColorLedHitReactor.hpp>

// private function declarations
static void init_lcd(void);
static void update_motor(int motor_power);
static void show_reflector_values(int top, int bottom);
static void show_motor_value(int power);
template<std::size_t N>
static void show_gun_id(std::array<target::Target*, N> targets);

// cnstant expressions
static constexpr int PIN_MOTOR_REF = 26;
static constexpr int PIN_MOTOR1 = 16;
static constexpr int PIN_MOTOR2 = 17;
static constexpr int PIN_BOTTOM_REFLECTOR = 35;
static constexpr int PIN_TOP_REFLECTOR = 36;

// global variables
static int motor_power = 0;
static LGFX lcd;
static std::array<target::Target*, 3> targets = {
  (new target::Target(std::unique_ptr<target::IHitReactor>(new FullColorLedHitReactor(1, 0x70, true)), std::unique_ptr<target::IRayDetector>(new IrRayDetector(0)))),
  (new target::Target(std::unique_ptr<target::IHitReactor>(new FullColorLedHitReactor(2, 0x70, false)), std::unique_ptr<target::IRayDetector>(new IrRayDetector(1)))),
  (new target::Target(std::unique_ptr<target::IHitReactor>(new FullColorLedHitReactor(3, 0x70, false)), std::unique_ptr<target::IRayDetector>(new IrRayDetector(2))))
};

// functions
void setup()
{
  Serial.begin(115200);
  M5.begin();
  M5.Power.begin();

  M5.Speaker.begin();
  M5.Speaker.mute();

  pinMode(PIN_MOTOR1, OUTPUT);
  pinMode(PIN_MOTOR2, OUTPUT);
  digitalWrite(PIN_MOTOR1, LOW);
  digitalWrite(PIN_MOTOR2, LOW);
  dacWrite(PIN_MOTOR_REF, 0);

  pinMode(PIN_BOTTOM_REFLECTOR, INPUT);
  pinMode(PIN_TOP_REFLECTOR, INPUT);

  for(target::Target* target: targets){
    target->maintenance();
  }
  init_lcd();

  show_motor_value(motor_power);
}

void loop()
{
  M5.update();
  int motor_power_diff = 10;
  if (M5.BtnA.isPressed())
  {
    if (motor_power - motor_power_diff >= -255)
    {
      motor_power -= motor_power_diff;
    }
    update_motor(motor_power);
  }
  if (M5.BtnC.isPressed())
  {
    if (motor_power + motor_power_diff <= 255)
    {
      motor_power += motor_power_diff;
    }
    update_motor(motor_power);
  }

  show_reflector_values(analogRead(PIN_TOP_REFLECTOR), analogRead(PIN_BOTTOM_REFLECTOR));

  show_gun_id(targets);
  delay(100);
}

static void init_lcd()
{
  lcd.init();
  lcd.setBrightness(100);
  lcd.clear();
  return;
}

static void update_motor(int motor_power)
{
  if (motor_power > 0)
  {
    dacWrite(PIN_MOTOR_REF, motor_power);
    digitalWrite(PIN_MOTOR1, HIGH);
    digitalWrite(PIN_MOTOR2, LOW);
  }
  else
  {
    dacWrite(PIN_MOTOR_REF, -motor_power);
    digitalWrite(PIN_MOTOR2, HIGH);
    digitalWrite(PIN_MOTOR1, LOW);
  }
  show_motor_value(motor_power);
}

static void show_reflector_values(int top, int bottom)
{
  lcd.setCursor(10, 10);
  lcd.printf("photo reflector: top=%4d, bottom=%4d", top, bottom);
  return;
}

static void show_motor_value(int power)
{
  lcd.setCursor(10, 20);
  lcd.printf("motor: power=%4d", power);
  return;
}

template<std::size_t N>
static void show_gun_id(std::array<target::Target*, N> targets)
{
  lcd.setCursor(10,30);
  lcd.print("gun id: ");
  int i = 0;
  for(target::Target* target: targets){
    lcd.printf("t[%d]=%d", i, target->update());
    i++;
  }
  return;
}
