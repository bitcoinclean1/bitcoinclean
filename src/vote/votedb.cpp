// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "vote/votedb.h"
#include <string>
#include <map>
#include <utility>

CVoteDB::CVoteDB() : db(GetDataDir() / "vote", 0, false, false, true)
{
}

bool CVoteDB::Read(const int height, CScoreKeeper &keeper)
{
  return db.Read(height, keeper);
}

bool CVoteDB::Write(const int height, const CScoreKeeper &keeper)
{
  return db.Write(height, keeper, true);
}

bool CVoteDB::Erase(const int height)
{
  return db.Erase(height);
}

bool CVoteDB::MempoolLock(const CVote &vote)
{
  return db.Write(vote, -1);
}

bool CVoteDB::MempoolUnlock(const CVote &vote)
{
  return db.Erase(vote);
}

bool CVoteDB::HasMempoolLock(const CVote &vote)
{
  int val = 0;
  db.Read(vote, val);
  return val == -1;
}
