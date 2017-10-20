#include "ActiveMQConsumer.h"
#include "JSON.h"

ActiveMQAsyncConsumer::~ActiveMQAsyncConsumer()
{
    cleanup();
}

void ActiveMQAsyncConsumer::close()
{
    cleanup();
}

void ActiveMQAsyncConsumer::runConsumer()
{
    try {

        // Create a ConnectionFactory
        ActiveMQConnectionFactory* connectionFactory =
            new ActiveMQConnectionFactory( brokerURI );

        // Create a Connection
        connection = connectionFactory->createConnection();
        delete connectionFactory;

        ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>( connection );
        if( amqConnection != NULL ) {
            amqConnection->addTransportListener( this );
        }

        connection->start();

        connection->setExceptionListener(this);

        // Create a Session
        if( clientAck ) {
            session = connection->createSession( Session::CLIENT_ACKNOWLEDGE );
        } else {
            session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
        }

        // Create the destination (Topic or Queue)
        if( useTopic ) {
            destination = session->createTopic( destURI );
        } else {
            destination = session->createQueue( destURI );
        }

        // Create a MessageConsumer from the Session to the Topic or Queue
        consumer = session->createConsumer( destination );
        consumer->setMessageListener( this );

    } catch (CMSException& e) {
        e.printStackTrace();
    }
}

// Called from the consumer since this class is a registered MessageListener.
void ActiveMQAsyncConsumer::onMessage( const Message* message )
{

        static int count = 0;
        try
        {
            count++;
            const TextMessage* textMessage =
                dynamic_cast< const TextMessage* >( message );
            string text = "";

            //wrong
            /*if( bytesMessage != NULL ) {
                text.assign((char *)(bytesMessage->getBodyBytes()),(bytesMessage->getBodyLength()));
            } else {
                text = "NOT A BYTESMESSAGE!";
            }*/

            if( textMessage != NULL ) {
                text = textMessage->getText();
            } else {
                text = "NOT A TEXTMESSAGE!";
            }

            if( clientAck ) {
                message->acknowledge();
            }

            readJSONMessage(text);
            //writeJSON();
            printf( "Message #%d Received: %s\n", count, text.c_str() );
        } catch (CMSException& e) {
            e.printStackTrace();
        }
    }


void ActiveMQAsyncConsumer::onException( const CMSException& ex AMQCPP_UNUSED )
{
    printf("CMS Exception occurred.  Shutting down client.\n");
    exit(1);
}

void ActiveMQAsyncConsumer::transportInterrupted()
{
    std::cout << "The Connection's Transport has been Interrupted." << std::endl;
}

void ActiveMQAsyncConsumer::transportResumed()
{
    std::cout << "The Connection's Transport has been Restored." << std::endl;
}



void ActiveMQAsyncConsumer::cleanup(){

        //*************************************************
        // Always close destination, consumers and producers before
        // you destroy their sessions and connection.
        //*************************************************

        // Destroy resources.
        try{
            if( destination != NULL ) delete destination;
        }catch (CMSException& e) {}
        destination = NULL;

        try{
            if( consumer != NULL ) delete consumer;
        }catch (CMSException& e) {}
        consumer = NULL;

        // Close open resources.
        try{
            if( session != NULL ) session->close();
            if( connection != NULL ) connection->close();
        }catch (CMSException& e) {}

        // Now Destroy them
        try{
            if( session != NULL ) delete session;
        }catch (CMSException& e) {}
        session = NULL;

        try{
            if( connection != NULL ) delete connection;
        }catch (CMSException& e) {}
        connection = NULL;
    }

void ActiveMQAsyncConsumer::start( const std::string& brokerURI,
                                    const std::string& destURI,
                                    bool useTopic = false,
                                    bool clientAck = false )
{
    this->connection = NULL;
    this->session = NULL;
    this->destination = NULL;
    this->consumer = NULL;
    this->useTopic = useTopic;
    this->brokerURI = brokerURI;
    this->destURI = destURI;
    this->clientAck = clientAck;
}


