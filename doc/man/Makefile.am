dist_man1_MANS=
if CLIENT_IS_VERIUM
if BUILD_BITCOIND
  dist_man1_MANS+=veriumd.1
endif

if ENABLE_QT
  dist_man1_MANS+=verium-qt.1
endif

if BUILD_BITCOIN_CLI
  dist_man1_MANS+=verium-cli.1
endif

if BUILD_BITCOIN_TX
  dist_man1_MANS+=verium-tx.1
endif

if ENABLE_WALLET
if BUILD_BITCOIN_WALLET
  dist_man1_MANS+=verium-wallet.1
endif
endif

else

if BUILD_BITCOIND
  dist_man1_MANS+=vericoind.1
endif

if ENABLE_QT
  dist_man1_MANS+=vericoin-qt.1
endif

if BUILD_BITCOIN_CLI
  dist_man1_MANS+=vericoin-cli.1
endif

if BUILD_BITCOIN_TX
  dist_man1_MANS+=vericoin-tx.1
endif

if ENABLE_WALLET
if BUILD_BITCOIN_WALLET
  dist_man1_MANS+=vericoin-wallet.1
endif
endif

endif
