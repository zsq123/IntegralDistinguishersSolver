# IntegralDistinguishersSolver
A tool to search the integral distinguisher of some block ciphers

The program is compiled with vs2015, .If you want to run this program, it is recommended to download vs2015. You can run directly in main.cpp to test each cipher algorithms.such as:

CipherBalancedBit ciphers;

ciphers.LblockBalancedBit();。

For simon, simeck, and simon(192), please note that one should modify the shift parameters a,b,c of function generate_new_simon() manually, which is built in Simon.cpp.
