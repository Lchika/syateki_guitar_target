#ifndef FULL_COLOR_LED_HIT_REACTOR_HPP
#define FULL_COLOR_LED_HIT_REACTOR_HPP

#include <memory>
#include <map>
#include <target.hpp>
#include <ht16k33LED.hpp>

class FullColorLedHitReactor : public target::IHitReactor
{
public:
  //! idはHT16K33LED基板上のチャンネル番号(1-5)を指定する。
  FullColorLedHitReactor(int id, int address = 0x70, bool do_wire_begin = false)
    :_address(address), _do_wire_begin(do_wire_begin)
  {
    if (id < 1 || id > 5)
    {
      id = 1;
    }
    _id = id;
  }
  void active(int gunId) override
  {
    _led->write_color(_gunId2Color(gunId));
  }
  void inactive() override
  {
    _led->clear();
  }
  void hit(int gunId) override
  {
    const int time = 100;
    const int count = 3;
    ht16k33LED::Color color = _gunId2Color(gunId);
    for (int i = 0; i < count; i++)
    {
      _led->write_color(color);
      delay(time);
      _led->clear();
      delay(time);
    }
  }
  bool maintenance() override
  {
    unsigned count = 0;
    for (int i = 0; i < 3; i++)
    {
      if (count % 3 == 0)
      {
        _led->write_color(ht16k33LED::red);
      }
      else if (count % 3 == 1)
      {
        _led->write_color(ht16k33LED::green);
      }
      else
      {
        _led->write_color(ht16k33LED::blue);
      }
      count++;
      delay(1000);
    }
    return true;
  }
  bool init() override
  {
    // M5.begin()前にI2C処理をすると上手くいかないようなので、
    // 呼び出しタイミング制御できるようにinit()関数の中で初期化する。
    _led.reset(new ht16k33LED::Led(this->_id - 1, this->_address, this->_do_wire_begin));
    _led->init();
    return true;
  }

private:
  int _id = 0;
  int _address = 0x70;
  int _do_wire_begin = false;
  std::unique_ptr<ht16k33LED::Led> _led;
  ht16k33LED::Color _gunId2Color(int gunId)
  {
    std::map<int, ht16k33LED::Color> dict{
        {1, ht16k33LED::Color::red},
        {2, ht16k33LED::Color::blue}};
    try
    {
      return dict.at(gunId);
    }
    catch (std::out_of_range &)
    {
    }
    return ht16k33LED::Color::clear;
  }
};


#endif