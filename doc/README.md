Veribase
=============

What's Veribase
---------------------
Veribase is the code base behind Vericoin and Verium. It allow to keep both coins at the same level of feature and update.
Therefore, it's a first step into a common wallet for Vericoin & Verium.
In the following docs and code, reference to veribase will often be made and you might have to adapt based on your need.

Setup
---------------------
Veribase is the original Vericoin & Verium client and it builds the backbone of the network. It downloads and, by default, stores the entire history of Vericoin & Verium transactions, which requires a few gigabytes of disk space. Depending on the speed of your computer and network connection, the synchronization process can take anywhere from a few hours to a day or more.

To download Vericoin / Verium, visit [vericonomy.com](https://vericonomy.com/).

Running
---------------------
The following are some helpful notes on how to run Vericoin and verium on your native platform.

### Unix

Unpack the files into a directory and run:

For Vericoin:
- `bin/vericoin-qt` (GUI) or
- `bin/vericoind` (headless)

For Verium
- `bin/verium-qt` (GUI) or
- `bin/veriumd` (headless)

### Windows

Unpack the files into a directory, and then run vericoin-qt.exe or verium-qt.exe .

### macOS

Drag Vericoin / Verium to your applications folder, and then run it.

### Need Help?

* See the documentation at the [Vericoin & Verium Wiki](https://wiki.vericoin.info/)
for help and more information.
* Ask for help on
 - [Slack](https://slack.vericoin.info)
 - [Telegram](https://t.me/vericoinandverium)
 - [Vericoin & Verium Reddit](https://www.reddit.com/r/vericoin)

Building
---------------------
The following are developer notes on how to build Veribase on your native platform. They are not complete guides, but include notes on the necessary libraries, compile flags, etc.

- [Dependencies](dependencies.md)
- [macOS Build Notes](build-osx.md)
- [Unix Build Notes](build-unix.md)
- [Windows Build Notes](build-windows.md)
- [FreeBSD Build Notes](build-freebsd.md)
- [OpenBSD Build Notes](build-openbsd.md)
- [NetBSD Build Notes](build-netbsd.md)

Development
---------------------
The Veribase repo's [root README](/README.md) contains relevant information on the development process and automated testing.

- [Developer Notes](developer-notes.md)
- [Productivity Notes](productivity.md)
- [Release Notes](release-notes.md)
- [Release Process](release-process.md)
- [Translation Process](translation_process.md)
- [Translation Strings Policy](translation_strings_policy.md)
- [JSON-RPC Interface](JSON-RPC-interface.md)
- [Unauthenticated REST Interface](REST-interface.md)
- [Shared Libraries](shared-libraries.md)
- [BIPS](bips.md)
- [Dnsseed Policy](dnsseed-policy.md)
- [Benchmarking](benchmarking.md)

### Resources
* Discuss on the [Slack](https://slack.vericoin.info), in the development channel.
* Follow the [Development Kanban](https://trello.com/b/Fna9ydfw/vericonomy).

### Miscellaneous
- [Assets Attribution](assets-attribution.md)
- [vericonomy.conf Configuration File](vericonomy-conf.md)
- [Files](files.md)
- [Fuzz-testing](fuzzing.md)
- [Reduce Memory](reduce-memory.md)
- [Reduce Traffic](reduce-traffic.md)
- [Tor Support](tor.md)
- [Init Scripts (systemd/upstart/openrc)](init.md)
- [ZMQ](zmq.md)
- [PSBT support](psbt.md)

License
---------------------
Distributed under the [MIT software license](/COPYING).
