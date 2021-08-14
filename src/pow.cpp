// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <version.h>
#include <arith_uint256.h>
#include <chain.h>
#include <chainparams.h>
#include <primitives/block.h>
#include <uint256.h>
#include <math.h>
#include <bignum.h>
#include <util/system.h>
#include <timedata.h>
#include <validation.h>

#include <inttypes.h>

double GetDifficulty(const CBlockIndex* blockindex = nullptr);

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const Consensus::Params& params)
{
    if (pindexLast->nHeight <= 2)
        return UintToArith256(params.powLimit).GetCompact(); // first few blocks
    const CBlockIndex* pindexPrev = pindexLast->pprev;
    const CBlockIndex* pindexPrevPrev = pindexPrev->pprev;
    unsigned int nTargetSpacing = CalculateBlocktime(pindexPrev);
    int64_t nActualSpacing = pindexPrev->GetBlockTime() - pindexPrevPrev->GetBlockTime();
    int64_t targetTimespan;

    // Two hour target timespan on launch needed to prevent accelerated blocktime while difficulty first equilibrates
    if (pindexLast->nHeight+1 <= 2394) {
        targetTimespan = 2 * 60 * 60;
    }
    else {
        // 48 hour normal target timespan with more stable difficulty equilibrium
        targetTimespan = params.nPowTargetTimespan;
    }

    // ppcoin: retarget with exponential moving toward target spacing (variable in Verium)
    CBigNum bnNew;
    bnNew.SetCompact(pindexPrev->nBits);
    int64_t nInterval = targetTimespan / nTargetSpacing;
    bnNew *= ((nInterval - 1) * nTargetSpacing + nActualSpacing + nActualSpacing);
    bnNew /= ((nInterval + 1) * nTargetSpacing);

    if (bnNew > CBigNum(params.powLimit))
        bnNew = CBigNum(params.powLimit);

    return bnNew.GetCompact();
}

// Check whether a block hash satisfies the proof-of-work requirement specified by nBits
bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    CBigNum bnTarget;
    bnTarget.SetCompact(nBits);

    // Check range
    if (bnTarget <= 0 || bnTarget > CBigNum(params.powLimit))
        return error("CheckProofOfWork() : nBits below minimum work");

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > UintToArith256(bnTarget.getuint256()))
        return error("CheckProofOfWork() : hash doesn't match nBits");

    return true;
}

CAmount calculateMinerReward(const CBlockIndex* pindex)
{
    int64_t nReward;
    unsigned int nBlockTime = CalculateBlocktime(pindex);
    int height = pindex->nHeight+1;
    if (height == 1)
    {
        nReward = 564705 * COIN; // Verium purchased in presale ICO
    }
    else if ((pindex->nMoneySupply/COIN) > 2899999)
    {
        double dReward = 0.04*exp(0.0116*nBlockTime); // Reward schedule after 10x VRC supply parity
        nReward = dReward * COIN;
    }
    else
    {
        double dReward = 0.25*exp(0.0116*nBlockTime); // Reward schedule up to 10x VRC supply parity
        nReward = dReward * COIN;
    }
    return nReward;
}

// miner's coin base reward
int64_t GetProofOfWorkReward(int64_t nFees,const CBlockIndex* pindex)
{
    if( IsVericoin() )
    {
        return (2500 * COIN) + nFees;
    }

    CAmount nSubsidy = calculateMinerReward(pindex);
    return nSubsidy + nFees;
}

// Get block time
unsigned int CalculateBlocktime(const CBlockIndex* pindex)
{
    unsigned int nBlockTime;
    double diff = GetDifficulty(pindex);
    double dBlockTime = -13.03*log(diff)+180;
    if (dBlockTime < 15)
    {
        nBlockTime = 15;
    }
    else
    {
        nBlockTime = dBlockTime;
    }
    return nBlockTime;
}

// Get the block rate for one hour
int GetBlockRatePerHour()
{
    int nRate = 0;
    CBlockIndex* pindex = ::ChainActive().Tip();
    int64_t nTargetTime = GetAdjustedTime() - 3600;

    while (pindex && pindex->pprev && pindex->nTime > nTargetTime) {
        nRate += 1;
        pindex = pindex->pprev;
    }
    return nRate;
}

double GetPoWKHashPM(const CChainParams& params)
{
    if( params.IsVericoin() && ::ChainActive().Tip()->nHeight > params.GetConsensus().PoSHeight)
        return 0;

    int nPoWInterval = 72;
    int64_t nTargetSpacingWorkMin = 30, nTargetSpacingWork = 30;
    CBlockIndex* pindex = ::ChainActive().Genesis();
    CBlockIndex* pindexPrevWork = ::ChainActive().Genesis();
     while (pindex)
    {
        int64_t nActualSpacingWork = pindex->GetBlockTime() - pindexPrevWork->GetBlockTime();
        nTargetSpacingWork = ((nPoWInterval - 1) * nTargetSpacingWork + nActualSpacingWork + nActualSpacingWork) / (nPoWInterval + 1);
        nTargetSpacingWork = std::max(nTargetSpacingWork, nTargetSpacingWorkMin);
        pindexPrevWork = pindex;
        pindex = ::ChainActive()[pindex->nHeight+1];
    }

    return (GetDifficulty(::ChainActive().Tip()) * 1024 * 4294.967296  / nTargetSpacingWork) * 60;  // 60= sec to min, 1024= standard scrypt work to scrypt^2
}