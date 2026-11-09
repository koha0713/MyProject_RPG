#include "RadialMenuUI.h"

#include "GameObject.h"
#include "GameObjectManager.h"

#include "UIRectComponent.h"
#include "UIFontComponent.h"

//=====================================================
// Initialize
//=====================================================

void RadialMenuUI::Initialize(
	GameObjectManager& manager)
{
	m_Manager =
		&manager;

	CreateActionTexts();

	SetVisible(
		false);
}

//=====================================================
// Finalize
//=====================================================

void RadialMenuUI::Finalize()
{
	m_MoveText =
		nullptr;

	m_NormalAttackText =
		nullptr;

	m_Skill1Text =
		nullptr;

	m_Skill2Text =
		nullptr;

	m_UltimateText =
		nullptr;

	m_ItemText =
		nullptr;

	m_CenterText =
		nullptr;

	m_Manager =
		nullptr;

	m_Visible =
		false;

	m_HasHighlight =
		false;
}

//=====================================================
// Create
//=====================================================

void RadialMenuUI::CreateActionTexts()
{
	m_MoveText =
		CreateText(
			"RadialMove",
			L"移動");

	m_NormalAttackText =
		CreateText(
			"RadialAttack",
			L"攻撃");

	m_Skill1Text =
		CreateText(
			"RadialSkill1",
			L"Skill 1");

	m_Skill2Text =
		CreateText(
			"RadialSkill2",
			L"Skill 2");

	m_UltimateText =
		CreateText(
			"RadialUltimate",
			L"Ultimate");

	m_ItemText =
		CreateText(
			"RadialItem",
			L"Item");

	m_CenterText =
		CreateText(
			"RadialCenter",
			L"Cancel");
}

//=====================================================
// Create Text
//=====================================================

GameObject* RadialMenuUI::CreateText(
	const char* name,
	const wchar_t* text)
{
	if (!m_Manager)
	{
		return nullptr;
	}

	GameObject* object =
		m_Manager->
		Create<GameObject>();

	if (!object)
	{
		return nullptr;
	}

	object->SetName(
		name);

	object->SetTag(
		Tag::Font);

	auto* rect =
		object->AddComponent<
		UIRectComponent>();

	auto* font =
		object->AddComponent<
		UIFontComponent>();

	if (rect)
	{
		rect->SetSize(
			110.0f,
			32.0f);
	}

	if (font)
	{
		font->SetText(
			text);

		font->SetFontSize(
			20.0f);

		font->SetColor(
			Color(
				1.0f,
				1.0f,
				1.0f,
				1.0f));
	}

	return object;
}

//=====================================================
// Visible
//=====================================================

void RadialMenuUI::SetVisible(
	bool visible)
{
	m_Visible =
		visible;

	if (!visible)
	{
		HideObject(
			m_MoveText);

		HideObject(
			m_NormalAttackText);

		HideObject(
			m_Skill1Text);

		HideObject(
			m_Skill2Text);

		HideObject(
			m_UltimateText);

		HideObject(
			m_ItemText);

		HideObject(
			m_CenterText);

		return;
	}

	UpdateLayout();

	UpdateHighlightVisual();
}

//=====================================================
// Center
//=====================================================

void RadialMenuUI::SetCenter(
	float x,
	float y)
{
	m_CenterX =
		x;

	m_CenterY =
		y;

	if (m_Visible)
	{
		UpdateLayout();
	}
}

//=====================================================
// Highlight
//=====================================================

void RadialMenuUI::SetHighlightedAction(
	PlayerActionType type)
{
	m_HasHighlight =
		true;

	m_HighlightedAction =
		type;

	if (m_Visible)
	{
		UpdateHighlightVisual();
	}
}

//=====================================================
// Clear Highlight
//=====================================================

void RadialMenuUI::ClearHighlight()
{
	m_HasHighlight =
		false;

	if (m_Visible)
	{
		UpdateHighlightVisual();
	}
}

//=====================================================
// Layout
//=====================================================

void RadialMenuUI::UpdateLayout()
{
	if (!m_Visible)
	{
		return;
	}

	//=================================================
	// 6方向配置
	//=================================================
	//
	//            Move
	//
	//     Item          Attack
	//
	//
	//   Ultimate       Skill1
	//
	//           Skill2
	//
	// 操作感を見て後で配置変更可能。
	//=================================================

	struct LayoutEntry
	{
		GameObject* Object;

		float OffsetX;

		float OffsetY;
	};

	const float halfRadius =
		m_Radius * 0.866f;

	const LayoutEntry entries[] =
	{
		{
			m_MoveText,
			0.0f,
			-m_Radius
		},
		{
			m_NormalAttackText,
			halfRadius,
			-m_Radius * 0.5f
		},
		{
			m_Skill1Text,
			halfRadius,
			m_Radius * 0.5f
		},
		{
			m_Skill2Text,
			0.0f,
			m_Radius
		},
		{
			m_UltimateText,
			-halfRadius,
			m_Radius * 0.5f
		},
		{
			m_ItemText,
			-halfRadius,
			-m_Radius * 0.5f
		}
	};

	for (const LayoutEntry& entry :
		entries)
	{
		if (!entry.Object)
		{
			continue;
		}

		auto* rect =
			entry.Object->
			GetComponent<
			UIRectComponent>();

		if (!rect)
		{
			continue;
		}

		// Textの左上位置なので、
		// おおよそ中央合わせする。
		rect->SetPosition(
			m_CenterX +
			entry.OffsetX -
			55.0f,

			m_CenterY +
			entry.OffsetY -
			16.0f);

		rect->SetSize(
			110.0f,
			32.0f);
	}

	//=================================================
	// Dead Zone Center
	//=================================================

	if (m_CenterText)
	{
		auto* rect =
			m_CenterText->
			GetComponent<
			UIRectComponent>();

		if (rect)
		{
			rect->SetPosition(
				m_CenterX -
				55.0f,
				m_CenterY -
				16.0f);

			rect->SetSize(
				110.0f,
				32.0f);
		}
	}
}

//=====================================================
// Highlight Visual
//=====================================================

void RadialMenuUI::UpdateHighlightVisual()
{
	const PlayerActionType actionTypes[] =
	{
		PlayerActionType::Move,
		PlayerActionType::NormalAttack,
		PlayerActionType::Skill1,
		PlayerActionType::Skill2,
		PlayerActionType::Ultimate,
		PlayerActionType::Item
	};

	for (PlayerActionType type :
	actionTypes)
	{
		GameObject* object =
			GetActionObject(
				type);

		if (!object)
		{
			continue;
		}

		auto* font =
			object->
			GetComponent<
			UIFontComponent>();

		if (!font)
		{
			continue;
		}

		const bool highlighted =
			m_HasHighlight &&
			m_HighlightedAction ==
			type;

		if (highlighted)
		{
			// 選択候補を強調。
			font->SetColor(
				Color(
					1.0f,
					0.85f,
					0.25f,
					1.0f));

			font->SetFontSize(
				26.0f);
		}
		else
		{
			font->SetColor(
				Color(
					1.0f,
					1.0f,
					1.0f,
					1.0f));

			font->SetFontSize(
				20.0f);
		}
	}

	// Dead Zone内ならCancel文字を強調。
	if (m_CenterText)
	{
		auto* font =
			m_CenterText->
			GetComponent<
			UIFontComponent>();

		if (font)
		{
			if (!m_HasHighlight)
			{
				font->SetColor(
					Color(
						1.0f,
						0.7f,
						0.7f,
						1.0f));
			}
			else
			{
				font->SetColor(
					Color(
						0.65f,
						0.65f,
						0.65f,
						1.0f));
			}
		}
	}
}

//=====================================================
// Get Action Object
//=====================================================

GameObject* RadialMenuUI::GetActionObject(
	PlayerActionType type) const
{
	switch (type)
	{
	case PlayerActionType::Move:

		return m_MoveText;

	case PlayerActionType::NormalAttack:

		return m_NormalAttackText;

	case PlayerActionType::Skill1:

		return m_Skill1Text;

	case PlayerActionType::Skill2:

		return m_Skill2Text;

	case PlayerActionType::Ultimate:

		return m_UltimateText;

	case PlayerActionType::Item:

		return m_ItemText;

	default:

		return nullptr;
	}
}

//=====================================================
// Hide Object
//=====================================================

void RadialMenuUI::HideObject(
	GameObject* object)
{
	if (!object)
	{
		return;
	}

	auto* rect =
		object->
		GetComponent<
		UIRectComponent>();

	if (!rect)
	{
		return;
	}

	rect->SetSize(
		0.0f,
		0.0f);
}