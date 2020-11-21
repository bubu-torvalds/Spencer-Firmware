#ifndef SPENCER_NET_H
#define SPENCER_NET_H

#include <WiFi.h>
#include <Loop/LoopListener.h>

class NetStateListener {
private: virtual void state(wl_status_t) = 0;
friend class NetImpl;
};

class NetImpl : public LoopListener {
public:
	NetImpl();
	void set(const char* ssid, const char* pass);

	/**
	 * Connects to the WiFi in a non-blocking fashion, in the loop thread. 3 tries, 5s timeout.
	 * Starts the WiFi animation. Will not trigger disconnect callbacks/listeners. Always check
	 * Net.connected() before making network requests.
	 *
	 * @see Net.connect()
	 */
	void connect();

	void loop(uint micros) override;

	/**
	 * A void function that takes a wl_status_t argument.
	 */
	typedef void (NetStateCallback)(wl_status_t);

	/**
	 * Register a state callback. This will not trigger when the network goes down for reconnection.
	 * Always check Net.connected() before making network requests.
	 * @param callback Will be called when connection goes up or down.
	 */
	void addStateCallback(NetStateCallback* callback);

	/**
	 * Register a state listener. This will not trigger when the network goes down for reconnection.
	 * Always check Net.checkConnection() before making network requests.
	 * @param NetEventListener::state will be called when connection goes up or down.
	 */
	void addStateListener(NetStateListener* listener);

	/**
	 * Checks if a working Internet connection is established. First, by checking if the WiFi is connected,
	 * and then trying to establish a connection to spencer.circuitmess.com. This is a blocking function.
	 * @return True is all OK, false otherwise
	 */
	bool checkConnection();

	/**
	 * Reconnects the WiFi in a blocking fashion. 2 tries, 5s timeout
	 * @return True if succeeded, false otherwise
	 */
	bool reconnect();

private:
	const char* ssid;
	const char* pass;

	std::vector<NetStateListener*> stateListeners;
	std::vector<NetStateCallback*> stateCallbacks;

	void setState(wl_status_t state);

	void tryConnect();
	void retryConnect();
	bool connecting = false;
	uint connectTime;
	uint8_t connectRetries;

	wl_status_t state = WL_DISCONNECTED;
};

extern NetImpl Net;

#endif //SPENCER_NET_H