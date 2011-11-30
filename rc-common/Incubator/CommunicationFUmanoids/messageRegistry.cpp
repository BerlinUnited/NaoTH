#include "messageRegistry.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>


/*------------------------------------------------------------------------------------------------*/

/** Constructor
 */

MessageRegistry::MessageRegistry() {
	cs.setName("MessageRegistry");
}


/*------------------------------------------------------------------------------------------------*/

/** Destructor
 */

MessageRegistry::~MessageRegistry() {
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Register a callback for a message
 **
 ** @param callback        MessageCallback object to call back when message is received
 ** @param msgName         Name of message (case sensitive)
 **
 ** @return true iff registration succeeded
 */


/*------------------------------------------------------------------------------------------------*/

bool MessageRegistry::registerMessageCallback(MessageCallback* callback, const std::string &msgName) {
	std::string fullMsgName = "de.fumanoids.message." + msgName;
	MessageRegistration msgReg(fullMsgName, callback);
	registry[fullMsgName] = msgReg;
	return true;
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Unregister a callback for a message
 **
 ** @param callback        MessageCallback object that was registered to be called back
 ** @param msgName         Name of message (case sensitive)
 **
 ** @return true iff unregistration succeeded
**/

bool MessageRegistry::unregisterMessageCallback(MessageCallback* callback, const std::string &msgName) {
	CriticalSectionLock lock(cs);

	Registry::iterator it = registry.find(msgName);
	if (it == registry.end())
		return false;
	else {
		registry.erase(it);
		return true;
	}
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Handle a message and delegate calls to the callbacks that are affected.
 **
 ** @param msg         Message to handle
 ** @param senderID    ID of sender
 */

void MessageRegistry::handleMessage(const google::protobuf::Message &msg, int32_t senderID) {
	// extract all populated fields from the message
	std::vector<const google::protobuf::FieldDescriptor*> fields;
	msg.GetReflection()->ListFields(msg, &fields);

	// check each field for something that we know of
	for (uint32_t i = 0; i < fields.size(); i++) {
		Registry::iterator it = registry.find(fields[i]->full_name());
		if (it != registry.end()) {
			it->second.callback->messageCallback(it->first, msg.GetReflection()->GetMessage(msg,fields[i]), senderID);
			break;
		}
	}
}
