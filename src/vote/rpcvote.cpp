// Copyright (c) 2018 The Bitcoin Clean Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <univalue.h>

#include "amount.h"
#include "base58.h"
#include "chain.h"
#include "consensus/validation.h"
#include "core_io.h"
#include "init.h"
#include "httpserver.h"
#include "validation.h"
#include "net.h"
#include "policy/feerate.h"
#include "policy/fees.h"
#include "policy/policy.h"
#include "policy/rbf.h"
#include "rpc/mining.h"
#include "rpc/server.h"
#include "script/sign.h"
#include "timedata.h"
#include "util.h"
#include "utilmoneystr.h"
#include "vote.h"

#ifdef ENABLE_WALLET
#include "wallet/coincontrol.h"
#include "wallet/feebumper.h"
#include "wallet/fees.h"
#include "wallet/wallet.h"
#include "wallet/walletdb.h"
#endif

#include <stdint.h>

#include <univalue.h>

#ifdef ENABLE_WALLET
UniValue emitvote(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() < 1)
        throw std::runtime_error(
            "\nemitvote\n"
            "\nUpvote or downvote target addresses from a source address that already has sufficient minerrank.\n"
            "\nOnly one vote tranaction can be emitted per block - therefore, it is recommended to perform all desired votes in one transaction as in the last example\n"
            "\nArguments:\n"
            "1. \"CLEAN source-address UP|DOWN targetAddress [targetAddress] ... [UP|DOWN] [targetAddress] [targetAddress] ...\"      (string, required) '[source_address] [UP|DOWN] target_address ... [UP|DOWN] target_address ...\n"
            "\nExamples:\n"

            "\nVote using source address 1M72Sfpbz1BPpXFHz9m3CdqATR44Jvaydd\n"
            + HelpExampleCli("emitvote", "\"CLEAN 1M72Sfpbz1BPpXFHz9m3CdqATR44Jvaydd UP 1PGFqEzfmQch1gKD3ra4k18PNj3tTUUSqg\"")
            + HelpExampleCli("emitvote", "\"CLEAN 1M72Sfpbz1BPpXFHz9m3CdqATR44Jvaydd UP 1PGFqEzfmQch1gKD3ra4k18PNj3tTUUSqg 171sgjn4YtPu27adkKGrdDwzRTxnRkBfKV\"")
            + HelpExampleCli("emitvote", "\"CLEAN 1M72Sfpbz1BPpXFHz9m3CdqATR44Jvaydd DOWN 1PGFqEzfmQch1gKD3ra4k18PNj3tTUUSqg\"")
            + HelpExampleCli("emitvote", "\"CLEAN 1M72Sfpbz1BPpXFHz9m3CdqATR44Jvaydd DOWN 1PGFqEzfmQch1gKD3ra4k18PNj3tTUUSqg 171sgjn4YtPu27adkKGrdDwzRTxnRkBfKV\"")
            + HelpExampleCli("emitvote", "\"CLEAN 1M72Sfpbz1BPpXFHz9m3CdqATR44Jvaydd UP 1PGFqEzfmQch1gKD3ra4k18PNj3tTUUSqg 171sgjn4YtPu27adkKGrdDwzRTxnRkBfKV DOWN 1PSSGeFHDnKNxiEyFrD1wcEaHr9hrQDDWc 16sSauSf5pF2UkUwvKGq4qjNRzBZYqgEL5\"")
          );

    if (request.params.size() > 1)
      throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Too many vote strings (on the command line, please enclose your vote string in quotes, e.g. emitvote \"CLEAN sourceaddr UP targetaddr\")");

    CVoteParser parser;
    parser.Init(request.params[0].get_str());

    while (!parser.done) {
      parser.Next();
      //LogPrintf("T %s\n", parser.result.ToString());
    }

    if (parser.valid == false)
      throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Vote syntax error: \"" + parser.str + "\"\n" + std::string(parser.pos + 20, ' ') + "^");
    CWallet * const pwallet = GetWalletForJSONRPCRequest(request);
    if (!EnsureWalletIsAvailable(pwallet, request.fHelp)) {
        return NullUniValue;
    }

    int nMinDepth = 1;
    int nMaxDepth = 9999999;
    bool include_unsafe = false;

    CAmount nMinimumAmount = 0;
    CAmount nMaximumAmount = MAX_MONEY;
    CAmount nMinimumSumAmount = MAX_MONEY;
    uint64_t nMaximumCount = 0;

    std::vector<COutput> vecOutputs;

    assert(pwallet != nullptr);
    LOCK2(cs_main, pwallet->cs_wallet);

    pwallet->AvailableCoins(vecOutputs, !include_unsafe, nullptr, nMinimumAmount, nMaximumAmount, nMinimumSumAmount, nMaximumCount, nMinDepth, nMaxDepth);

    CMutableTransaction mtx;

    std::string oo;
    bool sourceAddressInWallet = false;
    CAmount sourceAmount;

    uint256 txid;
    int nOutput;
    for (const COutput& out : vecOutputs) {
        CTxDestination address;
        const CScript& scriptPubKey = out.tx->tx->vout[out.i].scriptPubKey;
        bool fValidAddress = ExtractDestination(scriptPubKey, address);
        if (fValidAddress) {
            if (parser.result.sourceHash == boost::get<CKeyID>(address)) {
              sourceAddressInWallet = true;
              txid = out.tx->GetHash();
              nOutput = out.i;
              sourceAmount = out.tx->tx->vout[out.i].nValue;
              break;
            }
            //entry.push_back(Pair("txid", out.tx->GetHash().GetHex()));
            //entry.push_back(Pair("vout", out.i));
            //entry.push_back(Pair("address", CBitcoinAddress(address).ToString()));

            //if (pwallet->mapAddressBook.count(address)) {
                //entry.push_back(Pair("account", pwallet->mapAddressBook[address].name));
            //}
        }
        //entry.push_back(Pair("scriptPubKey", HexStr(scriptPubKey.begin(), scriptPubKey.end())));
        //entry.push_back(Pair("amount", ValueFromAmount(out.tx->tx->vout[out.i].nValue)));
        //entry.push_back(Pair("confirmations", out.nDepth));
        //entry.push_back(Pair("spendable", out.fSpendable));
        //entry.push_back(Pair("solvable", out.fSolvable));
        //entry.push_back(Pair("safe", out.fSafe));
    }
    if (!sourceAddressInWallet)
      throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "source address not in wallet: " + EncodeDestination(parser.result.sourceHash));

    if (!SufficientMinerrank(parser.result.sourceHash))
      throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Insufficient minerrank for source address: " + EncodeDestination(parser.result.sourceHash));

//    oo.append(sourceAddressInWallet ? "yes" : "no");
//    return oo;

    uint32_t nSequence = std::numeric_limits<uint32_t>::max();

    CTxIn in(COutPoint(txid, nOutput), CScript(), nSequence);

    mtx.vin.push_back(in);

    std::vector<unsigned char> data(parser.str.begin(), parser.str.end());

    CTxOut strOut(0, CScript() << OP_RETURN << data);
    mtx.vout.push_back(strOut);

    CScript scriptPubKey = GetScriptForDestination(parser.result.sourceHash);

    CCoinControl coinControl;
    unsigned int nBytes = GetVirtualTransactionSize(mtx);
    CAmount fee = GetMinimumFee(nBytes, coinControl, ::mempool, ::feeEstimator, nullptr /* FeeCalculation */);

    CAmount changeAmount = sourceAmount - fee;
    CTxOut changeOut(changeAmount, scriptPubKey);
    mtx.vout.push_back(changeOut);

    // Fetch previous transactions (inputs):
    CCoinsView viewDummy;
    CCoinsViewCache viewTmp(&viewDummy);
    {
        LOCK(mempool.cs);
        CCoinsViewCache &viewChain = *pcoinsTip;
        CCoinsViewMemPool viewMempool(&viewChain, mempool);
        viewTmp.SetBackend(viewMempool); // temporarily switch cache backend to db+mempool viewTmp

        for (const CTxIn& txin : mtx.vin) {
            viewTmp.AccessCoin(txin.prevout); // Load entries from viewChain into viewTmp; can fail.
        }

        viewTmp.SetBackend(viewDummy); // switch back to avoid locking mempool for too long
    }



    EnsureWalletIsUnlocked(pwallet);
    const CKeyStore& keystore = *pwallet;

    int nHashType = SIGHASH_ALL;
    bool fHashSingle = ((nHashType & ~SIGHASH_ANYONECANPAY) == SIGHASH_SINGLE);

    // Script verification errors
    UniValue vErrors(UniValue::VARR);

    // Use CTransaction for the constant parts of the
    // transaction to avoid rehashing.
    const CTransaction txConst(mtx);
    // Sign what we can:
    for (unsigned int i = 0; i < mtx.vin.size(); i++) {
        CTxIn& txin = mtx.vin[i];
        const Coin& coin = viewTmp.AccessCoin(txin.prevout);
        if (coin.IsSpent()) {
          UniValue entry(UniValue::VOBJ);
          entry.push_back(Pair("txid", txin.prevout.hash.ToString()));
          entry.push_back(Pair("vout", (uint64_t)txin.prevout.n));
          entry.push_back(Pair("scriptSig", HexStr(txin.scriptSig.begin(), txin.scriptSig.end())));
          entry.push_back(Pair("sequence", (uint64_t)txin.nSequence));
          entry.push_back(Pair("error", "Input not found or already spent"));
          vErrors.push_back(entry);
          continue;
        }
        const CScript& prevPubKey = coin.out.scriptPubKey;
        const CAmount& amount = coin.out.nValue;

        SignatureData sigdata;
        // Only sign SIGHASH_SINGLE if there's a corresponding output:
        if (!fHashSingle || (i < mtx.vout.size()))
            ProduceSignature(MutableTransactionSignatureCreator(&keystore, &mtx, i, amount, nHashType), prevPubKey, sigdata);
        sigdata = CombineSignatures(prevPubKey, TransactionSignatureChecker(&txConst, i, amount), sigdata, DataFromTransaction(mtx, i));

        UpdateTransaction(mtx, i, sigdata);

        ScriptError serror = SCRIPT_ERR_OK;
        if (!VerifyScript(txin.scriptSig, prevPubKey, &txin.scriptWitness, STANDARD_SCRIPT_VERIFY_FLAGS, TransactionSignatureChecker(&txConst, i, amount), &serror)) {
          UniValue entry(UniValue::VOBJ);
          entry.push_back(Pair("txid", txin.prevout.hash.ToString()));
          entry.push_back(Pair("vout", (uint64_t)txin.prevout.n));
          entry.push_back(Pair("scriptSig", HexStr(txin.scriptSig.begin(), txin.scriptSig.end())));
          entry.push_back(Pair("sequence", (uint64_t)txin.nSequence));
          entry.push_back(Pair("error", ScriptErrorString(serror)));
          vErrors.push_back(entry);
        }
    }
    bool fComplete = vErrors.empty();

    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("hex", EncodeHexTx(mtx)));
    result.push_back(Pair("complete", fComplete));
    if (!vErrors.empty()) {
        result.push_back(Pair("errors", vErrors));
    }

    LOCK(cs_main);

    CTransactionRef tx(MakeTransactionRef(std::move(mtx)));
    const uint256& hashTx = tx->GetHash();

    CCoinsViewCache &view = *pcoinsTip;
    bool fHaveChain = false;
    for (size_t o = 0; !fHaveChain && o < tx->vout.size(); o++) {
        const Coin& existingCoin = view.AccessCoin(COutPoint(hashTx, o));
        fHaveChain = !existingCoin.IsSpent();
    }
    bool fHaveMempool = mempool.exists(hashTx);
    if (!fHaveMempool && !fHaveChain) {
        // push to local node and sync with wallets
        CValidationState state;
        bool fMissingInputs;
        bool fLimitFree = true;

        if (!AcceptToMemoryPool(mempool, state, std::move(tx), &fMissingInputs, nullptr, true, maxTxFee)) {
            if (state.IsInvalid()) {
                throw JSONRPCError(RPC_TRANSACTION_REJECTED, strprintf("%i: %s", state.GetRejectCode(), state.GetRejectReason()));
            } else {
                if (fMissingInputs) {
                    throw JSONRPCError(RPC_TRANSACTION_ERROR, "Missing inputs");
                }
                throw JSONRPCError(RPC_TRANSACTION_ERROR, state.GetRejectReason());
            }
        }
    } else if (fHaveChain) {
        throw JSONRPCError(RPC_TRANSACTION_ALREADY_IN_CHAIN, "transaction already in block chain");
    }
    if(!g_connman)
        throw JSONRPCError(RPC_CLIENT_P2P_DISABLED, "Error: Peer-to-peer functionality missing or disabled");

    CInv inv(MSG_TX, hashTx);
    g_connman->ForEachNode([&inv](CNode* pnode)
    {
        pnode->PushInventory(inv);
    });
    return hashTx.GetHex();
}
#endif

UniValue listvotescore(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() > 1)
        throw std::runtime_error(
            "listapprovalscore Show approval scores based on approval transactions\n"
            "\nArguments:\n"
            "1. \"height\"      (int, optional) The block height. Use the current block if omitted.'\n"
            "\nResult:\n"
            "\"scores\"         (array) Addresses with corresponding minerrank and weight \n"
            "\nExamples:\n"
            + HelpExampleCli("listvotescore", "")
            + HelpExampleCli("listvotescore", "560388")
            + HelpExampleRpc("listvotescore", "")
            + HelpExampleRpc("listvotescore", "560388")
        );

    int height = -1;
    if (!request.params[0].isNull())
      height = request.params[0].get_int();
    CScoreKeeper k = GetScoreKeeper(height);

    UniValue ret(UniValue::VARR);

    for (auto s: k.scores) {
      UniValue entry(UniValue::VOBJ);
      entry.push_back(Pair("address", EncodeDestination(s.first)));
      entry.push_back(Pair("weight", s.second.weight));
      entry.push_back(Pair("minerrank", s.second.minerrank));
      ret.push_back(entry);
    }

    return ret;
}

UniValue getvotescore(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() > 1)
        throw std::runtime_error(
            "listapprovalscore Show approval scores based on approval transactions\n"
            "\nArguments:\n"
            "1. \"address\"     (string, required) Bitcoin address'\n"
            "\nResult:\n"
            "\"score\"          (object) addresses minerrank and weight \n"
            "\nExamples:\n"
            + HelpExampleCli("getvotescore", "1M72Sfpbz1BPpXFHz9m3CdqATR44Jvaydd")
            + HelpExampleRpc("getvotescore", "1D1ZrZNe3JUo7ZycKEYQQiQAWd9y54F4XX")
        );

    CTxDestination destination = DecodeDestination(request.params[0].get_str());
    if (!IsValidDestination(destination))
      throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "invalid address: " + request.params[0].get_str());

    const CKeyID* hash = boost::get<CKeyID>(&destination);
    if (!hash)
      throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "pubkey-address-required (use getnewaddress -addresstype legacy)" + request.params[0].get_str());

    CScore score;
    if (!GetVoteScore(*hash, score))
      throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "score-not-found " + request.params[0].get_str());

    UniValue ret(UniValue::VOBJ);
    ret.push_back(Pair("weight", score.weight));
    ret.push_back(Pair("minerrank", score.minerrank));

    return ret;
}

UniValue listvotelock(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() > 1)
        throw std::runtime_error(
            "listapprovalscore Show votes that are locked to to another recent vote (down after up and vice versa always possible)\n"
            "\nArguments:\n"
            "None"
            "\nResult:\n"
            "\"locks\"         (array) Addresses with corresponding vote height \n"
            "\nExamples:\n"
            + HelpExampleCli("listvotelock", "")
        );

    int height = -1;
    if (!request.params[0].isNull())
      height = request.params[0].get_int();
    CScoreKeeper k = GetScoreKeeper(height);

    UniValue ret(UniValue::VARR);

    for (auto s: k.voteLock) {
      UniValue entry(UniValue::VOBJ);
      entry.push_back(Pair("vote", s.first.ToString()));
      entry.push_back(Pair("lock", s.second));
      entry.push_back(Pair("free", s.second + VOTE_WINDOW));
      ret.push_back(entry);
    }

    return ret;
}

static const CRPCCommand commands[] =
{
#ifdef ENABLE_WALLET
    { "vote",  "emitvote",                 &emitvote,             {"vote"} },
#endif
    { "vote",  "getvotescore",             &getvotescore,         {"height"} },
    { "vote",  "listvotescore",            &listvotescore,        {"address"} },
    { "vote",  "listvotelock",             &listvotelock,         {} },
};

void RegisterVoteRPCCommands(CRPCTable &t)
{
    for (unsigned int vcidx = 0; vcidx < ARRAYLEN(commands); vcidx++)
        t.appendCommand(commands[vcidx].name, &commands[vcidx]);
}
