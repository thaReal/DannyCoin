DannyCoin (DAN)
===============

An[other] experimental cryptocurrency with the goal of providing some innovative
control on currency distribution. At the moment, the source code is based off of
Smallchange (commit a19eb2dc3115a708e3a7f1c5855937aa33f1e132). WARNING: source
code is still under construction and is in no way suitable for use for any 
purpose whatsoever at this point.


Initial Parameters
------------------
*TO BE UPDATED; still default values from smallchange source*

DannyCoin (DAN) - a 'faster' version of Litecoin which also uses scrypt
as a proof of work scheme and is intended for microtransactions.
 - 15 seconds block targets: beat that MinCoin! ;)
 - 42 007 680 total coins
 - no subsidy within the first 3 days and after approximately 5 years;
    in between: 4 coins per generated block
 - difficulty retargets every 0.35 days
 - currently peers are looked up over IRC only
 - currently no block checkpoints are in the code (but could be easily
   added)

Other than that, this coin is exactly like Litecoin and should by no
means be used as a real cryptocurrency. All of the coin parameters
are chosen arbitrarily or at most with 'fairness' towards everyone in mind.

Credits go to the original authors/communities that created Bitcoin and 
Litecoin.


To Do List:
-----------
-> Update IRC address/channel (do I need a private irc server?)
-> Create initial block reward scheme to be used for testnet
-> Update/Tweak difficulty retarget algorithm - check testnet settings since 
   that's where we'll be doing all initial testing
-> Create a seednode(s)
-> Sooo much more...