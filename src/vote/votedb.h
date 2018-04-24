// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_VOTEDB_H
#define BITCOIN_VOTEDB_H

#include "dbwrapper.h"

#include "vote.h"

class CVoteDB
{
  protected:
    CDBWrapper db;

  public:
    CVoteDB();
    bool Read(int height, CScoreKeeper &keeper);
    bool Write(int height, const CScoreKeeper &keeper);
    bool Erase(int height);

    bool MempoolLock(const CVote &vote);
    bool MempoolUnlock(const CVote &vote);
    bool HasMempoolLock(const CVote &vote);
};

#endif // BITCOIN_VOTEDB_H
