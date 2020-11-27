/**
 * @file irReceiver.hpp
 * @brief 赤外線受信モジュール用クラスヘッダ
 */

#ifndef IR_RECEIVER_HPP
#define IR_RECEIVER_HPP

#include <Arduino.h>

/**
 * @class IrReceiver
 * @brief 赤外線受信モジュール用クラス
 * @attention 本クラスのメンバ関数を使用する前にWire.begin()を実行しておくこと
 */
class IrReceiver
{
private:
  uint8_t _i2c_address = 8; //  赤外線受信モジュールのI2Cスレーブアドレス

public:
  IrReceiver() {}
  //! id = 0~15、赤外線受信モジュールのロータリースイッチの値と等しくする。
  IrReceiver(uint8_t id) { _i2c_address = id + 8; }
  ~IrReceiver() {}
  byte read() const;
  bool is_connected(void) const;
};

#endif