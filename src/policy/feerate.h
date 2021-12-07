// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_POLICY_FEERATE_H
#define BITCOIN_POLICY_FEERATE_H

#include <amount.h>
#include <serialize.h>

#include <string>

const std::string CURRENCY_UNIT = "BTC"; // One formatted unit
const std::string CURRENCY_ATOM = "sat"; // One indivisible minimum value unit

/**
 * Fee rate in satoshis per kilobyte: CAmount / kB
 */
class CFeeRate
{
private:
    CAmount nSatoshisPerK; // unit is satoshis-per-1,000-bytes
    bool withStartFee = false; // if true, we add nSatoshisPerK to the fee has a min start.

public:
    /** Fee rate of 0 satoshis per kB */
    CFeeRate() : nSatoshisPerK(0) { }
    template<typename I>
    explicit CFeeRate(const I _nSatoshisPerK, bool _withStartFee = false): nSatoshisPerK(_nSatoshisPerK), withStartFee(_withStartFee)  {
        // We've previously had bugs creep in from silent double->int conversion...
        static_assert(std::is_integral<I>::value, "CFeeRate should be used without floats");
    }
    /** Constructor for a fee rate in satoshis per kvB (sat/kvB).
     *
     *  Passing a num_bytes value of COIN (1e8) returns a fee rate in satoshis per vB (sat/vB),
     *  e.g. (nFeePaid * 1e8 / 1e3) == (nFeePaid / 1e5),
     *  where 1e5 is the ratio to convert from BTC/kvB to sat/vB.
     */
    CFeeRate(const CAmount& nFeePaid, uint32_t num_bytes, bool with_start_fee = false);
    /**
     * Return the fee in satoshis for the given size in bytes.
     */
    CAmount GetFee(uint32_t num_bytes, bool add_start_fee = false) const;
    /**
     * Return the fee in satoshis for a size of 1000 bytes
     */
    CAmount GetFeePerK(bool add_start_fee = false) const { return GetFee(1000, add_start_fee); }
    friend bool operator<(const CFeeRate& a, const CFeeRate& b) { return a.nSatoshisPerK < b.nSatoshisPerK; }
    friend bool operator>(const CFeeRate& a, const CFeeRate& b) { return a.nSatoshisPerK > b.nSatoshisPerK; }
    friend bool operator==(const CFeeRate& a, const CFeeRate& b) { return a.nSatoshisPerK == b.nSatoshisPerK; }
    friend bool operator<=(const CFeeRate& a, const CFeeRate& b) { return a.nSatoshisPerK <= b.nSatoshisPerK; }
    friend bool operator>=(const CFeeRate& a, const CFeeRate& b) { return a.nSatoshisPerK >= b.nSatoshisPerK; }
    friend bool operator!=(const CFeeRate& a, const CFeeRate& b) { return a.nSatoshisPerK != b.nSatoshisPerK; }
    CFeeRate& operator+=(const CFeeRate& a) { nSatoshisPerK += a.nSatoshisPerK; return *this; }
    std::string ToString() const;

    SERIALIZE_METHODS(CFeeRate, obj) { READWRITE(obj.nSatoshisPerK); }
};

#endif //  BITCOIN_POLICY_FEERATE_H
