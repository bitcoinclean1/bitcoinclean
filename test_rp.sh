#!/bin/bash
TEST_DIR1=test1
TEST_DIR2=test2
BITCOIN_CLI="src/bitcoinclean-cli -datadir=$TEST_DIR1 -port=19101 -rpcport=19201 -rpcuser=test1 -rpcpassword=test1"
BITCOIN_CLI_2="src/bitcoinclean-cli -datadir=$TEST_DIR2 -port=19101 -rpcport=19201 -rpcuser=test1 -rpcpassword=test1"
BITCOIND="src/bitcoincleand"

cleanup() {
 killall bitcoincleand
 sleep 1
 rm -rf $TEST_DIR1
 rm -rf $TEST_DIR2
}

echo "Killing bitcoincleand instances..."
killall bitcoincleand
sleep 1
rm -rf $TEST_DIR1
rm -rf $TEST_DIR2
mkdir $TEST_DIR1
mkdir $TEST_DIR2
echo "Starting bitcoincleand instances..."
$BITCOIND -regtest -datadir=$TEST_DIR1 -port=19101 -rpcport=19201 -daemon=1 -server=1 -rpcuser=test1 -rpcpassword=test1 -addnode=127.0.0.1:19102
$BITCOIND -regtest -datadir=$TEST_DIR2 -port=19102 -rpcport=19202 -daemon=1 -server=1 -rpcuser=test1 -rpcpassword=test1 -addnode=127.0.0.1:19101
sleep 5
WALLET_1=`$BITCOIN_CLI getaccountaddress ""`
WALLET_2=`$BITCOIN_CLI_2 getaccountaddress ""`

GN=`$BITCOIN_CLI generate 2050`
TX=`$BITCOIN_CLI listunspent 1001 2000 | python -c "import sys, json; print json.dumps(json.load(sys.stdin)[0])"`
if [ -z "$TX" ]; then
    echo "TX is empty"
    cleanup
    exit 1
fi
echo "found unspent transaction $TX"
AMOUNT=`echo $TX | python -c "import sys, json; print json.dumps(json.load(sys.stdin)['amount'])"`
echo "amount of transaction $AMOUNT"
tospend=`echo $AMOUNT-0.01 | bc`
echo $tospend
TX=`echo $TX | python -c "import sys, json; print json.dumps(json.load(sys.stdin)['txid'])"`
echo $TX
TX1="[{\"txid\":$TX,\"vout\":0}]"
TX2="{\"$WALLET_2\":$tospend}"
TXHEX=`$BITCOIN_CLI createrawtransaction "$TX1" "$TX2"`
echo "TX:" $TXHEX
SIG_RES=`$BITCOIN_CLI signrawtransaction "$TXHEX" null null "ALL"`
echo "signature without FORKID result: $SIG_RES"
SIGNHEX=`echo $SIG_RES | python -c "import sys, json; print json.dumps(json.load(sys.stdin)['hex']).replace('\"','')"`
echo "signed TX:" $SIGNHEX
echo "Trying to send transaction:"
SEND_RES=`$BITCOIN_CLI sendrawtransaction "$SIGNHEX"`
echo "sending result: $SEND_RES"
MEM1=`$BITCOIN_CLI getrawmempool`
echo "mempool before creating new block: $MEM1"
MEM_SIZE=`echo $MEM1 | python -c "import sys, json; print len(json.load(sys.stdin))"`
if [ "$MEM_SIZE" -ne "0" ];
then
    echo "TEST: Transaction without FORKID should not be in mempool after sending transaction: FAILED!"
    cleanup
    exit 1
fi
$BITCOIN_CLI generate 1
MEM2=`$BITCOIN_CLI getrawmempool`
echo "mempool after creating new block: $MEM2"
MEM2_SIZE=`echo $MEM2 | python -c "import sys, json; print len(json.load(sys.stdin))"`
if [ "$MEM2_SIZE" -ne "0" ];
then
    echo "TEST: Transaction without FORKID should not be in mempool after generating of block: FAILED!"
    cleanup
    exit 1
fi

SIG2_RES=`$BITCOIN_CLI signrawtransaction "$TXHEX" null null "ALL|FORKID"`
echo "signature with FORKID result: $SIG_RES"
SIGNHEX=`echo $SIG2_RES | python -c "import sys, json; print json.dumps(json.load(sys.stdin)['hex']).replace('\"','')"`
SEND_RES=`$BITCOIN_CLI sendrawtransaction "$SIGNHEX"`
echo "sending result: $SEND_RES"
MEM1=`$BITCOIN_CLI getrawmempool`
MEM_SIZE=`echo $MEM1 | python -c "import sys, json; print len(json.load(sys.stdin))"`
if [ "$MEM_SIZE" -ne "1" ];
then
    echo "TEST: Transaction with FORKID should be in mempool after sending raw transaction: FAILED!"
    cleanup
    exit 1
fi
echo "mempool before creating new block: $MEM1"
$BITCOIN_CLI generate 1
MEM2=`$BITCOIN_CLI getrawmempool`
echo "mempool after creating new block: $MEM2"

MEM2_SIZE=`echo $MEM2 | python -c "import sys, json; print len(json.load(sys.stdin))"`
if [ "$MEM2_SIZE" -ne "0" ];
then
    echo "TEST: Transaction with FORKID should not be in mempool after generating of block: FAILED!"
    cleanup
    exit 1
fi
echo "PASSED"
cleanup
exit 0
 
#SEND_RES=`$BITCOIN_CLI sendrawtransaction "$SIGNHEX"`
#echo "after fork sending result: $SEND_RES"



#SIGNHEX=`src/bitcoinclean-cli -regtest -datadir=1 -rpcport=19201 sendrawtransaction "$SIGHEX"`

#killall bitcoincleand
