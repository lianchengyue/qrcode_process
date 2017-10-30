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

#include "ActiveMQProducer.h"

using namespace activemq;
using namespace activemq::core;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace std;

//#define MESSGAE_NUM 1
////////////////////////////////////////////////////////////////////////////////
ActiveMQProducer::ActiveMQProducer( const std::string& brokerURI, unsigned int numMessages,
                const std::string& destURI, bool useTopic, bool clientAck ) :
    connection(NULL),
    session(NULL),
    destination(NULL),
    producer(NULL),
    useTopic(useTopic),
    clientAck(clientAck),
    numMessages(numMessages),
    brokerURI(brokerURI),
    destURI(destURI) {
}

ActiveMQProducer::~ActiveMQProducer(){
    cleanup();
}

void ActiveMQProducer::close() {
    this->cleanup();
}

void ActiveMQProducer::run() {
    try {
        auto_ptr<ActiveMQConnectionFactory> connectionFactory(new ActiveMQConnectionFactory( brokerURI ) );

       try{
            connection = connectionFactory->createConnection();
            connection->start();
        } catch( CMSException& e ) {
            e.printStackTrace();
            throw e;
        }

        if( clientAck )
        {
            session = connection->createSession( Session::CLIENT_ACKNOWLEDGE );
        } else
        {
            session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
        }

        if( useTopic )
        {
            destination = session->createTopic( destURI );
        } else
        {
            destination = session->createQueue( destURI );
        }

        producer = session->createProducer( destination );
        producer->setDeliveryMode( DeliveryMode::NON_PERSISTENT );

        //string UploadText = (string)"{\"filename\":\"hu.png\",\"date\":\"2017-3-4\",\"size\":1077,\"md5sum\":\"d54646434003246e49eac0f2abe211a4\",\"type\": 1}";

        for( unsigned int ix=0; ix<numMessages; ++ix )
        {
            TextMessage* message = session->createTextMessage( UploadText );
            message->setIntProperty( "Integer", ix );

            //printf( "Sent message #%d\UploadText:%s\n", ix+1, UploadText.c_str());
            printf("UploadText:%s\n",UploadText.c_str());
            producer->send( message );
            delete message;
        }

    }catch ( CMSException& e ) {
        e.printStackTrace();
    }
}

void ActiveMQProducer::cleanup(){
    try{
        if( destination != NULL ) delete destination;
    }catch ( CMSException& e ) { e.printStackTrace(); }
    destination = NULL;

    try
    {
        if( producer != NULL ) delete producer;
    }catch ( CMSException& e ) { e.printStackTrace(); }
    producer = NULL;

    try{
        if( session != NULL ) session->close();
        if( connection != NULL ) connection->close();
    }catch ( CMSException& e ) { e.printStackTrace(); }

    try{
        if( session != NULL ) delete session;
    }catch ( CMSException& e ) { e.printStackTrace(); }
    session = NULL;

    try{
        if( connection != NULL ) delete connection;
    }catch ( CMSException& e ) { e.printStackTrace(); }
    connection = NULL;
}

string ActiveMQProducer::getUploadText()
{
    return UploadText;
}

int ActiveMQProducer::setUploadText(string input_text){
    UploadText = input_text;

    return 0;
}
