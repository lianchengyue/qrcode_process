#ifndef ACTIVEMQCONSUMER_H
#define ACTIVEMQCONSUMER_H

#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Integer.h>
#include <activemq/util/Config.h>
#include <decaf/util/Date.h>
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

using namespace activemq;
using namespace activemq::core;
using namespace activemq::transport;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace std;


class ActiveMQAsyncConsumer : public ExceptionListener, public MessageListener, public DefaultTransportListener
{
public:
    ActiveMQAsyncConsumer(){}
    virtual ~ActiveMQAsyncConsumer();

    void start( const std::string& brokerURI, const std::string& destURI, bool useTopic, bool clientAck);

    void close();
    void runConsumer();
    virtual void onMessage( const Message* message );
    virtual void onException( const CMSException& ex AMQCPP_UNUSED );
    virtual void transportInterrupted();
    virtual void transportResumed();

private:
    void cleanup();

private:
    Connection* connection;
    Session* session;
    Destination* destination;
    MessageConsumer* consumer;
    bool useTopic;
    bool clientAck;
    std::string brokerURI;
    std::string destURI;
};

#endif // ACTIVEMQCONSUMER_H
