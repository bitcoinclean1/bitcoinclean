// Copyright (c) 2018 The Bitcoin Clean Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_VOTE_RPCVOTE_H
#define BITCOIN_VOTE_RPCVOTE_H

class CRPCTable;
class JSONRPCRequest;

void RegisterVoteRPCCommands(CRPCTable &t);

#endif //BITCOIN_VOTE_RPCVOTE_H
