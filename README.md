# IntegralDistinguishersSolver
A tool to search the integral distinguisher of some block ciphers

该程序是使用vs2015进行编译运行。 如果试验者想运行该程序 ，建议下载该程序后使用vs2015 打开 ，可以直接编译运行

对于不同的密码算法，可以在main.cpp里直接 使用 

CipherBalancedBit ciphers;
ciphers.LblockBalancedBit();运行。

特别需要注意的是，对于simon，simeck和simon(102)算法,试验者需要手动到 Simon.cpp文件里的generate_new_simon方法中去修改移位数a,b,c. 
