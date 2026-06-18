#pragma once

#include <string>
#include "BankAPI.h"

// Result of an ATM operation. An explicit result code (rather than
// exceptions) keeps control flow simple and easy to assert on in tests.
enum class ATMResult {
    SUCCESS,
    INVALID_STATE,       // operation not allowed in the controller's current state
    INCORRECT_PIN,
    ACCOUNT_NOT_FOUND,
    INSUFFICIENT_FUNDS,
    INVALID_AMOUNT,       // e.g. zero or negative amount requested
    FAILURE               // bank rejected an otherwise-valid deposit/withdraw
};

// The flow this controller implements:
//   IDLE -> insertCard() -> AWAITING_PIN
//   AWAITING_PIN -> enterPin() -> AWAITING_ACCOUNT_SELECTION
//   AWAITING_ACCOUNT_SELECTION -> selectAccount() -> ACCOUNT_SELECTED
//   ACCOUNT_SELECTED -> getBalance() / deposit() / withdraw()
//   any state -> cancel() -> IDLE
enum class ATMState {
    IDLE,
    AWAITING_PIN,
    AWAITING_ACCOUNT_SELECTION,
    ACCOUNT_SELECTED
};

// Core ATM business logic. Has no knowledge of any UI, network protocol,
// or real hardware - it only depends on the BankAPI interface, injected
// through the constructor. This makes it fully testable with a mock bank,
// and reusable by whatever UI layer gets built on top of it.
class ATMController {
public:
    explicit ATMController(BankAPI* bankApi);

    ATMResult insertCard(const std::string& cardNumber);
    ATMResult enterPin(const std::string& pin);
    ATMResult selectAccount(const std::string& accountId);
    ATMResult getBalance(int& balanceOut);
    ATMResult deposit(int amount);
    ATMResult withdraw(int amount);
    ATMResult cancel();

    ATMState getState() const { return state_; }

private:
    BankAPI* bankApi_;
    ATMState state_;
    std::string currentCard_;
    std::string currentAccount_;

    void reset();
};
