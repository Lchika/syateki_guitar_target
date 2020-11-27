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
  FullColorLedHitReactor(int id)
  {
    if (id < 1 || id > 5)
    {
      id = 1;
    }
    _led.reset(new ht16k33LED::Led(id - 1));
    _led->init();
    _led->clear();
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

private:
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