#pragma once

class NetworkManager
{
public:
	virtual void cleanup();

protected:
	virtual bool InitializeNetworking();
};

