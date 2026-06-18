#include "ATMController.h"

ATMController::ATMController(BankAPI* bankApi)
    : bankApi_(bankApi), state_(ATMState::IDLE) {}

void ATMController::reset() {
    state_ = ATMState::IDLE;
    currentCard_.clear();
    currentAccount_.clear();
}

ATMResult ATMController::insertCard(const std::string& cardNumber) {
    if (state_ != ATMState::IDLE) {
        return ATMResult::INVALID_STATE;
    }
    currentCard_ = cardNumber;
    state_ = ATMState::AWAITING_PIN;
    return ATMResult::SUCCESS;
}

ATMResult ATMController::enterPin(const std::string& pin) {
    if (state_ != ATMState::AWAITING_PIN) {
        return ATMResult::INVALID_STATE;
    }

    if (!bankApi_->verifyPin(currentCard_, pin)) {
        return ATMResult::INCORRECT_PIN;
    }

    state_ = ATMState::AWAITING_ACCOUNT_SELECTION;
    return ATMResult::SUCCESS;
}

ATMResult ATMController::selectAccount(const std::string& accountId) {
    if (state_ != ATMState::AWAITING_ACCOUNT_SELECTION) {
        return ATMResult::INVALID_STATE;
    }

    auto accounts = bankApi_->getAccountIds(currentCard_);
    bool found = false;
    for (const auto& acc : accounts) {
        if (acc == accountId) {
            found = true;
            break;
        }
    }
    if (!found) {
        return ATMResult::ACCOUNT_NOT_FOUND;
    }

    currentAccount_ = accountId;
    state_ = ATMState::ACCOUNT_SELECTED;
    return ATMResult::SUCCESS;
}

ATMResult ATMController::getBalance(int& balanceOut) {
    if (state_ != ATMState::ACCOUNT_SELECTED) {
        return ATMResult::INVALID_STATE;
    }
    balanceOut = bankApi_->getBalance(currentAccount_);
    return ATMResult::SUCCESS;
}

ATMResult ATMController::deposit(int amount) {
    if (state_ != ATMState::ACCOUNT_SELECTED) {
        return ATMResult::INVALID_STATE;
    }
    if (amount <= 0) {
        return ATMResult::INVALID_AMOUNT;
    }

    if (!bankApi_->deposit(currentAccount_, amount)) {
        return ATMResult::FAILURE;
    }
    return ATMResult::SUCCESS;
}

ATMResult ATMController::withdraw(int amount) {
    if (state_ != ATMState::ACCOUNT_SELECTED) {
        return ATMResult::INVALID_STATE;
    }
    if (amount <= 0) {
        return ATMResult::INVALID_AMOUNT;
    }
    if (amount > bankApi_->getBalance(currentAccount_)) {
        return ATMResult::INSUFFICIENT_FUNDS;
    }

    if (!bankApi_->withdraw(currentAccount_, amount)) {
        return ATMResult::FAILURE;
    }
    return ATMResult::SUCCESS;
}

ATMResult ATMController::cancel() {
    reset();
    return ATMResult::SUCCESS;
}
