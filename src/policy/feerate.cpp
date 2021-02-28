// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <policy/feerate.h>

#include <tinyformat.h>
#include <cmath>

const std::string CURRENCY_UNIT = "BTC";

CFeeRate::CFeeRate(const CAmount& nFeePaid, size_t nBytes_, bool _withStartFee)
{
    assert(nBytes_ <= uint64_t(std::numeric_limits<int64_t>::max()));
    int64_t nSize = int64_t(nBytes_);

    withStartFee = _withStartFee;

    if (nSize > 0) {
        if (withStartFee) {
            nSize -= 1000;
            if (nSize <= 0) {
                nSatoshisPerK = 0;
                return;
            }
        }

         nSatoshisPerK = nFeePaid * 1000 / nSize;
    }
    else
        nSatoshisPerK = 0;
}

CAmount CFeeRate::GetFee(size_t nBytes_, bool addStartFee) const
{
    assert(nBytes_ <= uint64_t(std::numeric_limits<int64_t>::max()));
    int64_t nSize = int64_t(nBytes_);

    CAmount nFee = nSatoshisPerK * floor(nSize / 1000);

    // Let's add startFee that are equal to nSatoshisPerK
    if (addStartFee)
        nFee += nSatoshisPerK;

    if (nFee == 0 && nSize != 0) {
        if (nSatoshisPerK > 0)
            nFee = CAmount(1);
        if (nSatoshisPerK < 0)
            nFee = CAmount(-1);
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
