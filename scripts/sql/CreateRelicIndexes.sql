/*
  Performance indexes for FIO.Relic tables.
  Created with IF NOT EXISTS so the script is safe to re-run.

  NOTE: Indexes on PRIMARY KEY columns (pk_block_number, pk_account_id,
        pk_handle_id, etc.) are omitted because PostgreSQL already creates
        an implicit unique B-tree index for every PRIMARY KEY constraint.
*/

-- ============================================================
-- blocks
-- ============================================================
-- (pk_block_number is PRIMARY KEY – implicit index already exists)

-- ============================================================
-- transactions
-- ============================================================
CREATE INDEX IF NOT EXISTS transactions_block_number_idx
  ON transactions (fk_block_number);

CREATE INDEX IF NOT EXISTS transactions_block_timestamp_idx
  ON transactions (block_timestamp DESC);

CREATE INDEX IF NOT EXISTS idx_transactions_fk_account_id
  ON transactions (fk_account_id);

CREATE INDEX IF NOT EXISTS idx_transactions_transaction_id
  ON transactions (transaction_id);

CREATE INDEX IF NOT EXISTS idx_transactions_fk_action_account_id
  ON transactions (fk_action_account_id);

CREATE INDEX IF NOT EXISTS idx_transactions_fk_account_pk_desc
  ON transactions (fk_account_id, pk_transaction_id DESC);

-- ============================================================
-- accounts
-- ============================================================
-- (pk_account_id is PRIMARY KEY – implicit index already exists)

CREATE INDEX IF NOT EXISTS idx_accounts_block_timestamp
  ON accounts (block_timestamp DESC);

CREATE INDEX IF NOT EXISTS idx_accounts_fio_balance_suf
  ON accounts (fio_balance_suf DESC);

CREATE INDEX IF NOT EXISTS idx_accounts_balance_id
  ON accounts (fio_balance_suf ASC, pk_account_id ASC);

CREATE INDEX IF NOT EXISTS idx_accounts_sorting_composite
  ON accounts (pk_account_id, account_name, fio_balance_suf, block_timestamp);

-- ============================================================
-- handles
-- ============================================================
-- (pk_handle_id is PRIMARY KEY – implicit index already exists)

CREATE INDEX IF NOT EXISTS handles_handle_idx
  ON handles (handle);

CREATE INDEX IF NOT EXISTS handles_handle_status_idx
  ON handles (handle_status);

CREATE INDEX IF NOT EXISTS handles_fk_domain_id_idx
  ON handles (fk_domain_id);

CREATE INDEX IF NOT EXISTS handles_domain_status_idx
  ON handles (fk_domain_id, handle_status);

CREATE INDEX IF NOT EXISTS idx_handles_fk_owner_account_id
  ON handles (fk_owner_account_id);

CREATE INDEX IF NOT EXISTS idx_handles_owner_pk_covering
  ON handles (fk_owner_account_id) INCLUDE (pk_handle_id);

CREATE INDEX IF NOT EXISTS idx_handles_owner_groupby
  ON handles (fk_owner_account_id, pk_handle_id);

-- ============================================================
-- domains
-- ============================================================
CREATE INDEX IF NOT EXISTS domains_domain_name_idx
  ON domains (domain_name);

CREATE INDEX IF NOT EXISTS domains_domain_status_idx
  ON domains (domain_status);

CREATE INDEX IF NOT EXISTS domains_status_public_idx
  ON domains (domain_status, is_public);

CREATE INDEX IF NOT EXISTS idx_domains_fk_owner_account_id
  ON domains (fk_owner_account_id);

CREATE INDEX IF NOT EXISTS idx_domains_owner_pk_covering
  ON domains (fk_owner_account_id) INCLUDE (pk_domain_id);

-- ============================================================
-- accountactivities
-- ============================================================
CREATE INDEX IF NOT EXISTS accountactivities_fk_account_id_idx
  ON accountactivities (fk_account_id);

CREATE INDEX IF NOT EXISTS idx_accountactivities_fk_transaction_id
  ON accountactivities (fk_transaction_id);

CREATE INDEX IF NOT EXISTS idx_accountactivities_account_transaction
  ON accountactivities (fk_account_id, fk_transaction_id);

-- ============================================================
-- handleactivities
-- ============================================================
CREATE INDEX IF NOT EXISTS handleactivities_fk_handle_id_idx
  ON handleactivities (fk_handle_id);

-- ============================================================
-- domainactivities
-- ============================================================
CREATE INDEX IF NOT EXISTS domainactivities_fk_domain_id_idx
  ON domainactivities (fk_domain_id);

-- ============================================================
-- tokentransfers
-- ============================================================
CREATE INDEX IF NOT EXISTS idx_tokentransfers_fk_transaction_id
  ON tokentransfers (fk_transaction_id);

CREATE INDEX IF NOT EXISTS idx_tokentransfers_fk_payer_account_id
  ON tokentransfers (fk_payer_account_id);

CREATE INDEX IF NOT EXISTS idx_tokentransfers_fk_payee_account_id
  ON tokentransfers (fk_payee_account_id);

-- Composite indexes for optimizing token transfer aggregation queries
CREATE INDEX IF NOT EXISTS idx_tokentransfers_txid_payer_composite
  ON tokentransfers (fk_transaction_id, fk_payer_account_id);

CREATE INDEX IF NOT EXISTS idx_tokentransfers_txid_payee_composite
  ON tokentransfers (fk_transaction_id, fk_payee_account_id);

-- Optimises: fk_payer_account_id = ? + GROUP BY fk_transaction_id
CREATE INDEX IF NOT EXISTS idx_tokentransfers_payer_txid_composite
  ON tokentransfers (fk_payer_account_id, fk_transaction_id);

-- Optimises: fk_payee_account_id = ? + JOIN … ON fk_transaction_id
CREATE INDEX IF NOT EXISTS idx_tokentransfers_payee_txid_composite
  ON tokentransfers (fk_payee_account_id, fk_transaction_id);

-- ============================================================
-- traces
-- ============================================================
CREATE INDEX IF NOT EXISTS idx_traces_fk_transaction_id
  ON traces (fk_transaction_id);

CREATE INDEX IF NOT EXISTS idx_traces_fk_action_account_id
  ON traces (fk_action_account_id);
