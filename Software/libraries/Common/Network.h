#ifndef NETWORK_H
#define NETWORK_H

class Network
{
public:
    //! Public constructor.
    Network();

    //! Public destructor.
    ~Network(){;}

    //! Initializes class.
    virtual void Initialize();

    //! Main state machine loop.
    virtual void Process();

protected:

};

#endif // NETWORK_H
