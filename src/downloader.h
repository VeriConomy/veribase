#ifndef BITCOIN_DOWNLOADER_H
#define BITCOIN_DOWNLOADER_H

#if defined(HAVE_CONFIG_H)
#include <config/bitcoin-config.h>
#endif

#include <string>

#if defined(__arm__) || defined(__aarch64__)
const std::string BOOTSTRAP_PATH("/bootstrap-arm/bootstrap.zip");
#else
const std::string BOOTSTRAP_PATH("/bootstrap/bootstrap.zip");
#endif

const std::string VERSIONFILE_PATH("/VERSION.json");

const std::string CLIENT_URL_VRM("https://files.vericonomy.com/vrm");
const std::string CLIENT_URL_VRC("https://files.vericonomy.com/vrc");

void downloadBootstrap();
void applyBootstrap();
void downloadVersionFile();
void downloadClient(std::string fileName);
int getArchitecture();

#endif // BITCOIN_DOWNLOADER_H
