#pragma once

/**
 * @file SoundCategory.h
 * @brief Soundの用途分類
 */

 /**
  * @brief Sound Category
  *
  * @details
  * 再生方法ではなく用途によって分類する。
  *
  * BGM     : 楽曲
  * SE      : 攻撃音、足音、UI音など
  * Ambient : 風、川、焚き火などの環境音
  */
enum class SoundCategory
{
	BGM = 0,
	SE,
	Ambient,

	Count
};