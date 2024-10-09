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

Now, an agent is set to intercept any call to the library, if permitted, the
call will be direct to the RPC modified library:

```
           +----------+       +--------+       +----------+
           |          | ----> |        | ----> |          |
           | Program  |       | Agent  |       | Library  |
           |          | <---- |        | <---- |          |
           +----------+       +--------+       +----------+
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
make all
```

## Demo

1. Change to `build/` directory: `cd build/`
2. In terminal 1, run `server` to start the server.
3. In terminal 2, run either of the following:
  a. `./caged_prog add <a> <b>`: This routine adds `a` and `b`.
  b. `./caged_prog cat <path_to_file>`: This routine print the first 8 character
  of the given file

## About ntirpc

libntirpc is the descendent of libtirpc (the extra 'n' stands for "new"), which
supports rpcgen to utilize the `AF_UNIX` family. However, its outdated
documentation, obscure function call, and uncleared unused methods implying that
this library is FOOBAR. The ntirpc/ implementation directory will be removed
soon.
