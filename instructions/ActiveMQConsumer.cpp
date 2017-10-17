#include "ActiveMQConsumer.h"




#if 0
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
#include <unistd.h>  //sleep()

#include "ActiveMQConsumer.h"

using namespace activemq;
using namespace activemq::core;
using namespace activemq::transport;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace std;


ActiveMQConsumer::ActiveMQConsumer( const std::string& brokerURI,
                     const std::string& destURI,
                     bool useTopic,
                     bool clientAck ) :
    connection(NULL),
    session(NULL),
    destination(NULL),
    consumer(NULL),
    useTopic(useTopic),
    brokerURI(brokerURI),
    destURI(destURI),
    clientAck(clientAck) {
}

ActiveMQConsumer::~ActiveMQConsumer(){
    cleanup();
}

void ActiveMQConsumer::close() {
    this->cleanup();
}

void ActiveMQConsumer::runConsumer() {

    try {
        ActiveMQConnectionFactory* connectionFactory = new ActiveMQConnectionFactory( brokerURI );
        connection = connectionFactory->createConnection();
        delete connectionFactory;

        ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>( connection );
        if( amqConnection != NULL )
        {
            amqConnection->addTransportListener( this );
        }

        connection->start();
        connection->setExceptionListener(this);

        if( clientAck ) {
            session = connection->createSession( Session::CLIENT_ACKNOWLEDGE );
        } else {
            session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
        }

        if( useTopic ) {
            destination = session->createTopic( destURI );
        } else {
            destination = session->createQueue( destURI );
        }

        consumer = session->createConsumer( destination );
        consumer->setMessageListener( this );

    } catch (CMSException& e) {
        e.printStackTrace();
    }
}

void ActiveMQConsumer::onMessage( const Message* message ) {
    static int count = 0;
    try
    {
        count++;
        const TextMessage* textMessage =
            dynamic_cast< const TextMessage* >( message );
        string text = "";

        if( textMessage != NULL ) {
            text = textMessage->getText();
        } else {
            text = "NOT A TEXTMESSAGE!";
        }

        if( clientAck ) {
            message->acknowledge();
        }

        printf( "Message #%d Received: %s\n", count, text.c_str() );
    } catch (CMSException& e) {
        e.printStackTrace();
    }
}

void ActiveMQConsumer::onException( const CMSException& ex AMQCPP_UNUSED ) {
    printf("CMS Exception occurred.  Shutting down client.\n");
    exit(1);
}

void ActiveMQConsumer::transportInterrupted() {
    std::cout << "The Connection's Transport has been Interrupted." << std::endl;
}

void ActiveMQConsumer::transportResumed() {
    std::cout << "The Connection's Transport has been Restored." << std::endl;
}

void ActiveMQConsumer::cleanup(){
    try{
        if( destination != NULL ) delete destination;
    }catch (CMSException& e) {}
    destination = NULL;

    try{
        if( consumer != NULL ) delete consumer;
    }catch (CMSException& e) {}
    consumer = NULL;

    try{
        if( session != NULL ) session->close();
        if( connection != NULL ) connection->close();
    }catch (CMSException& e) {}

    try{
        if( session != NULL ) delete session;
    }catch (CMSException& e) {}
    session = NULL;

    try{
        if( connection != NULL ) delete connection;
    }catch (CMSException& e) {}
    connection = NULL;
}
#endif
