#pragma once

class NonMovable
{
protected:
    NonMovable() = default;
	virtual ~NonMovable() = default;
public:
	NonMovable(NonMovable &&) noexcept = delete;
	NonMovable &operator=(NonMovable &&) noexcept = delete;
};