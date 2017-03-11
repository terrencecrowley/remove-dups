#pragma once

#include "stdafx.h"
#include "basictypes.h"


typedef union
{
	uint32 _u32;
	uint8 _u8[4];
} TrieBytes;

class TrieBrancher
{
  public:
  	TrieBrancher() {}
	virtual ~TrieBrancher() {}
};

typedef TrieBrancher* TrieBrancherP;

class TrieLeaf : public TrieBrancher
{
  public:
  	TrieLeaf() : _v(256) {}
	~TrieLeaf() {}

  	void insert(uint8 u) { _v[u] = true; }
  	bool test(uint8 u) { return _v[u]; }

  private:
  	std::vector<bool>	_v;
};

typedef TrieLeaf* TrieLeafP;
class TrieBranch;
typedef TrieBranch* TrieBranchP;

class TrieBranch : public TrieBrancher
{
  public:
	  TrieBranch() : _branches() { }
	~TrieBranch() { for (int i = 0; i < 256; i++) delete _branches[i]; }

	TrieLeafP leaf(uint8 i)
		{
			if (_branches[i] == 0) _branches[i] = new TrieLeaf();
			return static_cast<TrieLeafP>(_branches[i]);
		}

	TrieBranchP branch(uint8 i)
		{
			if (_branches[i] == 0) _branches[i] = new TrieBranch();
			return static_cast<TrieBranchP>(_branches[i]);
		}

	bool empty(uint8 i)
		{
			return _branches[i] == 0;
		}

  private:
	TrieBrancherP	_branches[256];
};


class Trie
{
  public:
  	Trie() {}
	~Trie() {}

	void insert(uint32 u)
		{
			TrieBytes b;
			b._u32 = u;
			_root.branch(b._u8[3])->branch(b._u8[2])->leaf(b._u8[1])->insert(b._u8[0]);
		}

	bool test(uint32 u)
		{
			TrieBytes b;
			b._u32 = u;
			return _root.branch(b._u8[3])->branch(b._u8[2])->leaf(b._u8[1])->test(b._u8[0]);
		}

  private:
  	TrieBranch	_root;
};
