#ifndef ACTIVEMQPRODUCER_H
#define ACTIVEMQPRODUCER_H

#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Long.h>
#include <decaf/util/Date.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Config.h>
#include <activemq/library/ActiveMQCPP.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <memory>

using namespace activemq;
using namespace activemq::core;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace std;

#define MESSGAE_NUM 1
////////////////////////////////////////////////////////////////////////////////
class ActiveMQProducer : public Runnable {

    private:
    Connection* connection;
    Session* session;
    Destination* destination;
    MessageProducer* producer;
    bool useTopic;
    bool clientAck;
    unsigned int numMessages;
    std::string brokerURI;
    std::string destURI;

    std::string UploadText;

    private:
    ActiveMQProducer( const ActiveMQProducer& );
    ActiveMQProducer& operator= ( const ActiveMQProducer& );

    public:
    ActiveMQProducer( const std::string& brokerURI, unsigned int numMessages,
                    const std::string& destURI, bool useTopic = false, bool clientAck = false );
    string getUploadText();
    int setUploadText(string input_text);

    virtual ~ActiveMQProducer();
    void close();
    virtual void run();

    private:
    void cleanup();

};

#endif //ACTIVEMQPRODUCER_H
