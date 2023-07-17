# Note

Currently WIP

# Roadmap

## Working

- [x] ICMP echo request sending
- [x] ICMP echo reply receiving (and parsing)
- [x] Use `usleep` to not spam the network
- [x] Checksum (re)calculation
- [x] Get the correct data size for the logging
- [x] Get the correct response time
- [x] CTRL+C summary
- [X] Collect the correct `ttl`
- [x] Fix response time's inaccuracy
- [x] Setup `alarm()` & `signal` handler to timeout pings

## TODO

- [ ] Verification of ICMP version & other frame data
- [ ] Clean the code before implementing fancy stuff

# Useful resources

## RFCs

### RFC 792

[Internet Control Message Protocol](https://tools.ietf.org/html/rfc792)

### RFC 1071

[Computing the Internet checksum](https://tools.ietf.org/html/rfc1071)

## Wikipedia articles

### Ping (networking utility)

https://en.wikipedia.org/wiki/Ping_(networking_utility)

### Internet Control Message Protocol

https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol


## Other

[Charles M. Kozierok's TCP/IP Guide](http://www.tcpipguide.com/free/t_ICMPv4EchoRequestandEchoReplyMessages-2.htm)
