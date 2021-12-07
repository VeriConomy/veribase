// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <wallet/fees.h>

#include <wallet/coincontrol.h>
#include <wallet/wallet.h>


CAmount GetRequiredFee(const CWallet& wallet, unsigned int nTxBytes)
{
    CAmount nMinFee = wallet.chain().getMinTxFeeRate().GetFee(nTxBytes, true);

    if (!MoneyRange(nMinFee))
        nMinFee = MAX_MONEY;

    return nMinFee;
}


CAmount GetMinimumFee(const CWallet& wallet, unsigned int nTxBytes)
{
    return GetRequiredFee(wallet, nTxBytes);
}

CFeeRate GetRequiredFeeRate(const CWallet& wallet)
{
    return wallet.chain().relayMinFee();
}

CFeeRate GetPayTxFee(const CWallet& wallet) {
    if(wallet.f_enforce_pay_tx_fee)
        return wallet.m_pay_tx_fee;
    else
        return wallet.chain().getMinTxFeeRate();
}

CFeeRate GetMinimumFeeRate(const CWallet& wallet)
{
    CFeeRate feerate_needed = GetPayTxFee(wallet);

    // prevent user from paying a fee below the required fee rate
    CFeeRate required_feerate = GetRequiredFeeRate(wallet);
    if (required_feerate > feerate_needed) {
        feerate_needed = required_feerate;
    }
    return feerate_needed;
}

CFeeRate GetDiscardRate(const CWallet& wallet)
{
    return wallet.m_discard_rate;
}
