// remove-dups.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "trie.h"

class TimeIt
{
  public:
  	TimeIt(const char* msg)
		{
			_msg = msg;
			_start = Clock::now();
		}
	~TimeIt()
		{
			_end = Clock::now();
			std::cout << msg
					  << ": "
					  << std::chrono::duration_cast<std::chrono::nanoseconds(_end - _start).count()
					  << "ns.\n";
		}

  private:
  	const char* _msg;
	Clock _start;
	Clock _end;
};

class TrieTester : public Tester
{
  public:
  	TrieTester(uint32*, size_t) {}

	virtual void insert(uint32* up) { _t.insert(*up); }
	virtual bool test(uint32* up) { return _t.test(*up); }

  private:
  	Trie _t;
};

int uint32cmp(const void* v1, const void* v2)
{
	const uint32* u1 = static_cast<const uint32*>(v1);
	const uint32* u2 = static_cast<const uint32*>(v2);

	return *u1 == *u2 ? 0 : (*u1 < *u2 ? -1 : 1);
}

class SortTester : public Tester
{
  public:
  	SortTester(uint32* a, size_t n) : _a(a)
		{
			qsort(_a, n, sizeof(uint32), uint32cmp);
		}

	virtual void insert(uint32* up) {}
	virtual bool test(uint32* up) { if (up == _a) return false; return up[0] == up[-1]; }

  private:
  	uint32* _a;
  	
};

class SquareTester : public Tester
{
  public:
  	SquareTester(uint32* a, size_t) : _a(a), _e(a) {}

	virtual void insert(uint32* up) { *_e++ = *up; }
	virtual bool test(uint32* up)
		{
			for (uint32* p = _a; p < _e; p++)
				if (*p == *up) return true;
			return false;
		}

  private:
  	uint32* _a;
	uint32* _e;
};

class SetTester : public Tester
{
  public:
  	SetTester(uint32*, size_t n) : _set(n) {}

	virtual void insert(uint32* up) { if (!test(up)) _set.insert(*up); }
	virtual bool test(uint32* up) { return _set.count(*up) == 0; }

  private:
  	std::unordered_set<uint32> _set;
};

class BaselineTester : public Tester
{
  public:
  	BaselineTester(uint32*, size_t) {}

	virtual void insert(uint32*) {}
	virtual bool test(uint32*) { return false; }
};

void RunTest(Tester& t, uint32* a, size_t n)
{
	uint32 *p1 = a;
	uint32 *p2 = a;
	uint32 *e = a + n;

	for (; p2 < e; p2++)
	{
		if (! t.test(p2))
		{
			t.insert(p2);
			*p1++ = *p2;
		}
	}
}

void TrieTest(uint32* a, size_t n)
{
	TrieTester t(a, n);

	RunTest(t, a, n);
}

void SortTest(uint32* a, size_t n)
{
	SortTester t(a, n);

	RunTest(t, a, n);
}

void SquareTest(uint32* a, size_t n)
{
	SquareTester t(a, n);

	RunTest(t, a, n);
}

void SetTest(uint32* a, size_t n)
{
	SetTester t(a, n);

	RunTest(t, a, n);
}


void BaselineTest(uint32* a, size_t n)
{
	BaselineTester t(a, n);

	RunTest(t, a, n);
}

void AllTest(uint32* a, size_t n, size_t u)
{
	cout << "Array size: " << n;
	cout << "Unique size: " << u;
	{ TimeIt t("Baseline"); BaselineTest(a, n); }
	{ TimeIt t("Sort"); SortTest(a, n); }
	{ TimeIt t("NSquare"); SquareTest(a, n); }
	{ TimeIt t("Trie"); TrieTest(a, n); }
	{ TimeIt t("std.Set"); SetTest(a, n); }
}

enum FillAlgorithm { Random, Linear };			// Fill range with random numbers or linear and repeat?
enum RangeAlgorithm { Random, Linear, Unique }; // Does range over uint32 range from 1-max, 1-n or 1-u?

void FillArray(uint32* a, size_t n, size_t u, FillAgorithm fill, RangeAlgorithm range)


int main()
{
	Trie	t;

	t.insert(934);
	if (t.test(934)) printf("works");
    return 0;
}

