#pragma once

class DisableCopy {
public:
    DisableCopy(const DisableCopy&) = delete;
    DisableCopy& operator=(const DisableCopy&) = delete;
    DisableCopy() = default;
};
