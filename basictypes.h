#pragma once

#include "stdafx.h"

typedef unsigned _int8 uint8;
typedef unsigned _int32 uint32;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<Clock> ClockValue;


class Tester
{
public:
	virtual void insert(uint32* pu) = 0;
	virtual bool test(uint32* pu) = 0;
};
