
#ifndef BITCOIN_VOTE_H
#define BITCOIN_VOTE_H

// Mechanism for miners to approve each other
// Currently used to ensure proof of renewable energy use
// Could be used to enforce arbitrary non-technical rules

#include "base58.h"
#include "chain.h"
#include "util.h"
#include "primitives/block.h"
#include "primitives/transaction.h"
#include "pubkey.h"
#include "script/standard.h"
#include "serialize.h"
#include "util.h"
#include "utilstrencodings.h"
#include "validation.h"
#include "validationinterface.h"
#include "vote/rpcvote.h"


class CKeyID;

typedef unsigned char vote_type_t;

static const vote_type_t VOTE_UP = 0xa1;
static const vote_type_t VOTE_DOWN = 0xa2;

static const unsigned char REJECT_VOTE_INVALID = 0x61;
static const unsigned char REJECT_VOTE_INAUTHENTIC = 0x62;
static const unsigned char REJECT_VOTE_INSUFFICIENT = 0x63;
static const unsigned char REJECT_VOTE_SELF = 0x64;
static const unsigned char REJECT_VOTE_LOCKED = 0x65;
static const unsigned char REJECT_VOTE_MEMPOOL = 0x66;

static const int VOTE_WEIGHT_CYCLE = 4;
static const double VOTE_WEIGHT_FACTOR = 89.0/113.0;
static const double VOTE_ATTRITION_RATE = 1.000087644373237;

const std::string ScriptToString(const CScript &script, int n=0);
const CKeyID ExtractDestination(const CScript &scriptPubKey);

bool IsVoteTx(const CTransaction &tx);
bool ValidateVoteTx(const CTransaction &tx, CValidationState& state, const CCoinsViewCache &inputs);
bool SufficientMinerrank(const CKeyID &hash);
int VoteType(const std::string &str);
std::string VoteTypeStr(const int &type);
void InitVote();

static const int VOTE_HEIGHT_CUTOFF = 560388;
static const unsigned char VOTE_MIN_ACTIVE = 7;

// window size in blocks for vote lock and weight vote
static const int VOTE_WINDOW = 4500;

class CVote
{
  public:
    CKeyID sourceHash;
    vote_type_t type;
    CKeyID targetHash;
    std::string ToString() const;
    void Invert();

    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
      READWRITE(sourceHash);
      READWRITE(type);
      READWRITE(targetHash);
    }
};

bool operator<(const CVote &v1, const CVote &v2);
bool IsVoteLocked(const CVote &vote);
bool IsVoteInMempool(const CVote &vote);

class CScore
{
  public:
    double minerrank = 0;
    double weight = 0;
    double delta = 1;
    double cycle = 0;

    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
      READWRITE(minerrank);
      READWRITE(weight);
      READWRITE(delta);
      READWRITE(cycle);
    }

    std::string ToString() const;
};

struct confirmee_str
{
  int height;
  CKeyID hash;
  vote_type_t type;

  ADD_SERIALIZE_METHODS;
  template <typename Stream, typename Operation>
  inline void SerializationOp(Stream& s, Operation ser_action) {
    READWRITE(height);
    READWRITE(hash);
    READWRITE(type);
  }
};

typedef confirmee_str confirmee_t;

class CScoreKeeper
{
  public:
    int height;
    std::map<CKeyID, CScore> scores;
    int active = 0;

    std::map<CVote, int> voteLock;
    std::map<CKeyID, std::vector<confirmee_t>> confirmees;

    void Add(const CVote &vote);
    void Attrit();
    void Window();
    bool Sufficient(const CKeyID &hash);

    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
      READWRITE(height);
      READWRITE(scores);
      READWRITE(active);
      READWRITE(voteLock);
      READWRITE(confirmees);
    }

    std::string ToString() const;
};

bool IsVoteAuthentic(const CTransaction &tx, const CKeyID &sourceHash, const CCoinsViewCache &inputs);

class CVoteParser
{
  public:
    std::string str;
    int pos;
    int next;
    int type;
    std::string token;
    void Init(std::string s);
    void Next();
    bool valid;
    bool done;
    CVote result;
};

class CVoteInterface : public CValidationInterface
{
  public:
    void BlockConnected(const std::shared_ptr<const CBlock>& pblock, const CBlockIndex *pindex, const std::vector<CTransactionRef>& vtxConflicted) override;
    void BlockDisconnected(const std::shared_ptr<const CBlock>& pblock) override;
    void TransactionAddedToMempool(const CTransactionRef& ptx) override;
};

//RPC helpers

CScoreKeeper GetScoreKeeper(int height = -1);
CScore GetVoteScore(const CKeyID &hash);

#endif // BITCOIN_VOTE_H
