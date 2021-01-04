#ifndef IR_RAY_DETECTOR_HPP
#define IR_RAY_DETECTOR_HPP

#include <memory>
#include <target.hpp>
#include <irReceiver.hpp>

class IrRayDetector : public target::IRayDetector
{
public:
  //! idは赤外線受信モジュールのロータリースイッチの値(0-15)を設定する。
  IrRayDetector(int id) : _irReciever(new IrReceiver(id)) {}
  int detect() override
  {
    return _irReciever->read();
  }
  bool init() override
  {
    // 特に初期化処理はなし
    return true;
  }

private:
  std::unique_ptr<IrReceiver> _irReciever;
};

#endif