#pragma once

//=========================
// コピー禁止クラス
//=========================
class NonCopyable
{
protected:
    //====================
    // ライフサイクル
	//====================

    /**
    * @brief コンストラクタ
	*/
    NonCopyable() = default;

	/**
    * @brief デストラクタ
    */
    ~NonCopyable() = default;

	//====================
	// コピー禁止
	//====================
    /**
    * @brief コピーコンストラクタ（禁止）
	* @detail コピーコンストラクタを削除して子クラスのコピーを禁止
    */
    NonCopyable(const NonCopyable&) = delete;

    /**
	* @brief コピー代入演算子（禁止）
    * @detail コピー代入演算子を削除して子クラスのコピーを禁止
    */
    NonCopyable& operator=(const NonCopyable&) = delete;

};