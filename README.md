bitcoinClean
============

Mission
-------
bitcoinClean aims to overcome the issue of vast energy consumption with high carbon emissions for mining digital currencies by exclusively using renewable energy. Therefore bitcoinClean introduces the proof-of-greenness protocol to verify the use of green energy.

bitcoinClean will be the first coin mined only with renewable energy. It relies on a network that is powered by Solar, Wind, Hydro, Thermal, Biogas or Tidal energy. Every node must undergo a peer review to participate in the network.

We want to make bitcoinClean the currency of the future for everyone who cares.

Technical Information
---------------------

bitcoinClean is a hard fork of Bitcoin Core and adds a restriction on mining- blocks are rejected if the address does not possess a sufficient quantity of minerrank.

minerrank is achieved by receiving votes by miners who already possess enough minerrank.

A vote is a null data transaction that contains a special microlanguage that allows expressing approval or disapproval of a miner.

bitcoinClean clients collects these votes and consensus minerrank is determined.

It is understood that miners are upvoted based on their ability to sufficiently document that the source of energy for their hash rate has indeed come from renewable sources such as hydro, wind or solar. Further, a miner is downvoted when does not provide such documentation even after being prompted several times. A new miner needs to make a credible case that sufficient clean energy capacity is available.

Please contact the community at https://bitcoinclean.org if you wish to become a miner.

Contributions
-------------

Contributions, large and small, are very welcome. Please report any issues at https://github.com/bitcoinclean/bitcoinclean/issues and apply for pull requests at https://github.com/bitcoinclean/bitcoinclean/pulls. Label an issue as "discussion" for development questions.

Usage
-----

For a user, bitcoinClean works just like Bitcoin Core.

bitcoinClean adds the following RPC commands to participate in voting as a miner, and to make transparent the current consensus of minerrank.

    emitvote                    Perform a vote
    listvotescores              List minerrank and weight
    listvotelock                See when the same address can be voted for again

Please see the help for these commands for details.

Security
--------

To verify that replay attacks are not feasible with bitcoinClean, please see doc/bitcoinclean-replay.md

License
--------
bitcoinClean is released under the terms of the MIT license. See COPYING for more information or see https://opensource.org/licenses/MIT.

