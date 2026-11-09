#pragma once

/**
 * @file PlayerActionType.h
 * @brief Playerが使用できる共通Action種別
 */

#include <cstdint>

 /**
  * @brief Player Action種別
  *
  * @details
  * 全職業で共通して6種類持つ。
  *
  * Squad操作はActionとは別系統で扱う。
  */
enum class PlayerActionType : uint8_t
{
	Move,

	NormalAttack,

	Skill1,

	Skill2,

	Ultimate,

	Item
};