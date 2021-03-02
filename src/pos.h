// Copyright (c) 2016-2020 The Vericonomy developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef BITCOIN_POS_H
#define BITCOIN_POS_H

#include <amount.h>
#include <consensus/params.h>
#include <primitives/transaction.h>

#include <stdint.h>


class CBlockHeader;
class CBlockIndex;
class CBlock;
class CWallet;
class CTransaction;
class CCoinsViewCache;
class uint256;
class BlockValidationState;

/** Get next required staking work **/
unsigned int GetNextTargetRequired(const CBlockIndex* pindexLast, bool fProofOfStake, const Consensus::Params& params);

double GetPoSKernelPS(CBlockIndex* pindexPrev, const Consensus::Params& params);
double GetPoSKernelPS(CBlockIndex* pindexPrev);
double GetPoSKernelPS();

double GetCurrentInflationRate(double nAverageWeight);
double GetCurrentInterestRate(CBlockIndex* pindexPrev, const Consensus::Params& params);
double GetAverageStakeWeight(CBlockIndex* pindexPrev);
int64_t GetStakeTimeFactoredWeight(int64_t timeWeight, int64_t bnCoinDayWeight, CBlockIndex* pindexPrev);

/** Get reward amount for a solved work **/
int64_t GetProofOfStakeReward(int64_t nCoinAge, int64_t nFees, CBlockIndex* pindex, const Consensus::Params& params);

bool GetCoinAge(const CTransaction& tx, const CCoinsViewCache &view, uint64_t& nCoinAge, CBlockIndex* pindexPre);
bool SignBlock(CBlock& block, const CWallet& keystore);
bool CheckBlockSignature(const CBlock& block);

static const double PI = 3.1415926535;
// MODIFIER_INTERVAL_RATIO:
// ratio of group interval length between the last group and the first group
static const int MODIFIER_INTERVAL_RATIO = 3;

// Compute the hash modifier for proof-of-stake
bool ComputeNextStakeModifier(const CBlockIndex* pindexCurrent, uint64_t& nStakeModifier, bool& fGeneratedStakeModifier);

// Check whether stake kernel meets hash target
// Sets hashProofOfStake on success return
bool CheckStakeKernelHash(unsigned int nBits, CBlockIndex* pindexPrev, const CBlockHeader& blockFrom, unsigned int nTxPrevOffset, const CTransactionRef& txPrev, const COutPoint& prevout, unsigned int nTimeTx, uint256& hashProofOfStake, bool fPrintProofOfStake=false);

// Check kernel hash target and coinstake signature
// Sets hashProofOfStake on success return
bool CheckProofOfStake(BlockValidationState &state, CBlockIndex* pindexPrev, const CTransactionRef &tx, unsigned int nBits, uint256& hashProofOfStake);

// Check whether the coinstake timestamp meets protocol
bool CheckCoinStakeTimestamp(int64_t nTimeBlock, int64_t nTimeTx);

// Get stake modifier checksum
unsigned int GetStakeModifierChecksum(const CBlockIndex* pindex);

// Check stake modifier hard checkpoints
bool CheckStakeModifierCheckpoints(int nHeight, unsigned int nStakeModifierChecksum);

// peercoin: entropy bit for stake modifier if chosen by modifier
unsigned int GetStakeEntropyBit(const CBlock& block);

#endif // BITCOIN_POS_H