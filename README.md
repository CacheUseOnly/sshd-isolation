# sshd-isolation

## What is sshd-isolation

The `sshd-isolation` repository aims to decouple libraries and dependent
programs into their own "cages" using a model similar to RPC.

## How does sshd-isolation work

Initially, a program would directly call functions from a shared object:

```
           +----------+                        +----------+
           |          | ---------------------> |          |
           | Program  |                        | Library  |
           |          | <--------------------- |          |
           +----------+                        +----------+
```

Now, an additional .so is set to override white-listed calls to the library, if
permitted, the call will be direct to the RPC modified library:

```
           +------------+                      +----------+
           |            | -------------------> |          |
           |libisolation|                      |RPC Server|
           |            | <------------------- |          |
           +------------+                      +----------+
                ^                                   ^
                | isolated calls                    |
                |                                   |
                v                                   v
           +----------+                        +----------+
           |          | ---------------------> |          |
           | Program  |   other calls          | Library  |
           |          | <--------------------- |          |
           +----------+                        +----------+
```

## Usage

The repository includes:
- **`server`**: A standalone executable created from `server.c`, acting as the
  server component.
- **`libisolation.so`**: A shared library generated from `client.c`, which can
  be used by other programs to leverage isolation functions.
- **`caged_prog`**: A test program created from `caged_prog.c`, used to validate
  the functionality of `libisolation.so`.

To build and use the components in this repository, you can follow these commands:

```bash
make
```

## Demo

### Demo program

1. In terminal 1, run `build/server` to start the server.
2. In terminal 2, run either of the following:
  a. `build/caged_prog add <a> <b>`: This routine adds `a` and `b`.
  b. `build/caged_prog cat <path_to_file>`: This routine print the first 8 character
  of the given file
  c. `build/caged_prog support`: This routine calls the caged version of
  `lzma_check_is_supported()`.

### Isolate xz

1. In terminal 1, run `build/server` to start the server.
2. In terminal 2, run `LD_PRELOAD=$(shell pwd)/build/lib/libisolation.so xz -z src/caged_prog.c -k`
  - You should see that the server prints log the call from `xz`.

## About ntirpc

libntirpc is the descendent of libtirpc (the extra 'n' stands for "new"), which
supports rpcgen to utilize the `AF_UNIX` family. However, its outdated
documentation, obscure function call, and uncleared unused methods implying that
this library is FOOBAR. The ntirpc/ implementation directory will be removed
soon.
