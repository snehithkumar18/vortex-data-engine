#include "tests/test_framework.h"
#include "vde/transaction/transaction_manager.h"

TEST(transaction_manager_begin_commit) {
    vde::TransactionManager& tm = vde::TransactionManager::instance();
    vde::Transaction* tx = tm.begin_transaction();
    ASSERT_NE(tx, nullptr);
    ASSERT_EQ(static_cast<int>(tx->state()), static_cast<int>(vde::TransactionState::Active));

    tm.commit(tx);
    ASSERT_EQ(static_cast<int>(tx->state()), static_cast<int>(vde::TransactionState::Committed));
}

RUN_ALL_TESTS()
