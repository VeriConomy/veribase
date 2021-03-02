// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CONSENSUS_PARAMS_H
#define BITCOIN_CONSENSUS_PARAMS_H

#include <arith_uint256.h>
#include <uint256.h>
#include <limits>

namespace Consensus {

/**
 * Parameters that influence chain consensus.
 */
struct Params {
    uint256 hashGenesisBlock;
    /** Block height and hash at which BIP34 becomes active */
    int BIP34Height;
    /** Block height at which BIP65 becomes active */
    int BIP65Height;
    /** Block height at which BIP66 becomes active */
    int BIP66Height;
    /** Block height at which CSV (BIP68, BIP112 and BIP113) becomes active */
    int CSVHeight;

    /** Vericoin Params **/
    uint256 posLimit;
    int NextTargetV2Height;
    int PoSTHeight;
    int PoSHeight;
    int nStakeTargetSpacing;
    int nStakeMinAge;
    int nModifierInterval;

    /** Proof of work parameters */
    uint256 powLimit;
    bool fPowAllowMinDifficultyBlocks;
    bool fPowNoRetargeting;
    int64_t nPowTargetSpacing;
    int64_t nPowTargetTimespan;
    uint256 nMinimumChainWork;
    uint256 defaultAssumeValid;

    /** coin params **/
    int nMaturity;
    int nTargetTimespan;
    int nInitialCoinSupply;
    int VIP1Height;

};
} // namespace Consensus

#endif // BITCOIN_CONSENSUS_PARAMS_H
