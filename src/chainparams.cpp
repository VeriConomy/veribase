// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/merkle.h>
#include <tinyformat.h>
#include <util/system.h>
#include <util/strencodings.h>

#include <assert.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

static CBlock CreateGenesisBlock(std::string chainName, const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.nTime = nTime; // PoXT
    txNew.vin.resize(1);
    txNew.vout.resize(1);

    if (chainName == CBaseChainParams::VERIUM)
    {
        txNew.vin[0].scriptSig = CScript() << 0 << CScriptNum(999) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].SetEmpty();
    }
    else
    {
        txNew.vin[0].scriptSig = CScript() << 0 << CScriptNum(42) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].SetEmpty();
    }

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 */
static CBlock CreateGenesisBlock(std::string chainName, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "9 May 2014 US politicians can accept bitcoin donations";
    if (chainName == CBaseChainParams::VERIUM)
        pszTimestamp = "VeriCoin block 1340292";

    const CScript genesisOutputScript = CScript();

    return CreateGenesisBlock(chainName, pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Vericoin network
 */
class CVericoinParams : public CChainParams {
public:
    CVericoinParams() {
        strNetworkID = CBaseChainParams::VERICOIN;
        consensus.BIP34Height = 1;
        consensus.BIP65Height = 600000; // 00000000007f6655f22f98e72ed80d8b06dc761d5da09df0fa1dc4be4f861eb6
        consensus.BIP66Height = 600000; // 000000002104c8c45e99a8853285a3b592602a3ccde2b832481da85e9e4ba182
        consensus.CSVHeight = 600000; // 00000000025e930139bac5c6c31a403776da130831ab85be56578f3fa75369bb

        /** Coin Settings **/
        consensus.VIP1Height = 0; // Change Min Fee
        consensus.nMaturity = 500;
        consensus.NextTargetV2Height = 38424; // Moving From GetNextTargetRequiredV1 to V2
        consensus.PoSTHeight = 608100; // Start PoST
        consensus.PoSHeight = 20160; // Start PoS for Vericoin
        consensus.nInitialCoinSupply = 26751452;

        /** PoWS Settings **/
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetSpacing = 60;
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two days
        consensus.fPowNoRetargeting = false;

        /** PoST Settings **/
        consensus.posLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // No use for verium
        consensus.nStakeTargetSpacing = 60;
        consensus.nStakeMinAge = 8 * 60 * 60; // 8 hours
        consensus.nModifierInterval = 10 * 60;
        consensus.nTargetTimespan = 16 * 60;

        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000000000000");
        consensus.defaultAssumeValid = uint256S("0x0000000000000000000000000000000000000000000000000000000000000000"); // 654683

        pchMessageStart[0] = 0x70;
        pchMessageStart[1] = 0x35;
        pchMessageStart[2] = 0x22;
        pchMessageStart[3] = 0x05;
        nDefaultPort = 58684;
        m_assumed_blockchain_size = 3;
        m_assumed_chain_state_size = 10;

        genesis = CreateGenesisBlock(strNetworkID, 1399690945, 612416, 0x1e0fffff, 1, 2500 * COIN);
        consensus.hashGenesisBlock = genesis.GetWorkHash();

        assert(consensus.hashGenesisBlock == uint256S("0x000004da58a02be894a6c916d349fe23cc29e21972cafb86b5d3f07c4b8e6bb8"));
        assert(genesis.hashMerkleRoot == uint256S("0x60424046d38de827de0ed1a20a351aa7f3557e3e1d3df6bfb34a94bc6161ec68"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.emplace_back("seed.vrc.vericonomy.com");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,70);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,132);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128+70);
        base58Prefixes[EXT_PUBLIC_KEY] = {0xE3, 0xCC, 0xBB, 0x92};
        base58Prefixes[EXT_SECRET_KEY] = {0xE3, 0xCC, 0xAE, 0x01};

        bech32_hrp = "vry";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_vericoin, pnSeed6_vericoin + ARRAYLEN(pnSeed6_vericoin));

        fIsVericoin = true;
        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        m_is_test_chain = false;

        checkpointData = {
            {
                {2700, uint256S("52f0119bd2252422ea4aebb25273a98155972cf25a6ef267a7ef35103b5466c3")},
            }
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 000000000000056c49030c174179b52a928c870e6e8a822c75973b7970cfbd01
            /* nTime    */ 1499513240,
            /* nTxCount */ 1499513240,
            /* dTxRate  */ 0.0013,
        };
    }
};

/**
 * Verium
 */
class CVeriumParams : public CChainParams {
public:
    CVeriumParams() {
        strNetworkID = CBaseChainParams::VERIUM;
        consensus.BIP34Height = 1;
        consensus.BIP65Height = 600000; // 00000000007f6655f22f98e72ed80d8b06dc761d5da09df0fa1dc4be4f861eb6
        consensus.BIP66Height = 600000; // 000000002104c8c45e99a8853285a3b592602a3ccde2b832481da85e9e4ba182
        consensus.CSVHeight = 600000; // 00000000025e930139bac5c6c31a403776da130831ab85be56578f3fa75369bb

        /** Coin Settings **/
        consensus.VIP1Height = 520000; // Change Min Fee
        consensus.nMaturity = 100;
        consensus.NextTargetV2Height = 0; // No use for verium
        consensus.PoSTHeight = 0; // No use for verium
        consensus.PoSHeight = 0; // No use for verium
        consensus.nInitialCoinSupply = 0;

        /** PoWS Settings **/
        consensus.powLimit = uint256S("001fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetSpacing = 5 * 60;  // not used for consensus in Verium as it's variable, but used to indicate age of data
        consensus.nPowTargetTimespan = 2 * 24 * 60 * 60; // two days
        consensus.fPowNoRetargeting = false;

        /** PoST Settings **/
        consensus.posLimit = uint256S("001fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // No use for verium
        consensus.nStakeTargetSpacing = 0; // No use for verium
        consensus.nStakeMinAge = 0; // No use for verium
        consensus.nModifierInterval = 0; // No use for verium
        consensus.nTargetTimespan = 0;

        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000000000000");
        consensus.defaultAssumeValid = uint256S("0x0000000000000000000000000000000000000000000000000000000000000000"); // 654683

        pchMessageStart[0] = 0x70;
        pchMessageStart[1] = 0x35;
        pchMessageStart[2] = 0x22;
        pchMessageStart[3] = 0x05;
        nDefaultPort = 36988;
        m_assumed_blockchain_size = 1;
        m_assumed_chain_state_size = 4;

        genesis = CreateGenesisBlock(strNetworkID, 1472669240, 233180, 0x1f1fffff, 1, 2500 * COIN);
        consensus.hashGenesisBlock = genesis.GetVeriumHash();
        assert(consensus.hashGenesisBlock == uint256S("0x8232c0cf3bd7e05546e3d7aaaaf89fed8bc97c4df1a8c95e9249e13a2734932b"));
        assert(genesis.hashMerkleRoot == uint256S("0x925e430072a1f39b530fc79db162e29433ab0ea266a99c8cab4f03001dc9faa9"));

        vFixedSeeds.clear();
        vSeeds.clear();

        vSeeds.emplace_back("seed.vrm.vericonomy.com");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,70);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,132);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128+70);
        base58Prefixes[EXT_PUBLIC_KEY] = {0xE3, 0xCC, 0xBB, 0x92};
        base58Prefixes[EXT_SECRET_KEY] = {0xE3, 0xCC, 0xAE, 0x01};

        bech32_hrp = "vry";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_verium, pnSeed6_verium + ARRAYLEN(pnSeed6_verium));

        fIsVericoin = false;
        fMiningRequiresPeers = true;

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        m_is_test_chain = false;


        checkpointData = {
            {
                {     1, uint256S("0x3f2566fc0abcc9b2e26c737d905ff3e639a49d44cd5d11d260df3cfb62663012")},
                {  1500, uint256S("0x0458cc7c7093cea6e78eed03a8f57d0eed200aaf5171eea82e63b8e643891cce")},
                {100000, uint256S("0x0510c6cb8c5a2a5437fb893853f10e298654361a05cf611b1c54c1750dfbdad6")},
            }
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 00000000000000b7ab6ce61eb6d571003fbe5fe892da4c9b740c49a07542462d
            /* nTime    */ 1499513240,
            /* nTxCount */ 1499513240,
            /* dTxRate  */ 0.0013,
        };
    }
};

static std::unique_ptr<const CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<const CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::VERICOIN)
        return std::unique_ptr<CChainParams>(new CVericoinParams());
    else if (chain == CBaseChainParams::VERIUM)
        return std::unique_ptr<CChainParams>(new CVeriumParams());
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}
