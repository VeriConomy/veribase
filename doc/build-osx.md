# macOS Build Instructions and Notes

The commands in this guide should be executed in a Terminal application.
The built-in one is located in
```
/Applications/Utilities/Terminal.app
```

## Preparation
Install the macOS command line tools:

```shell
xcode-select --install
```

When the popup appears, click `Install`.

Then install [Homebrew](https://brew.sh).

## Dependencies
```shell
brew install automake berkeley-db4 libtool boost miniupnpc openssl pkg-config protobuf python3 qt libevent
```

If you run into issues, check [Homebrew's troubleshooting page](https://docs.brew.sh/Troubleshooting).
See [dependencies.md](dependencies.md) for a complete overview.

If you want to build the disk image with `make deploy` (.dmg / optional), you need RSVG:
```shell
brew install librsvg
```

## Berkeley DB
It is recommended to use Berkeley DB 4.8. If you have to build it yourself,
you can use [this](/contrib/install_db4.sh) script to install it
like so:

```shell
./contrib/install_db4.sh .
```

from the root of the repository.

**Note**: You only need Berkeley DB if the wallet is enabled (see [*Disable-wallet mode*](/doc/build-osx.md#disable-wallet-mode)).

## Build Vericoin

1. Clone the Vericoin source code:
    ```shell
    git clone https://github.com/VeriConomy/vericoin.git
    cd vericoin
    ```

2.  Build Vericoin:

    Configure and build the headless Vericoin binaries as well as the GUI (if Qt is found).

    You can disable the GUI build by passing `--without-gui` to configure.
    ```shell
    ./autogen.sh
    ./configure
    make
    ```

3.  It is recommended to build and run the unit tests:
    ```shell
    make check
    ```

4.  You can also create a  `.dmg` that contains the `.app` bundle (optional):
    ```shell
    make deploy
    ```

## Build Verium

1. Clone the Veriumsource code:
    ```shell
    git clone https://github.com/VeriConomy/verium.git
    cd verium
    ```

2.  Build Verium:

    Configure and build the headless Verium binaries as well as the GUI (if Qt is found).

    You can disable the GUI build by passing `--without-gui` to configure.
    ```shell
    ./autogen.sh
    ./configure
    make
    ```

3.  It is recommended to build and run the unit tests:
    ```shell
    make check
    ```

4.  You can also create a  `.dmg` that contains the `.app` bundle (optional):
    ```shell
    make deploy
    ```

## `disable-wallet` mode
When the intention is to run only a P2P node without a wallet, Vericoin/Verium may be
compiled in `disable-wallet` mode with:
```shell
./configure --disable-wallet
```

In this case there is no dependency on Berkeley DB 4.8.

Mining is also possible in disable-wallet mode using the `getblocktemplate` RPC call.

## Running
Vericoin is now available at `./src/vericoind`
and
Verium is now available at `./src/veriumd`


Before running, you may create an empty configuration file:
```shell
mkdir -p "/Users/${USER}/Library/Application Support/Vericonomy"

touch "/Users/${USER}/Library/Application Support/Vericonomy/vericonomy.conf"

chmod 600 "/Users/${USER}/Library/Application Support/Vericonomy/vericonomy.conf"
```

The first time you run the wallet, it will start downloading the blockchain. This process could
take many hours, or even days on slower than average systems.

You can monitor the download process by looking at the debug.log file:
```shell
tail -f $HOME/Library/Application\ Support/Vericonomy/vericoin/debug.log
tail -f $HOME/Library/Application\ Support/Vericonomy/verium/debug.log
```

## Other Vericoin commands:
```shell
./src/vericoind -daemon     # Starts the vericoin daemon.
./src/vericoin-cli --help   # Outputs a list of command-line options.
./src/vericoin-cli help     # Outputs a list of RPC commands when the daemon is running.
```

## Other Verium commands:
```shell
./src/veriumd -daemon     # Starts the verium daemon.
./src/verium-cli --help   # Outputs a list of command-line options.
./src/verium-cli help     # Outputs a list of RPC commands when the daemon is running.
```


## Notes
* Tested on OS X 10.12 Sierra through macOS 10.15 Catalina on 64-bit Intel
processors only.