#ifndef BITCOIN_DOWNLOADER_H
#define BITCOIN_DOWNLOADER_H

#include <string>

#if defined(__arm__) || defined(__aarch64__)
const std::string BOOTSTRAP_PATH("/bootstrap-arm/bootstrap.zip");
#else
const std::string BOOTSTRAP_PATH("/bootstrap/bootstrap.zip");
#endif

const std::string VERSIONFILE_PATH("VERSION.json");

#if CLIENT_IS_VERIUM
const std::string CLIENT_URL("https://files.vericonomy.com/vrm");
#else
const std::string CLIENT_URL("https://files.vericonomy.com/vrc");
#endif

void downloadBootstrap();
void applyBootstrap();
void downloadVersionFile();
void downloadClient(std::string fileName);
int getArchitecture();

#endif // BITCOIN_DOWNLOADER_H
