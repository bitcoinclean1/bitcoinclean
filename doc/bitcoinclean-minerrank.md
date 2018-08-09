
bitcoinClean MinerRank
======================

bitcoinClean introduced two additional concepts into the core bitcoin software: minerrank, and weight.

Minerrank and weight are associated with a bitcoinClean address that is to receive block rewards, and are implemented as a global object `keeper` of type `CScoreKeeper` defined in `src/vote/vote.cpp`, and made persistent by a global object `voteDB` of type `*CVoteDB`.

Most code is kept in the new directory `src/vote`. Outside this directory, only `src/init.cpp` and `src/rpcclient.cpp` are modified for initialization purposes, and `src/validation.cpp` contains conditionals in two areas in order to reject newly mined blocks with rewards that do not have sufficient minerrank applied to them. 

Keeping score
-------------

The score keeper object contains a hash table associating a score object of type `CScore` with zero or more address signatures, as well as methods to apply an upvote, a downvote, as well as to attrition to the stored scores.

`CScore` saves `rank`, `weight`, `delta` and `cycle`. Rank and weight are minerrank and weight, which are calculated from the numbers and types of votes received according to the formulas in the whitepaper. `delta` and `cycle` are implementation details- `delta` represents the amount of score that a target will change on receiving on upvote or downvote, and `cycle` determines how many votes are required to trigger a weight recalculation. `delta` and `cycle` are used the way they are to use multiplications rather than recursive expentials calculations to achieve feasible performance characteristics.

The `Add` method handles adding an additional up- or downvote in the form of a `CVote` to the score. One or more `CVote` objects are emitted by creating an object of type `CVoteParser` and handing it a vote string, which in turn is passed into the block chain as the payload of a null data transaction. A loop is used to iterate over all vote objects derived from a vote string, and `Add` is called on each, if it is valid, by the block verification code.

The `Window` method is used to prevent the repitition of the same vote- an upvote from and for the same address must be followed by a downvote and vice versa, unless a certain number of blocks has passed, after which votes may be repeated.

The `Sufficient` method checks whether an address possesses sufficient miner rank to receive mining rewards and emit votes.

The `Attrit` method applies attrition to minerrank and weight, slowly degrading them, requiring intermittent re-upvotes in order to maintain sufficient minerrank.

If the number of approved miners sinks below a the minimum, the minerrank mechanism is disabled and voting and mining is allowed for everybody- this is considered an extraordinary circumstance and not a normal part of operation after a sufficient number of miners has been initially reached during an initial process of bootstrapping.

Providing persistence
---------------------

The bitcoin network may rewind blocks at any time, and does so by saving undo data. For minerrank, the required data is much smaller, so the complete score keeper object is serialized for every block instead and saved into the `voteDB` database using the block number as key. VoteDB is a levelDB instance that saves its data into the `.bitcoinclean/vote` directory.

When a block rewind is detected using the `BlockDisconnect` API call, the data is deleted. When BlockConnect is called, the data is loaded when available.

The RPC commands
----------------

RPC commands are declared in `src/vote/rpcvote.h`, added like any other core RPC command in `src/init.cpp` and implemented in `src/vote/rpcvote.cpp`. They allow the emission of a vote on the blockchain (`emitevote`) as well as checking the current vote scores (`listvotescores` and `listvotescore`). `emitvote` was mostly adapted into a single command from the core raw transaction code and additional checks added.

Validation
----------

In order to make sure everyone has the same votes and by derivation the same score, votes are checked for their integrity and validity by additional checks in `src/validation.cpp`. Blocks are rejected and votes are ignored if the votes are not valid, depending on the reason, using newly added bitcoin core error messages, so reasons for behavior can always be verified using log files or other monitoring tools already in use. `ValidateVoteTx` and `SufficientMinerrank` are validation functions provided for this purpose, introducing the following new possible transaction verification error strings:

    repeated-vote-target    
    invalid-vote-syntax
    self-vote
    inauthentic-vote
    insufficient-minerrank
    vote-in-mempool
    locked-vote

Summary
-------

The vote system permits voting on the inclusion and exclusion of new and existing miners by the existing community, calculates a score, and enforces the votes with regards to mining, on-chain.

Performance and disk-space impact is very low compared to existing code, and patches are minimally invasive in order to facilitate merging of future upstream technologies as they are adopted by the core team.

