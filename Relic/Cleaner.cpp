//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include <stdio.h>
#include <string.h>
#include <chrono>
#include <thread>

#include "Cleaner.h"

void Cleaner::Run()
{
	Database::Initialize();
	sth_get_min_irrev = connection->prepareStatement("SELECT MIN(irreversible) FROM SYNC");
	sth_get_min_tx_block = connection->prepareStatement("SELECT MIN(block_num) FROM TRANSACTIONS");
	sth_prune_transactions = connection->prepareStatement("DELETE FROM TRANSACTIONS WHERE block_num < ?");
	sth_prune_receipts = connection->prepareStatement("DELETE FROM RECEIPTS WHERE block_num < ?");

	keepBlocks = keepDays * 24 * 7200;

	for (;; std::this_thread::sleep_for(std::chrono::seconds(10)))
	{
		sql::ResultSet* r = sth_get_min_irrev->executeQuery();
		//connection->commit();!!!by default Autocommit is enabled
		r->next();
		//r->isAfterLast();
		int minIrrev = r->getInt(1);
		if (lastIrrev >= minIrrev)
		{
			StdOut(Warning, "Nothing to delete");
			continue;
		}

		lastIrrev = minIrrev;
		int	uptoBlock = lastIrrev - keepBlocks;
		r = sth_get_min_tx_block->executeQuery();
		r->next();
		int minBlock = r->getInt(1);
		if (minBlock < uptoBlock)
			StdOut(Error, "Pruning %d blocks", uptoBlock - minBlock);

		while (minBlock < uptoBlock)
		{
			int deleteUpto = minBlock + 10;
			if (deleteUpto > uptoBlock)
				deleteUpto = uptoBlock;

			StdOut(Error, "Deleting blocks < %d", deleteUpto);
			sth_prune_transactions->setInt(1, deleteUpto);
			sth_prune_transactions->executeQuery();
			sth_prune_receipts->setInt(1, deleteUpto);
			sth_prune_receipts->executeQuery();
			sth_prune_receipts->executeQuery();
			//$db->{'dbh'}->commit();!!!by default Autocommit is enabled

			minBlock = deleteUpto;
		}
	}
}

void Cleaner::Close()
{
	Database::Close();
}