#pragma once

enum class [[generate::to_string]] ETest
{
	First = 1,
	Second = 1 << 3,
	Last = 4000
};
