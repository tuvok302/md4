## Synopsis

DO NOT USE IN PRODUCTION ENVIRONMENTS.  This is a C++ implemenation of the MD4 hashing function that 
allows you to specify the round and operation step.  

## Motivation

I built this MD4 hashing algorithm to attempt to recreate [Wang's](https://link.springer.com/chapter/10.1007%2F11426639_1) collision attack on MD4.

## Tests

Check RFC1320 for reference strings and output hashes. 

## TODO

Break into separate files

Make it more easily embeddable in other projects

Start working on Wang's attack

## License

GPLv3
