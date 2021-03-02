// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_POW_H
#define BITCOIN_POW_H

#include <amount.h>
#include <consensus/params.h>

#include <stdint.h>

class CBlockHeader;
class CBlockIndex;
class uint256;

/** Get next required mining work **/
unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const Consensus::Params& params);
unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params&);

/** Check whether a block hash satisfies the proof-of-work requirement specified by nBits */
bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params&);

/** Get reward amount for a solved work **/
CAmount GetProofOfWorkReward(int64_t nFees,const CBlockIndex* pindex);

/** Get block time **/
unsigned int CalculateBlocktime(const CBlockIndex *pindex);

/** Get Block rate per hour **/
int GetBlockRatePerHour();

#endif // BITCOIN_POW_H
