## On Basic Node (.12)

#### To check status

/root/btc_clean_prod/src/bitcoin-cli getblockcount

511517

## Real BTC wallet (with BTC before split)

KzYDwQCCoaw47DjyZ78a8Ufi5V938BcyrKpGT6CLrdjDMMCHei7g

17JkL9CJNWDPJFBoWHek7KcBq4KfhvHXf2

https://blockchain.info/address/17JkL9CJNWDPJFBoWHek7KcBq4KfhvHXf2

the same in our system

/root/btc_clean_prod/src/bitcoin-cli listunspent 1 99999999 '''["17JkL9CJNWDPJFBoWHek7KcBq4KfhvHXf2"]'''

### Imported by private key

/root/btc_clean_prod/src/bitcoin-cli importprivkey "KzYDwQCCoaw47DjyZ78a8Ufi5V938BcyrKpGT6CLrdjDMMCHei7g" "ksu_old"

/root/btc_clean_prod/src/bitcoin-cli listaddressgroupings

### Tx before split

/root/btc_clean_prod/src/bitcoin-cli listunspent 1 99999999 '''["17JkL9CJNWDPJFBoWHek7KcBq4KfhvHXf2"]'''

`
[
  {
    "txid": "3054ee41da9e806ba23c8957bf1819cf7c00cf1d03180df3412984c990f7580a",
    "vout": 0,
    "address": "17JkL9CJNWDPJFBoWHek7KcBq4KfhvHXf2",
    "account": "ksu_old",
    "scriptPubKey": "76a914452bff02a5977e55ee6b7af6a65c035a0e734c0d88ac",
    "amount": 0.00105774,
    "confirmations": 14870,
    "spendable": true,
    "solvable": true,
    "safe": true
  },
  {
    "txid": "9410a715ff8461478a2f8f8349e7e6c29477202463032a14e82b07d9cab99929",
    "vout": 1,
    "address": "17JkL9CJNWDPJFBoWHek7KcBq4KfhvHXf2",
    "account": "ksu_old",
    "scriptPubKey": "76a914452bff02a5977e55ee6b7af6a65c035a0e734c0d88ac",
    "amount": 0.19601793,
    "confirmations": 6302,
    "spendable": true,
    "solvable": true,
    "safe": true
  },
  {
    "txid": "26dc88683c037d84913d77ba76d334293780edec02f205209295c4ae84edf496",
    "vout": 1,
    "address": "17JkL9CJNWDPJFBoWHek7KcBq4KfhvHXf2",
    "account": "ksu_old",
    "scriptPubKey": "76a914452bff02a5977e55ee6b7af6a65c035a0e734c0d88ac",
    "amount": 0.27370000,
    "confirmations": 4436,
    "spendable": true,
    "solvable": true,
    "safe": true
  }
]`
all correct, https://blockchain.info/ru/tx/3054ee41da9e806ba23c8957bf1819cf7c00cf1d03180df3412984c990f7580a

## Main BTC -> BTC Clean

### Lets take tx after split

https://blockchain.info/ru/tx/e19bcd6085559c6a05f59c8f2b814264832de9579dcf0212218315f429b6a015

in hex

https://blockchain.info/ru/tx/e19bcd6085559c6a05f59c8f2b814264832de9579dcf0212218315f429b6a015?format=hex

0100000002d8c8df6a6fdd2addaf589a83d860f18b44872d13ee6ec3526b2b470d42a96d4d000000008b483045022100b31557e47191936cb14e013fb421b1860b5e4fd5d2bc5ec1938f4ffb1651dc8902202661c2920771fd29dd91cd4100cefb971269836da4914d970d333861819265ba014104c54f8ea9507f31a05ae325616e3024bd9878cb0a5dff780444002d731577be4e2e69c663ff2da922902a4454841aa1754c1b6292ad7d317150308d8cce0ad7abffffffff2ab3fa4f68a512266134085d3260b94d3b6cfd351450cff021c045a69ba120b2000000008b4830450220230110bc99ef311f1f8bda9d0d968bfe5dfa4af171adbef9ef71678d658823bf022100f956d4fcfa0995a578d84e7e913f9bb1cf5b5be1440bcede07bce9cd5b38115d014104c6ec27cffce0823c3fecb162dbd576c88dd7cda0b7b32b0961188a392b488c94ca174d833ee6a9b71c0996620ae71e799fc7c77901db147fa7d97732e49c8226ffffffff02c0175302000000001976a914a3d89c53bb956f08917b44d113c6b2bcbe0c29b788acc01c3d09000000001976a91408338e1d5e26db3fce21b011795b1c3c8a5a5d0788ac00000000

### Lets send it to our network

/root/btc_clean_prod/src/bitcoin-cli sendrawtransaction <hex_signrawtransaction>

/root/btc_clean_prod/src/bitcoin-cli sendrawtransaction 0100000002d8c8df6a6fdd2addaf589a83d860f18b44872d13ee6ec3526b2b470d42a96d4d000000008b483045022100b31557e47191936cb14e013fb421b1860b5e4fd5d2bc5ec1938f4ffb1651dc8902202661c2920771fd29dd91cd4100cefb971269836da4914d970d333861819265ba014104c54f8ea9507f31a05ae325616e3024bd9878cb0a5dff780444002d731577be4e2e69c663ff2da922902a4454841aa1754c1b6292ad7d317150308d8cce0ad7abffffffff2ab3fa4f68a512266134085d3260b94d3b6cfd351450cff021c045a69ba120b2000000008b4830450220230110bc99ef311f1f8bda9d0d968bfe5dfa4af171adbef9ef71678d658823bf022100f956d4fcfa0995a578d84e7e913f9bb1cf5b5be1440bcede07bce9cd5b38115d014104c6ec27cffce0823c3fecb162dbd576c88dd7cda0b7b32b0961188a392b488c94ca174d833ee6a9b71c0996620ae71e799fc7c77901db147fa7d97732e49c8226ffffffff02c0175302000000001976a914a3d89c53bb956f08917b44d113c6b2bcbe0c29b788acc01c3d09000000001976a91408338e1d5e26db3fce21b011795b1c3c8a5a5d0788ac00000000

!correct! error got

https://blockdozer.com/insight/tx/send

!correct! error got

## BTC Clean -> Main BTC

### Lets create tx in

/root/btc_clean_prod/src/bitcoin-cli sendfrom "ksu" 3Ldmu7WzmXK9C65DzJJdHNLf3sPe5dLh6w" 0.01 6

got

b87671e909d0b09eeebc45bc389b967ae1c3e214410cb756f25f3c8c8c0ef386

/root/btc_clean_prod/src/bitcoin-cli getrawtransaction b87671e909d0b09eeebc45bc389b967ae1c3e214410cb756f25f3c8c8c0ef386

0200000002294560abd68b824d2cb74ee29556dc109f381da1242b5686f67650c8d8fd9e02010000006a4730440220630a8612075c1d9bae97dd8971815a913440c135ef41b5c82572c65e17c28339022001b9af023581a37e018a5d8ef883b49d714747bd74a1e26d3ce1d3ddb4c2c50f012102200fa9f053285efc5c8d2a59525878bb77f28b27f2977be3b85193c535f6b609feffffff56ef92a83539c8557c725b8dd808a3be94ba2fbbd89433c7d92343061e4d9ef5000000006b483045022100c65070dc2b4db9e42e0dadc6fcdebd17da7414fd616225e57e191e13e31eda4302203ed352bc6ba634b348b64c091320e0b4f88d8c630603956dbe726540a70d8abc012102200fa9f053285efc5c8d2a59525878bb77f28b27f2977be3b85193c535f6b609feffffff02809698000000000017a9142f768db37e4aef0d542637aefd1111a31d3b17648709aa56000000000017a9149919d220d4eb11fb1b67110492e2d35f3dddc6ee871dce0700

https://blockchain.info/ru/pushtx

cant send !great!

https://blockdozer.com/insight/tx/send

cant send !great!

https://blockchain.info/ru/decode-tx

cant send !great!

/root/btc_clean_prod/src/bitcoin-cli gettransaction b87671e909d0b09eeebc45bc389b967ae1c3e214410cb756f25f3c8c8c0ef386
