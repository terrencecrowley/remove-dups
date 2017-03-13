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
		}
	void End()
		{
			_end = Clock::now();
			std::cout << _msg
					  << ","
					  << std::chrono::duration_cast<std::chrono::nanoseconds>(_end - _start).count()
					  << ",";
		}


  private:
  	const char* _msg;
	ClockValue _start;
	ClockValue _end;
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
	virtual bool test(uint32* up) { return _set.count(*up) != 0; }

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

void RunTest(Tester& t, uint32* a, size_t n, size_t u)
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
	assert(p1 - a == u);
}

void TrieTest(uint32* a, size_t n, size_t u)
{
	TrieTester t(a, n);

	RunTest(t, a, n, u);
}

void SortTest(uint32* a, size_t n, size_t u)
{
	SortTester t(a, n);

	RunTest(t, a, n, u);
}

void SquareTest(uint32* a, size_t n, size_t u)
{
	SquareTester t(a, n);

	RunTest(t, a, n, u);
}

void SetTest(uint32* a, size_t n, size_t u)
{
	SetTester t(a, n);

	RunTest(t, a, n, u);
}


void BaselineTest(uint32* a, size_t n, size_t u)
{
	BaselineTester t(a, n);

	RunTest(t, a, n, u);
}

enum class UniqueAlgorithm { Random, Linear };		// Fill range with random numbers or linear and repeat?

static std::random_device s_rd;
static std::mt19937_64 s_gen(s_rd());
static std::uniform_int_distribution<unsigned int> s_dist(0, 0xFFFFFFFF);

size_t RandomInRange(size_t n)
{
	std::uniform_int_distribution<unsigned int> dist(0, n);

	return dist(s_gen);
}


void KnuthShuffle(uint32* a, size_t n)
{
	for (size_t i = n-1; i > 0; i--)
	{
		size_t swapWith = RandomInRange(i);
		uint32 t = a[i];
		a[i] = a[swapWith];
		a[swapWith] = t;
	}
}


void FillArray(uint32* a, size_t n, size_t u, UniqueAlgorithm fill, bool bShuffle)
{
	// First generate unique values
	switch (fill)
	{
		case UniqueAlgorithm::Random:
			for (size_t i = 0; i < u; i++)
				a[i] = s_dist(s_gen);
			break;
		case UniqueAlgorithm::Linear:
			for (size_t i = 0; i < u; i++)
				a[i] = i;
			break;
	}

	// Now duplicate into rest of array
	for (size_t i = u; i < n;)
		for (size_t j = 0; j < u && i < n; j++, i++)
			a[i] = a[j];

	if (bShuffle)
		KnuthShuffle(a, n);
}


void PrintHeader()
{
	std::cout << "Algo,NSecs,NSize,NUnique,UniqueAlgorithm\n";
}

void PrintFill(size_t n, size_t u, UniqueAlgorithm fill)
{
	std::cout << n << "," << u << "," << int(fill) << "\n";

}


size_t SizeCounts[] = { 1 << 4, 1 << 6, 1 << 8, 1 << 10, 1 << 12, 1 << 14, 1 << 16, 1 << 18, 1 << 20, 1 << 22 };
#define NSIZES (sizeof(SizeCounts)/sizeof(SizeCounts[0]))
size_t UniqueCounts[] = { 1 << 4, 1 << 6, 1 << 8, 1 << 10, 1 << 12, 1 << 14, 1 << 16, 1 << 18, 1 << 20, 1 << 22 };
#define NUNIQUES (sizeof(UniqueCounts)/sizeof(UniqueCounts[0]))
UniqueAlgorithm Algos[] = { UniqueAlgorithm::Linear, UniqueAlgorithm::Random };
#define NALGOS (sizeof(Algos)/sizeof(Algos[0]))

// Bit flags for which algorithms to test
#define TEST_NONE		0
#define TEST_BASELINE	(1<<0)
#define TEST_SORT		(1<<1)
#define TEST_NSQUARE	(1<<2)
#define TEST_TRIE		(1<<3)
#define TEST_SET		(1<<4)
#define TEST_ALL		(TEST_BASELINE|TEST_SORT|TEST_NSQUARE|TEST_TRIE|TEST_SET)

int usage()
{
	std::cerr << "usage: remove-dups [-n count] [-u count] [-s] [-r] [-l] [-t (all | baseline | sort | nsquare | trie | set)]\n";
	return 1;
}

int main(int argc, const char* argv[])
{
	size_t nsizes = NSIZES;
	size_t nuniques = NUNIQUES;
	size_t tests = TEST_ALL;
	size_t nalgos = NALGOS;
	bool bFirstTest = true;
	bool bShuffle = false;

	for (argc--, argv++; argc > 0; argc--, argv++)
	{
		const char* arg = *argv;
		if (*arg == '-')
		{
			switch (arg[1])
			{
				case 'n':
					if (arg[2] == 0)
						{ argc--,argv++,arg=*argv; nsizes = atoi(arg); }
					else
						nsizes = atoi(arg+2);
					break;
				case 'u':
					if (arg[2] == 0)
						{ argc--,argv++,arg=*argv; nuniques = atoi(arg); }
					else
						nuniques = atoi(arg+2);
					break;
				case 'r':
					nalgos = 2;
					break;
				case 'l':
					nalgos = 1;
					break;
				case 's':
					bShuffle = true;
					break;
				case 't':
					if (bFirstTest)
						{ bFirstTest = false; tests = TEST_NONE; }
					if (arg[2] == 0)
						{ argc--,argv++,arg=*argv; }
					else
						arg += 2;
					if (_stricmp(arg, "all") == 0)
						tests |= TEST_ALL;
					else if (_stricmp(arg, "baseline") == 0)
						tests |= TEST_BASELINE;
					else if (_stricmp(arg, "sort") == 0)
						tests |= TEST_SORT;
					else if (_stricmp(arg, "nsquare") == 0)
						tests |= TEST_NSQUARE;
					else if (_stricmp(arg, "trie") == 0)
						tests |= TEST_TRIE;
					else if (_stricmp(arg, "set") == 0)
						tests |= TEST_SET;
					break;
				default:
					return usage();
			}
		}
	}
	if (nsizes > NSIZES)
		return usage();
	if (nuniques > NUNIQUES)
		return usage();

	PrintHeader();
	for (size_t i = 0; i < nsizes; i++)
		for (size_t j = 0; j < nuniques; j++)
			for (size_t k = 0; k < nalgos; k++)
			{
				size_t n = SizeCounts[i];
				size_t u = UniqueCounts[j];
				if (u > n) continue;
				UniqueAlgorithm algo = Algos[k];
				uint32* a = new uint32[n];
				if (tests & TEST_BASELINE)
				{
					FillArray(a, n, u, algo, bShuffle);
					{ TimeIt t("Baseline"); BaselineTest(a, n, n); t.End(); PrintFill(n, u, algo); }
				}
				if (tests & TEST_SORT)
				{
					FillArray(a, n, u, algo, bShuffle);
					{ TimeIt t("Sort"); SortTest(a, n, u); t.End(); PrintFill(n, u, algo); }
				}
				if (tests & TEST_NSQUARE)
				{
					FillArray(a, n, u, algo, bShuffle);
					{ TimeIt t("NSquare"); SquareTest(a, n, u); t.End(); PrintFill(n, u, algo); }
				}
				if (tests & TEST_TRIE)
				{
					FillArray(a, n, u, algo, bShuffle);
					{ TimeIt t("Trie"); TrieTest(a, n, u); t.End(); PrintFill(n, u, algo); }
				}
				if (tests & TEST_SET)
				{
					FillArray(a, n, u, algo, bShuffle);
					{ TimeIt t("Set"); SetTest(a, n, u); t.End(); PrintFill(n, u, algo); }
				}
				delete[] a;
			}
    return 0;
}

