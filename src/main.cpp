#include <memory>
#include <array>
#include <M5Stack.h>
#include <LovyanGFX.hpp>
#include <target.hpp>
#include <irRayDetector.hpp>
#include <fullColorLedHitReactor.hpp>
#include <motor.hpp>
#include <photo_reflector.hpp>
#include <servo.hpp>

// private function declarations
static void init_lcd(void);
static void update_motor(int motor_power);
static void show_reflector_values(int top, int bottom);
static void show_motor_value(int power);
template <std::size_t N>
static void show_gun_id(std::array<target::Target *, N> targets);
static void maintenance_servos(void);

// cnstant expressions
static constexpr int PIN_MOTOR_REF = 26;
static constexpr int PIN_MOTOR1 = 16;
static constexpr int PIN_MOTOR2 = 17;
static constexpr int PIN_BOTTOM_REFLECTOR = 35;
static constexpr int PIN_TOP_REFLECTOR = 36;
static constexpr int PIN_SERVO_PICK = 2;    // 起動設定に関係するピンなので注意
static constexpr int PIN_SERVO_VOLUMES = 5; // 起動設定に関係するピンなので注意

// global variables
static int motor_power = 0;
static LGFX lcd;
static std::array<target::Target *, 3> targets = {
    (new target::Target(std::unique_ptr<target::IHitReactor>(new FullColorLedHitReactor(1)), std::unique_ptr<target::IRayDetector>(new IrRayDetector(0)))),
    (new target::Target(std::unique_ptr<target::IHitReactor>(new FullColorLedHitReactor(2)), std::unique_ptr<target::IRayDetector>(new IrRayDetector(1)))),
    (new target::Target(std::unique_ptr<target::IHitReactor>(new FullColorLedHitReactor(3)), std::unique_ptr<target::IRayDetector>(new IrRayDetector(2))))};
static Motor motor(PIN_MOTOR_REF, PIN_MOTOR1, PIN_MOTOR2);
static PhotoReflector bottom_reflector(PIN_BOTTOM_REFLECTOR);
static PhotoReflector top_reflector(PIN_TOP_REFLECTOR);
static M5Servo servo_pick(0, PIN_SERVO_PICK);
static M5Servo servo_volumes(1, PIN_SERVO_VOLUMES);

// functions
void setup()
{
  M5.begin();
  dacWrite(25, 0);
  M5.Power.begin();

  // maintenance all parts
  for (target::Target *target : targets)
  {
    target->initialize();
    target->maintenance();
  }
  maintenance_servos();

  init_lcd();

  show_motor_value(motor_power);
}

void loop()
{
  M5.update();
  int motor_power_diff = 10;
  if (M5.BtnA.isPressed())
  {
    if (motor_power - motor_power_diff >= 0)
    {
      motor_power -= motor_power_diff;
    }
  }
  if (M5.BtnC.isPressed())
  {
    if (motor_power + motor_power_diff <= 255)
    {
      motor_power += motor_power_diff;
    }
  }
  update_motor(motor_power);

  show_reflector_values(top_reflector.value(), bottom_reflector.value());

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
  Direction direction = Direction::NOCHANGE;
  if (bottom_reflector.is_close())
  {
    direction = Direction::UP;
    if (top_reflector.is_close())
    {
      // 両方のフォトリフレクタが反応していたら念のためモータは止める
      motor_power = 0;
    }
  }
  else if (top_reflector.is_close())
  {
    direction = Direction::DOWN;
  }
  motor.set_power(motor_power, direction);
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

template <std::size_t N>
static void show_gun_id(std::array<target::Target *, N> targets)
{
  lcd.setCursor(10, 30);
  lcd.print("gun id: ");
  int i = 0;
  for (target::Target *target : targets)
  {
    lcd.printf("t[%d]=%d", i, target->update());
    i++;
  }
  return;
}

static void maintenance_servos(void)
{
  auto maintenance = [](M5Servo &servo) -> void {
    servo.write(-90);
    delay(1000);
    servo.write(90);
    delay(1000);
    servo.write(0);
  };
  maintenance(servo_pick);
  maintenance(servo_volumes);
}