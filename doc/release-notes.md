bitcoinClean is now available as version 0.16.0

bitcoinClean version numbers mirror the Bitcoin Core version number it is based on.

In addition to the Bitcoin updates, bitcoinClean 0.16.0 has added:

# Security fixes
- Fix replay protection
- Fix minerrank vulnerability

# Testing
- Functional replay protection tests in test_rp.sh, and in seperate Bitcoin Core repository, test_rp2.sh

# Triggerered release

Replay protection will take effect after block 578269. All full nodes must be upgraded before that date.


