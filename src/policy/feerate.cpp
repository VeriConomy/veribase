// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <policy/feerate.h>

#include <tinyformat.h>

CFeeRate::CFeeRate(const CAmount& nFeePaid, uint32_t num_bytes, bool with_start_fee)
{
    int64_t nSize{num_bytes};

    if (nSize > 0) {
        if (with_start_fee) {
            nSize -= 1000;
            if (nSize <= 0) {
                nSatoshisPerK = 0;
                return;
            }
        }

         nSatoshisPerK = nFeePaid * 1000 / nSize;
    } else {
        nSatoshisPerK = 0;
    }
}

CAmount CFeeRate::GetFee(uint32_t num_bytes, bool add_start_fee) const
{
    const int64_t nSize{num_bytes};

   CAmount nFee = nSatoshisPerK * int(nSize / 1000);

    // Let's add startFee that are equal to nSatoshisPerK
    if (add_start_fee)
        nFee += nSatoshisPerK;

    if (nFee == 0 && nSize != 0) {
        if (nSatoshisPerK > 0) nFee = CAmount(1);
        if (nSatoshisPerK < 0) nFee = CAmount(-1);
    }

    return nFee;
}

std::string CFeeRate::ToString() const
{
    if (withStartFee)
        return strprintf("minfee: %d.%08d %s, feerate:%d.%08d %s/kB", nSatoshisPerK / COIN, nSatoshisPerK % COIN, CURRENCY_UNIT, nSatoshisPerK / COIN, nSatoshisPerK % COIN, CURRENCY_UNIT);
    else
        return strprintf("%d.%08d %s/kB", nSatoshisPerK / COIN, nSatoshisPerK % COIN, CURRENCY_UNIT);
}
