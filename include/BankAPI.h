#pragma once

#include <string>
#include <vector>

// Abstract interface representing communication with the bank's backend
// system. A production implementation would talk to the bank over a
// network (REST/RPC/etc.), but that is out of scope here. The ATM
// controller only ever depends on this interface, so a real
// implementation can be plugged in later without touching controller code.
//
// Note: the bank never hands the PIN back to the ATM - it only confirms
// whether a given PIN was correct via verifyPin().
class BankAPI {
public:
    virtual ~BankAPI() = default;

    // Returns true if `pin` is the correct PIN for `cardNumber`.
    virtual bool verifyPin(const std::string& cardNumber,
                            const std::string& pin) = 0;

    // Returns the list of account IDs associated with a card.
    virtual std::vector<std::string> getAccountIds(
        const std::string& cardNumber) = 0;

    // Returns the current balance (whole dollars) for an account.
    virtual int getBalance(const std::string& accountId) = 0;

    // Deposits `amount` whole dollars into the account.
    // Returns true on success.
    virtual bool deposit(const std::string& accountId, int amount) = 0;

    // Withdraws `amount` whole dollars from the account.
    // Returns true on success, false if the bank rejects it
    // (e.g. insufficient funds, frozen account, etc.).
    virtual bool withdraw(const std::string& accountId, int amount) = 0;
};
