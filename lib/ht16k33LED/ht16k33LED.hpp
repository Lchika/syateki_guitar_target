/**
 * @brief ht16K33 フルカラーLED制御クラス
 */

#ifndef HT16K33LED_HPP
#define HT16K33LED_HPP

#include <array>

namespace ht16k33LED
{

enum Color{
  clear = (0),
  red,
  green,
  blue
};

/**
 * @class Led
 * @brief
 */
class Led {
private:
  static bool _initialized;
  uint8_t _address = 0;
  uint8_t _id = 0;
  std::array<uint8_t, 3> _color_array(Color color) const;

public:
  //! idは0始まりで指定する
  Led(uint8_t id, uint8_t adress = 0x70, bool do_wire_begin = true);
  void init();
  //! r, g, bは0x00(OFF)か0x01(ON)で指定する
  void write_rgb(uint8_t r, uint8_t g, uint8_t b);
  void write_row(uint8_t com, uint8_t row1, uint8_t row2);
  void write_color(Color color);
  void clear();
};

}// namespace HT16K33LED
#endif