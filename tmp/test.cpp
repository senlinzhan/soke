#include <glog/logging.h>
#include <iostream>

class Name
{
public:
    Name()
    {        
    }

    void close()
    {
        LOG(FATAL) << "Haha\n";
    }
    
    ~Name()
    {
        std::cerr << "Hello, world" << std::endl;
    }
};


int main(int argc, char *argv[])
{
    Name n;
    n.close();
    
    return 0;
}


