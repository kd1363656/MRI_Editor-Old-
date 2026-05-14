#pragma once

struct ComponentEventTagBase : TagBase {};

// 攻撃タグ
struct AtkComboStartActiveTag  : ComponentEventTagBase {};
struct AtkComboFirstActiveTag  : ComponentEventTagBase {};
struct AtkComboSecondActiveTag : ComponentEventTagBase {};
struct AtkComboThirdActiveTag  : ComponentEventTagBase {};
struct AtkComboFourthActiveTag : ComponentEventTagBase {};

// 移動タグ
struct MoveInputBasedOnTargetActiveTag : ComponentEventTagBase {};

struct MoveInputAtkComboFirstActiveTag  : ComponentEventTagBase {};
struct MoveInputAtkComboSecondActiveTag : ComponentEventTagBase {};
struct MoveInputAtkComboThirdActiveTag  : ComponentEventTagBase {};
struct MoveInputAtkComboFourthActiveTag : ComponentEventTagBase {};

// 回転タグ
struct RotationInputSmoothBasedOnTargetActiveTag  : ComponentEventTagBase {};
struct RotationInputInstantBasedOnTargetActiveTag : ComponentEventTagBase {};