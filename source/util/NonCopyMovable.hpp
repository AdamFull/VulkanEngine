#pragma once

class NonCopyMovable
{
protected:
    NonCopyMovable() = default;
	virtual ~NonCopyMovable() = default;
public:
    NonCopyMovable(const NonCopyMovable &) = delete;
	NonCopyMovable(NonCopyMovable &&) noexcept = delete;
	NonCopyMovable &operator=(const NonCopyMovable &) = delete;
	NonCopyMovable &operator=(NonCopyMovable &&) noexcept = delete;
};