// remove-dups.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "trie.h"

class TimeIt
{
  public:
  	TimeIt()
		{
			_start = Clock::now();
			_end = _start;
		}
	~TimeIt() { }

	void start()
		{
			_start = Clock::now();
		}

	void end()
		{
			_end = Clock::now();
		}

	_int64 micro()
		{
		  	return std::chrono::duration_cast<std::chrono::microseconds>(_end - _start).count();
		}


  private:
	ClockValue _start;
	ClockValue _end;
};

class Tester
{
public:
	virtual const char* name() = 0;
	virtual void init(uint32* a, size_t n) = 0;
	virtual void empty() = 0;
	virtual void insert(uint32* pu) = 0;
	virtual bool test(uint32* pu) = 0;
	virtual void validate(bool b) { assert(b); }

	void run(uint32* a, size_t n, size_t u)
		{
			uint32 *p1 = a;
			uint32 *p2 = a;
			uint32 *e = a + n;

			init(a, n);
			for (; p2 < e; p2++)
			{
				if (! test(p2))
				{
					insert(p2);
					*p1++ = *p2;
				}
			}
			validate(p1 - a == u);
		}
};

typedef Tester* TesterP;

class TrieTester : public Tester
{
  public:
  	TrieTester() : _pt(0) {}
	~TrieTester() { delete _pt; }

	virtual const char* name() { return "trie"; }
  	virtual void init(uint32*, size_t) { delete _pt; _pt = new Trie; }
	virtual void empty() { delete _pt; _pt = 0; }
	virtual void insert(uint32* up) { _pt->insert(*up); }
	virtual bool test(uint32* up) { return _pt->test(*up); }

  private:
  	Trie* _pt;
};

class Trie16Tester : public Tester
{
  public:
  	Trie16Tester() : _pt(0) {}
	~Trie16Tester() { delete _pt; }

	virtual const char* name() { return "trie16"; }
  	virtual void init(uint32*, size_t) { delete _pt; _pt = new Trie16; }
	virtual void empty() { delete _pt; _pt = 0; }
	virtual void insert(uint32* up) { _pt->insert(*up); }
	virtual bool test(uint32* up) { return _pt->test(*up); }

  private:
  	Trie16* _pt;
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
	  SortTester() : _a(0) {}

	virtual const char* name() { return "sort"; }
	virtual void init(uint32* a, size_t n) { _a = a; qsort(_a, n, sizeof(uint32), uint32cmp); }
	virtual void empty() { _a = 0; }
	virtual void insert(uint32* up) {}
	virtual bool test(uint32* up) { if (up == _a) return false; return up[0] == up[-1]; }

  private:
  	uint32* _a;
  	
};

class SquareTester : public Tester
{
  public:
  	SquareTester() : _a(0), _e(0) {}

	virtual const char* name() { return "nsquare"; }
	virtual void init(uint32* a, size_t) { _a = a; _e = a; }
	virtual void empty() { _a = 0; _e = 0; }
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
  	SetTester() : _pset(0) {}
	~SetTester() { delete _pset; }

	virtual const char* name() { return "set"; }
	virtual void init(uint32*, size_t n) { delete _pset; _pset = new std::unordered_set<uint32>(n); }
	virtual void empty() { delete _pset; _pset = 0; }
	virtual void insert(uint32* up) { if (!test(up)) _pset->insert(*up); }
	virtual bool test(uint32* up) { return _pset->count(*up) != 0; }

  private:
  	std::unordered_set<uint32>* _pset;
};

class BaselineTester : public Tester
{
  public:
  	BaselineTester() {}

	virtual const char* name() { return "baseline"; }
	virtual void init(uint32*,size_t) {}
	virtual void empty() { }
	virtual void insert(uint32*) {}
	virtual bool test(uint32*) { return false; }
	virtual void validate(bool b) {}
};


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
			{
				Trie t;	// make sure random values are not duplicated

				for (size_t i = 0; i < u; i++)
				{
					uint32 x = s_dist(s_gen);
					if (t.test(x)) { i--; continue; }
					t.insert(x);
					a[i] = x;
				}
			}
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
#define TEST_TRIE16		(1<<5)
#define TEST_ALL		(TEST_BASELINE|TEST_SORT|TEST_NSQUARE|TEST_TRIE|TEST_SET|TEST_TRIE16)
#define NTESTS			6

int usage()
{
	std::cerr << "usage: remove-dups [-c runs] [-n count] [-u count] [-s] [-r] [-l] [-t (all | baseline | sort | nsquare | trie | set | trie16)]\n";
	return 1;
}

size_t GetWorkingSet()
{
	size_t nWS = 0;
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
	if (hProcess != NULL)
	{
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
			nWS = pmc.WorkingSetSize;
		CloseHandle(hProcess);
	}
	return nWS;
}

int main(int argc, const char* argv[])
{
	size_t nsizes = NSIZES;
	size_t nuniques = NUNIQUES;
	size_t tests = TEST_ALL;
	size_t nalgos = NALGOS;
	size_t nruns = 0;
	bool bFirstTest = true;
	bool bShuffle = false;
	TesterP testers[NTESTS] = { };
	TimeIt timeit;

	for (argc--, argv++; argc > 0; argc--, argv++)
	{
		const char* arg = *argv;
		if (*arg == '-')
		{
			switch (arg[1])
			{
				case 'c':
					if (arg[2] == 0)
						{ argc--,argv++,arg=*argv; nruns = atoi(arg); }
					else
						nruns = atoi(arg+2);
					break;
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
					else if (_stricmp(arg, "trie16") == 0)
						tests |= TEST_TRIE16;
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
	size_t ntests = 0;
	if (tests & TEST_BASELINE)
		testers[ntests++] = new BaselineTester();
	if (tests & TEST_SORT)
		testers[ntests++] = new SortTester();
	if (tests & TEST_NSQUARE)
		testers[ntests++] = new SquareTester();
	if (tests & TEST_TRIE)
		testers[ntests++] = new TrieTester();
	if (tests & TEST_TRIE16)
		testers[ntests++] = new Trie16Tester();
	if (tests & TEST_SET)
		testers[ntests++] = new SetTester();

	// The Column Headers
	std::cout << "Algo,Size,";
	size_t j = 0;
	if (nruns == 1)
		j = nuniques-1;
	for (; j < nuniques; j++)
		for (size_t k = 0; k < nalgos; k++)
			std::cout << "U" << UniqueCounts[j] << "/" << ((k == 0) ? "L," : "R,");
	std::cout << "WS";
	std::cout << "\n";
	size_t nWS = 0;

	// The data
	for (size_t t = 0; t < ntests; t++)
	{
		TesterP tp = testers[t];
		size_t i = 0;
		if (nruns == 1)
			i = nsizes-1;
		for (; i < nsizes; i++)
		{
			size_t n = SizeCounts[i];
			std::cout << tp->name() << "," << n << ",";

			size_t j = 0;
			if (nruns == 1)
				j = nuniques-1;
			for (; j < nuniques; j++)
			{
				size_t u = UniqueCounts[j];

				for (size_t k = 0; k < nalgos; k++)
				{
					if (u <= n)
					{
						UniqueAlgorithm algo = Algos[k];
						uint32* a = new uint32[n];

						FillArray(a, n, u, algo, bShuffle);
						timeit.start();
						tp->init(a, n);
						tp->run(a, n, u);
						nWS = GetWorkingSet();
						tp->empty();
						timeit.end();

						delete[] a;
						std::cout << timeit.micro() << ",";
					}
					else
						std::cout << "_,";
				}
			}
			std::cout << nWS;
			std::cout << "\n";
		}
	}


    return 0;
}

