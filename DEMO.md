# demo.c Description

`demo.c` is a small smoke-test program for the `u64` library. It demonstrates the normal lifecycle of an in-memory database: initialize, create records, read a record, update it, delete it, read by sorted index, write a snapshot, and load that snapshot into a second database.

What it tests:

- **Initialization:** Allocates a `Database`, calls `init_database(db)`, then prints the initial size, expected to be `0`.

- **Create:** Inserts key `100` with bytes `01 02 03 04`, then key `200` with bytes `11 12 13 14`. It checks behavior informally by printing `getsize_db(db)` after each insert.

- **Read by key:** Reads key `100` with `read_record`, then prints the returned bytes in hex.

- **Update:** Updates key `100` to a shorter byte value, `21 22 23`, reads it again, and prints the new value.

- **Delete:** Deletes key `100`, then prints the database size. Since key `200` remains, the expected size is `1`.

- **Read by index:** Calls `read_record_by_index(db, 0)`. Because key `100` was deleted, index `0` should now refer to the remaining record, key `200`, so it prints `11 12 13 14`.

- **Persistence:** Writes the current database to `database.dat`, creates a second database, loads `database.dat` into it, and prints the loaded size.

It is not a formal test suite: it does not assert expected values or return failure on mismatches. It verifies behavior by exercising the API and printing the observed results.
