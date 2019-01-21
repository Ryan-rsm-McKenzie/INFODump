#include "skse64/PluginAPI.h"  // PluginHandle, SKSEInterface, PluginInfo
#include "skse64_common/skse_version.h"  // RUNTIME_VERSION

#include <ShlObj.h>  // CSIDL_MYDOCUMENTS

#include "version.h"  // INFODUMP_VERSION_VERSTRING

#include "RE/TESDataHandler.h"  // TESDataHandler
#include "RE/TESTopic.h"  // TESTopic
#include "RE/TESTopicInfo.h"  // TESTopicInfo


static PluginHandle				g_pluginHandle = kPluginHandle_Invalid;
static SKSEMessagingInterface*	g_messaging = 0;


void MessageHandler(SKSEMessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSEMessagingInterface::kMessage_DataLoaded:
		{
			RE::TESDataHandler* dataHandler = RE::TESDataHandler::GetSingleton();
			for (auto& topic : dataHandler->topics) {
				_MESSAGE("DIAL: %08X", topic->formID);
				for (UInt32 i = 0; i < topic->infoCount; ++i) {
					_MESSAGE("INFO: %08X", topic->infoTopics[i]->formID);
				}
			}
		}
		break;
	}
}


extern "C" {
	bool SKSEPlugin_Query(const SKSEInterface* a_skse, PluginInfo* a_info)
	{
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\INFODump.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_DebugMessage);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		a_info->infoVersion = PluginInfo::kInfoVersion;
		a_info->name = "INFODump";
		a_info->version = INFODUMP_VERSION_MAJOR;

		g_pluginHandle = a_skse->GetPluginHandle();

		if (a_skse->isEditor) {
			_FATALERROR("[FATAL ERROR] Loaded in editor, marking as incompatible!\n");
			return false;
		}

		if (a_skse->runtimeVersion != RUNTIME_VERSION_1_5_62) {
			_FATALERROR("[FATAL ERROR] Unsupported runtime version %08X!\n", a_skse->runtimeVersion);
			return false;
		}

		return true;
	}


	bool SKSEPlugin_Load(const SKSEInterface* a_skse)
	{
		g_messaging = (SKSEMessagingInterface*)a_skse->QueryInterface(kInterface_Messaging);
		if (!g_messaging->RegisterListener(g_pluginHandle, "SKSE", MessageHandler)) {
			_FATALERROR("[FATAL ERROR] Messaging interface registration failed!\n");
			return false;
		}

		return true;
	}
};
