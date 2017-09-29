#ifndef CLIENTCALLBACK_H
#define CLIENTCALLBACK_H

typedef void (*client_buf_notify_t) (int frame_idx, void *user_data);

class ClientCbNotifier
{
//Q_OBJECT
public:
    ClientCbNotifier();
    virtual ~ClientCbNotifier();

    void *notifyCallback(/*qcamera_callback_args_t &cbArgs */ );

    void setCallbacks();

    //static void *cbNotifyRoutine(void *data);// in thread

private:

};


#endif // CLIENTCALLBACK_H
