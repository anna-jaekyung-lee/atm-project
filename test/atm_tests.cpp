#include <iostream>
#include "ATMController.h"
#include "FakeBankAPI.h"
using namespace std;

int testsRun = 0;
int testsPassed = 0;

// Small helper: print PASS/FAIL for one check, same idea as printing
// debug values like we did while debugging StringChallenge/GameChallenge.
void check(bool condition, const string& testName) {
    testsRun++;
    if (condition) {
        testsPassed++;
        cout << "[PASS] " << testName << endl;
    } else {
        cout << "[FAIL] " << testName << endl;
    }
}

// Covers the required flow end-to-end:
// Insert Card -> PIN -> Select Account -> Balance / Deposit / Withdraw
void testFullHappyPathFlow() {
    FakeBankAPI bank;        // balance starts at 100 by default
    ATMController atm(&bank);

    check(atm.insertCard("1234") == ATMResult::SUCCESS, "insert card succeeds");
    check(atm.enterPin("0000") == ATMResult::SUCCESS, "correct pin succeeds");
    check(atm.selectAccount("acc1") == ATMResult::SUCCESS, "select known account succeeds");

    int balance = -1;
    atm.getBalance(balance);
    check(balance == 100, "balance starts at 100");

    check(atm.deposit(30) == ATMResult::SUCCESS, "deposit succeeds");
    check(atm.withdraw(50) == ATMResult::SUCCESS, "withdraw succeeds");
    check(bank.balance == 80, "balance is 100 + 30 - 50 = 80 after both");
}

// "A bank API wouldn't give the ATM the PIN number, but it can tell you
// if the PIN number is correct or not." - the controller must respect that.
void testIncorrectPinIsRejected() {
    FakeBankAPI bank;
    bank.pinIsCorrect = false;
    ATMController atm(&bank);

    atm.insertCard("1234");
    check(atm.enterPin("wrong") == ATMResult::INCORRECT_PIN, "wrong pin rejected");
    check(atm.getState() == ATMState::AWAITING_PIN, "still awaiting pin after wrong attempt");
}

void testSelectingUnknownAccountFails() {
    FakeBankAPI bank;
    bank.accounts = {"acc1"};
    ATMController atm(&bank);

    atm.insertCard("1234");
    atm.enterPin("0000");
    check(atm.selectAccount("nope") == ATMResult::ACCOUNT_NOT_FOUND, "unknown account rejected");
}

// Balance is a whole-dollar int; withdrawing more than the balance must fail.
void testWithdrawFailsWhenInsufficientFunds() {
    FakeBankAPI bank;
    bank.balance = 10;
    ATMController atm(&bank);

    atm.insertCard("1234");
    atm.enterPin("0000");
    atm.selectAccount("acc1");

    check(atm.withdraw(50) == ATMResult::INSUFFICIENT_FUNDS, "withdraw more than balance fails");
    check(bank.balance == 10, "balance unchanged after failed withdraw");
}

void testOperationsRequireCorrectState() {
    FakeBankAPI bank;
    ATMController atm(&bank);

    int balance;
    check(atm.enterPin("0000") == ATMResult::INVALID_STATE, "can't enter pin before inserting card");
    check(atm.getBalance(balance) == ATMResult::INVALID_STATE, "can't check balance before selecting account");
}

void testCancelResetsSessionForNextCustomer() {
    FakeBankAPI bank;
    ATMController atm(&bank);

    atm.insertCard("1234");
    atm.cancel();
    check(atm.getState() == ATMState::IDLE, "cancel returns to idle");
    check(atm.insertCard("5678") == ATMResult::SUCCESS, "next customer can insert a new card");
}

int main() {
    testFullHappyPathFlow();
    testIncorrectPinIsRejected();
    testSelectingUnknownAccountFails();
    testWithdrawFailsWhenInsufficientFunds();
    testOperationsRequireCorrectState();
    testCancelResetsSessionForNextCustomer();

    cout << endl << testsPassed << "/" << testsRun << " tests passed" << endl;
    return (testsPassed == testsRun) ? 0 : 1;
}
