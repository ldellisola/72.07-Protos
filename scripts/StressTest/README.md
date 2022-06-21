# StressTest

This is a small program designed to stress test a socks5 server. It will spawn a given amount of concurrent users that will do  N request per second to a target address through a Socks5 proxy server for a given amount of time.

## Installation

The program is releases as a self-contained executable and you can find it in the folder `./exe/`.

This executable should be universal to all linux x64 distributions.

## Usage

```bash
$ ./exe/StressTest [options]

Options:
  -p, --proxy <proxy>                              Proxy to test [default: socks5://localhost:1080]
  -r, --runtime <runtime>                          Time to run the test [default: 60]
  -rps, --request-per-second <request-per-second>  The number of requests per second each user will do
                                                   [default: 2]
  -u, --users <users>                              The number of users to run concurrently [default: 2]
  -t, --target <target> (REQUIRED)                 The address to make all the calls to
  --version                                        Show version information
  -?, -h, --help                                   Show help and usage information
```

After the requested time passes, it will print a small results view with the total attempts to connect and the times the connection failed.

## Examples

To run `3` concurrent users doing each `5` requests per second to `www.google.com` using a socks5 proxy hosted on the address `127.0.0.1` and port `8001`  with the user `usr` and the password `psd`, for 60 seconds:

```bash
$ ./exe/StressTest -u 3 -rps 5 -p socks5://usr:psd@127.0.0.1:8001 -t www.google.com -r 60
-----------------------------------------------
Starting stress test!
Expected runtime:	60 seconds
Number of users:	3
Expected requests per second:	5
Target Url:	www.google.com
Socks5 server:	socks5://usr:psd@127.0.0.1:8001
-----------------------------------------------
Starting up user...
Starting up user...
Starting up user...
Stopping workers!
Shutting down user...
Shutting down user...
Shutting down user...
-----------------------------------------------
Stress test finished!
Total attempts: 12321
Total errors: 0
```

## Compilation

To compile this program in a linux distribution, it is a requirement to have the .NET6 SDK installed. 

You can see how to do it for your own distro [here](https://docs.microsoft.com/en-us/dotnet/core/install/linux).

Then, it is as simple as publishing the binaries:

```bash
$ dotnet publish --sc -o ./exe/ -c Release -r linux-x64
```

After this command finishes, a binary called `StressTest` will be created in the `./exe/` folder. 

To build the binaries for other platforms, check out the [supported runtimes](https://docs.microsoft.com/en-us/dotnet/core/rid-catalog). You can swap any of them with the  `-r ` argument.