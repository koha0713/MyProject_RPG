#pragma once
#include <cstdint>

/**
 * @class IScene
 * @brief シーンインターフェース
 */
class IScene {
public:
	IScene() = default;
	virtual ~IScene() = default;
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Update(uint64_t delta) = 0;
	virtual void Draw(uint64_t delta) = 0;
};
