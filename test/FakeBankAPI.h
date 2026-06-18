#pragma once

#include "BankAPI.h"

// A "fake" stand-in for the real bank, used only in tests. Instead of a
// mocking framework, this is just a plain class: set the public fields
// to whatever you want the bank to "say" before running the test, then
// hand it to ATMController.
class FakeBankAPI : public BankAPI {
public:
    bool pinIsCorrect = true;
    std::vector<std::string> accounts = {"acc1"};
    int balance = 100;
    bool depositSucceeds = true;
    bool withdrawSucceeds = true;

    bool verifyPin(const std::string& cardNumber, const std::string& pin) override {
        return pinIsCorrect;
    }

    std::vector<std::string> getAccountIds(const std::string& cardNumber) override {
        return accounts;
    }

    int getBalance(const std::string& accountId) override {
        return balance;
    }

    bool deposit(const std::string& accountId, int amount) override {
        if (depositSucceeds) balance += amount;
        return depositSucceeds;
    }

    bool withdraw(const std::string& accountId, int amount) override {
        if (withdrawSucceeds) balance -= amount;
        return withdrawSucceeds;
    }
};
