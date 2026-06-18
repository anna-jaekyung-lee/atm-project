# ATM Controller

A simplified ATM controller implementing the required flow:

```
Insert Card -> Enter PIN -> Select Account -> Balance / Deposit / Withdraw
```

Money is represented as whole-dollar `int`s (no cents), since only $1
bills exist in this simplified world.

## Design

`ATMController` is the testable business logic / state machine. It has
no knowledge of any UI, network protocol, or real hardware - it depends
only on the `BankAPI` interface, passed in through its constructor. A
real bank integration would be a new class that inherits from `BankAPI`
and implements its functions for real; `ATMController` itself would not
need to change at all. Note that the bank never exposes the actual PIN
to the ATM - `verifyPin()` only returns whether it was correct.

`BankAPI` is an abstract class: it declares its functions as `virtual`
with no body, which means "any class that inherits from me must provide
its own version of this function." `ATMController` only ever talks to a
`BankAPI*` pointer, so at runtime it doesn't matter whether that pointer
points to a real bank connection or, as in the tests, a fake one - the
right version of the function runs either way.

```
include/
  BankAPI.h          // bank interface (implement for real integration)
  ATMController.h     // controller's public API and state machine
src/
  ATMController.cpp   // controller implementation
test/
  FakeBankAPI.h        // plain stand-in for the bank, used only in tests
  atm_tests.cpp        // test runner (no external test framework)
```

### State machine

```
IDLE --insertCard()--> AWAITING_PIN --enterPin()--> AWAITING_ACCOUNT_SELECTION
                                                            |
                                                     selectAccount()
                                                            v
                                                    ACCOUNT_SELECTED
                                          (getBalance() / deposit() / withdraw())
```

`cancel()` returns the controller to `IDLE` from any state. Every
method returns an `ATMResult` enum (`SUCCESS`, `INVALID_STATE`,
`INCORRECT_PIN`, `ACCOUNT_NOT_FOUND`, `INSUFFICIENT_FUNDS`,
`INVALID_AMOUNT`, `FAILURE`) rather than throwing, so callers and tests
can branch on outcomes explicitly.

## Clone, Build & Run Tests

Requires only a C++17 compiler (e.g. g++) - no build system or test
framework to install.

```bash
git clone <repo-url>
cd atm-project
g++ -std=c++17 -Iinclude -Itest src/ATMController.cpp test/atm_tests.cpp -o atm_tests
./atm_tests
```

Each test prints `[PASS]`/`[FAIL]` and the program ends with a summary
line and a non-zero exit code if anything failed. Tests cover the full
required flow (insert card -> correct PIN -> select account -> balance
/ deposit / withdraw), incorrect PIN, unknown account, insufficient
funds, wrong-state calls, and session reset via `cancel()`.

## Out of Scope / Future Integration

- Real bank and ATM hardware (card reader, cash bin) integration are
  out of scope by design. `BankAPI` is the seam for plugging in a real
  bank backend; a similar interface for the physical cash bin could be
  added the same way if/when that integration is needed.
- No persistence across process restarts - state lives in memory in
  the `ATMController` instance for the duration of one session.
