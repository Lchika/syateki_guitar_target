#ifndef TARGET_HPP
#define TARGET_HPP

#include <memory>

namespace target
{

  class IHitReactor
  {
  public:
    virtual ~IHitReactor() {}
    virtual void active(int gun_id) = 0;
    virtual void inactive() = 0;
    virtual void hit(int gun_id) = 0;
    virtual bool maintenance() = 0;
    virtual bool init() = 0;
  };

  class IRayDetector
  {
  public:
    virtual ~IRayDetector() {}
    virtual int detect() = 0;
    virtual bool init() = 0;
  };

  class Target
  {
  public:
    Target(std::unique_ptr<IHitReactor> hitReactor, std::unique_ptr<IRayDetector> rayDetector)
        : _hitReactor(std::move(hitReactor)), _rayDetector(std::move(rayDetector))
    {
    }
    int update()
    {
      int recieved_gun_id = _rayDetector->detect();
      if (recieved_gun_id > 0)
      {
        _hitReactor->active(recieved_gun_id);
      }
      else
      {
        _hitReactor->inactive();
      }
      return recieved_gun_id;
    }
    int shot(int shot_gun_id)
    {
      int recieved_gun_id = _rayDetector->detect();
      if (shot_gun_id == recieved_gun_id)
      {
        _hitReactor->hit(shot_gun_id);
      }
      return recieved_gun_id;
    }
    bool maintenance()
    {
      return _hitReactor->maintenance();
    }
    // M5Stackの場合、I2C関係はM5.begin()の後でないとうまく動かないようだ。
    // I2C関係や内容をシリアルに出したい処理はここに書く。
    bool initialize()
    {
      if (!_hitReactor->init())
      {
        Serial.println("faild to initialize hitReactor");
        return false;
      }
      if (!_rayDetector->init())
      {
        Serial.println("faild to initialize rayDetector");
        return false;
      }
      return true;
    }

  private:
    std::unique_ptr<IHitReactor> _hitReactor;
    std::unique_ptr<IRayDetector> _rayDetector;
  };

} // namespace target

#endif